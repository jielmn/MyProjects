#include <time.h>
#include "common.h"
#include <setupapi.h>
#include <locale.h>
#include "resource.h"
#include "CustomControls.h"

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

const char * GetMachineType(MachineType e) {
	switch (e)
	{
	case MachineType_MR:
		return "MR";
	case MachineType_FLP:
		return "飞利浦";
	case MachineType_GE:
		return "GE";
	case MachineType_GE2:
		return "GE2";
	default:
		break;
	}
	return "未知机器";
}

BOOL LoadStandardRes() {
	int ret = 0;
	int ids[MAX_MACHINE_CNT] = { IDR_TXT1 ,IDR_TXT2 ,IDR_TXT3 ,IDR_TXT4 };
	for ( int i = 0; i < MAX_MACHINE_CNT; i++ ) {
		HRSRC hResource = ::FindResource(0, MAKEINTRESOURCE(ids[i]), "TXT");
		if (hResource == 0)
			return FALSE;

		// 加载资源
		HGLOBAL hg = LoadResource(0, hResource);
		if ( hg == 0 )
			return FALSE;

		// 锁定资源
		LPVOID pData = LockResource(hg);
		if (pData == 0)
			return FALSE;

		// 获取资源大小
		DWORD dwSize = SizeofResource(0, hResource);
		char buf[8192];
		assert(dwSize < 8192);
		if ( dwSize >= 8192 )
			return FALSE;

		memcpy(buf, pData, dwSize);
		buf[dwSize] = '\0';

		SplitString  s;
		s.Split(buf, '\n');
		DWORD  dwItemsCnt = s.Size();
		for ( DWORD j = 0; j < dwItemsCnt; j++ ) {
			SplitString s1;
			s1.SplitByBlankChars(s[j]);
			DWORD dwSubCnt = s1.Size();
			if (dwSubCnt == 2) {
				float fValue = 0.0f;
				ret = sscanf_s(s1[0], "%f", &fValue);
				assert(1 == ret);
				if (ret != 1)
					return FALSE;

				g_data.m_standard_items[i][j].m_nTemp = (int)round( fValue * 100.0f );
				if (g_data.m_standard_items[i][j].m_nTemp != FIRST_TEMP + j * 10)
					return FALSE;

				ret = sscanf_s(s1[1], "%d", &g_data.m_standard_items[i][j].m_nDutyCycle);
				assert(1 == ret);
				if (ret != 1)
					return FALSE;
			}
		}
	}
	
	return TRUE;
}

CControlUI*  CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	DuiLib::CDialogBuilder builder;
	DuiLib::CDuiString  strText;
	DuiLib::CControlUI * pUI = 0;

	if (0 == strcmp(pstrClass, "MyButton")) {
		return new CMyButtonUI; 
	}
	return NULL;
}