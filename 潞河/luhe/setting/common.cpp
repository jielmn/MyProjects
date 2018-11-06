#include <time.h>
#include <windows.h>  
#include <setupapi.h>
#include <locale.h>
#include <assert.h>
#include "common.h"
#include "UIlib.h"
using namespace DuiLib;

CGlobalData  g_data;
std::vector<TArea *>  g_vArea;
IConfig * g_cfg_area = 0;

BOOL EnumPortsWdm(std::vector<std::string> & v)
{
	// Create a device information set that will be the container for 
	// the device interfaces.
	GUID *guidDev = (GUID*)&GUID_CLASS_COMPORT;
	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;

	hDevInfo = SetupDiGetClassDevs(guidDev,
		NULL,
		NULL,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
	);

	if (INVALID_HANDLE_VALUE == hDevInfo)
	{
		return FALSE;
	}

	// Enumerate the serial ports
	BOOL bOk = TRUE;
	SP_DEVICE_INTERFACE_DATA ifcData;
	DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
	pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)new char[dwDetDataSize];
	if (!pDetData)
	{
		return FALSE;
	}
	// This is required, according to the documentation. Yes,
	// it's weird.
	ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
	for (DWORD ii = 0; bOk; ii++)
	{
		bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
			NULL, guidDev, ii, &ifcData);
		if (bOk)
		{
			// Got a device. Get the details.
			SP_DEVINFO_DATA devdata = { sizeof(SP_DEVINFO_DATA) };
			bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
				&ifcData, pDetData, dwDetDataSize, NULL, &devdata);
			if (bOk) {
				// Got a path to the device. Try to get some more info.
				CHAR fname[256] = { 0 };
				CHAR desc[256] = { 0 };
				BOOL bSuccess = SetupDiGetDeviceRegistryProperty(
					hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
					(PBYTE)fname, sizeof(fname), NULL);
				bSuccess = bSuccess && SetupDiGetDeviceRegistryProperty(
					hDevInfo, &devdata, SPDRP_DEVICEDESC, NULL,
					(PBYTE)desc, sizeof(desc), NULL);
				BOOL bUsbDevice = FALSE;
				CHAR locinfo[256] = { 0 };
				if (SetupDiGetDeviceRegistryProperty(
					hDevInfo, &devdata, SPDRP_LOCATION_INFORMATION, NULL,
					(PBYTE)locinfo, sizeof(locinfo), NULL))
				{

					bUsbDevice = (strncmp(locinfo, "USB", 3) == 0);
				}
				if (bSuccess)
				{
					//printf("FriendlyName = %S\r\n", fname);
					//printf("Port Desc = %S\r\n", desc);

					v.push_back(fname);
				}

			}
			else
			{
				if (pDetData != NULL)
				{
					delete[](char*)pDetData;
				}
				if (hDevInfo != INVALID_HANDLE_VALUE)
				{
					SetupDiDestroyDeviceInfoList(hDevInfo);
				}
				return FALSE;
			}
		}
		else
		{
			DWORD err = GetLastError();
			if (err != ERROR_NO_MORE_ITEMS)
			{
				if (pDetData != NULL)
				{
					delete[](char*)pDetData;
				}
				if (hDevInfo != INVALID_HANDLE_VALUE)
				{
					SetupDiDestroyDeviceInfoList(hDevInfo);
				}
				return FALSE;
			}
		}
	}

	if (pDetData != NULL)
	{
		delete[](char*)pDetData;
	}
	if (hDevInfo != INVALID_HANDLE_VALUE)
	{
		SetupDiDestroyDeviceInfoList(hDevInfo);
	}

	return TRUE;
}

int  GetCh340Count(char * szComPort, DWORD dwComPortLen) {
	std::vector<std::string> vComPorts;
	EnumPortsWdm(vComPorts);

	char buf[8192];
	int nFindCount = 0;
	std::vector<std::string>::iterator it;
	for (it = vComPorts.begin(); it != vComPorts.end(); it++) {
		std::string & s = *it;
		Str2Lower(s.c_str(), buf, sizeof(buf));

		if (0 != strstr(buf, "usb-serial ch340")) {
			// ����ǵ�һ�μ�¼ comport
			if (0 == nFindCount) {
				int nComPort = 0;
				const char * pFind = strstr(buf, "com");
				while (pFind) {
					if (1 == sscanf(pFind + 3, "%d", &nComPort)) {
						break;
					}
					pFind = strstr(pFind + 3, "com");
				}
				assert(nComPort > 0);
				SNPRINTF(szComPort, dwComPortLen, "com%d", nComPort);
			}
			nFindCount++;
		}
	}
	return nFindCount;
}

static DWORD  FindMaxAreaId(const std::vector<TArea *> & v) {
	DWORD  dwMax = 0;
	std::vector<TArea *>::const_iterator  it;
	for (it = v.begin(); it != v.end(); ++it) {
		TArea * pArea = *it;
		if (pArea->dwAreaNo > dwMax) {
			dwMax = pArea->dwAreaNo;
		}
	}
	return dwMax + 1;
}

DWORD  FindNewAreaId(const std::vector<TArea *> & v) {
	DWORD dwMaxId = FindMaxAreaId(v);
	if (dwMaxId <= 100) {
		return dwMaxId;
	}

	for (DWORD i = 1; i <= 100; i++) {
		std::vector<TArea *>::const_iterator  it;
		for (it = v.begin(); it != v.end(); ++it) {
			TArea * pArea = *it;
			if (pArea->dwAreaNo == i) {
				break;
			}
		}
		// ���û���ҵ���ͬ��id���������id
		if (it == v.end()) {
			return i;
		}
	}
	return -1;
}

void  SaveAreas() {
	g_cfg_area->ClearConfig();

	std::vector<TArea *>::iterator it;
	int i = 0;
	DuiLib::CDuiString strText;

	for (it = g_vArea.begin(); it != g_vArea.end(); it++, i++) {
		TArea * pArea = *it;
		strText.Format("area no %d", i + 1);
		g_cfg_area->SetConfig(strText, pArea->dwAreaNo);
		strText.Format("area name %d", i + 1);
		g_cfg_area->SetConfig(strText, pArea->szAreaName);
	}

	g_cfg_area->Save();
}