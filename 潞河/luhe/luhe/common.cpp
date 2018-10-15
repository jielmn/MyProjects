#include <time.h>
#include <windows.h>  
#include <setupapi.h>
#include <locale.h>
#include <assert.h>
#include "common.h"

CGlobalData  g_data;

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
			// 如果是第一次记录 comport
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

BOOL  CheckComPortExist(int nCheckComPort) {
	std::vector<std::string> vComPorts;
	EnumPortsWdm(vComPorts);

	char buf[8192];
	std::vector<std::string>::iterator it;
	for (it = vComPorts.begin(); it != vComPorts.end(); it++) {
		std::string & s = *it;
		Str2Lower(s.c_str(), buf, sizeof(buf));

		int nComPort = 0;
		const char * pFind = strstr(buf, "com");
		while (pFind) {
			if (1 == sscanf(pFind + 3, "%d", &nComPort)) {
				break;
			}
			pFind = strstr(pFind + 3, "com");
		}
		assert(nComPort > 0);

		if (nCheckComPort == nComPort) {
			return TRUE;
		}
	}

	return FALSE;
}