#include <time.h>
#include <stdio.h>   
#include <windows.h>  
#include <setupapi.h>
#include "common.h"
#include "EditableButtonUI.h"
#include "MyImageUI.h"
#include "ModeButtonUI.h"

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ole32.lib")



CGlobalData  g_data;
std::vector<TArea *>  g_vArea;
DWORD g_ReaderIndicator[MAX_READERS_PER_GRID] 
	= { 0xFF00FF00, 0xFF1b9375, 0xFF00FFFF, 0xFF51786C, 0xFFFFFF00, 0xFFCA5100 };

const char * g_BodyParts[MAX_READERS_PER_GRID] = { "左腋下", "右腋下", "左腹股沟", "右腹股沟", "前胸", "后背" };
const char * g_BodyPart = "腋下";

CControlUI*  CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	DuiLib::CDialogBuilder builder;
	DuiLib::CDuiString  strText;
	DuiLib::CControlUI * pUI = 0;

	if ( 0 == strcmp(pstrClass, "GridUI") ) {
		strText.Format("%s.xml", pstrClass);   
		pUI = builder.Create((const char *)strText, (UINT)0, this, m_pManager);
		return pUI;         
	}
	else if (0 == strcmp(pstrClass, "MaxiumUI")) {
		strText.Format("%s.xml", pstrClass); 
		pUI = builder.Create((const char *)strText, (UINT)0, this, m_pManager);
		return pUI;
	}
	else if (0 == strcmp(pstrClass, "EditableButton")) {  
		return new CEditableButtonUI; 
	}
	else if (0 == strcmp(pstrClass, "MyImage")) {
		return new CMyImageUI;
	}
	else if (0 == strcmp(pstrClass, "ModeButton")) {
		return new CModeButton;
	}
	return NULL;
}

LONG WINAPI pfnUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	if (IsDebuggerPresent())
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	typedef BOOL(WINAPI* MiniDumpWriteDumpT)(
		HANDLE,
		DWORD,
		HANDLE,
		MINIDUMP_TYPE,
		PMINIDUMP_EXCEPTION_INFORMATION,
		PMINIDUMP_USER_STREAM_INFORMATION,
		PMINIDUMP_CALLBACK_INFORMATION);

	HMODULE hDbgHelp = LoadLibrary("dbghelp.dll");
	if (NULL == hDbgHelp)
	{
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	SYSTEMTIME stSysTime;
	memset(&stSysTime, 0, sizeof(SYSTEMTIME));
	GetLocalTime(&stSysTime);

	TCHAR szFile[MAX_PATH] = { 0 };		//根据字符集，有时候可能为WCHAR
	wsprintf(szFile, "%0.4d-%0.2d-%0.2d-%0.2d-%0.2d-%0.2d-%0.3d.dmp", \
		stSysTime.wYear, stSysTime.wMonth, stSysTime.wDay, stSysTime.wHour, \
		stSysTime.wMinute, stSysTime.wSecond, stSysTime.wMilliseconds);

	HANDLE hFile = CreateFile(szFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, \
		0, CREATE_ALWAYS, 0, 0);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		MINIDUMP_EXCEPTION_INFORMATION objExInfo;
		objExInfo.ThreadId = ::GetCurrentThreadId();
		objExInfo.ExceptionPointers = pExceptionInfo;
		objExInfo.ClientPointers = NULL;

		BOOL bOk = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, \
			MiniDumpWithDataSegs, (pExceptionInfo ? &objExInfo : NULL), NULL, NULL);
		CloseHandle(hFile);
	}

	FreeLibrary(hDbgHelp);

	return EXCEPTION_EXECUTE_HANDLER;
}

DWORD GetGridOrderByGridId(DWORD  dwId) {
	for ( DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++ ) {
		if ( g_data.m_CfgData.m_GridOrder[i] == dwId ) {
			return i;
		}
	}
	assert(0);
	return -1;
}

char * GetDefaultGridOrder(char * buf, DWORD  dwBufLen, DWORD  dwGridsCnt) {
	assert(buf);
	assert(dwGridsCnt > 0);

	CDuiString  strText;
	for (DWORD i = 0; i < dwGridsCnt; i++) {
		CDuiString strTmp;
		strTmp.Format("%lu", i+1);

		if (i > 0)
			strText += ",";

		strText += strTmp;
	}

	STRNCPY(buf, strText, dwBufLen);
	return buf;
}

char * GetGridOrder(char * buf, DWORD  dwBufLen, DWORD  dwGridsCnt, DWORD * pOrder) {
	assert(buf);
	assert(dwGridsCnt > 0);

	CDuiString  strText;
	for (DWORD i = 0; i < dwGridsCnt; i++) {
		CDuiString strTmp;
		strTmp.Format("%lu", pOrder[i]+1);

		if (i > 0)
			strText += ",";

		strText += strTmp;
	}

	STRNCPY(buf, strText, dwBufLen);
	return buf;
}

char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d:%02d:%02d", tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	return szDest;
}

char * Date2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d-%02d-%02d", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday);
	return szDest;
}

char * DateTime2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d-%02d-%02d %02d:%02d:%02d", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday,
		tmp.tm_hour, tmp.tm_min, tmp.tm_sec );
	return szDest;
}

void   ResetGridOrder() {
	for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
		g_data.m_CfgData.m_GridOrder[i] = i;
	}
}

time_t  GetTodayZeroTime() {
	time_t now = time(0);

	struct tm tTmTime;
	localtime_s(&tTmTime, &now);

	tTmTime.tm_hour = 0;
	tTmTime.tm_min = 0;
	tTmTime.tm_sec = 0;

	return mktime(&tTmTime);
}

static BOOL EnumPortsWdm(std::vector<std::string> & v)
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

DWORD   GetCollectInterval(DWORD dwIndex) {
	switch (dwIndex)
	{
	case 0:
		return 10;
	case 1:
		return 60;
	case 2:
		return 300;
	case 3:
		return 900;
	case 4:
		return 1800;
	case 5:
		return 3600;
	default:
		break;
	}
	return 10;
}

void  SaveReaderSwitch(DWORD i, DWORD j) {
	CDuiString  strText;
	strText.Format("%s %lu %lu", CFG_READER_SWITCH, i + 1, j + 1);
	BOOL  bValue = DEFAULT_READER_SWITCH;
	g_data.m_cfg->SetBooleanConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch, &bValue);
	g_data.m_cfg->Save();
}

void  SaveReaderName(DWORD i, DWORD j) {
	CDuiString  strText;
	strText.Format("%s %lu %lu", CFG_READER_NAME, i + 1, j + 1);
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_szReaderName, 0);
	g_data.m_cfg->Save();
}

char *  GetTagId(char * szTagId, DWORD dwTagIdLen, const BYTE * pData, DWORD dwDataLen) {
	if (0 == szTagId || 0 == dwTagIdLen) {
		return 0;
	}

	if (dwDataLen != 8) {
		szTagId[0] = '\0';
		return szTagId;
	}

	if (dwTagIdLen < 16) {
		szTagId[0] = '\0';
		return szTagId;
	}

	for (DWORD i = 0; i < dwDataLen; i++) {
		SNPRINTF(szTagId + 2 * i, 3, "%02x", pData[dwDataLen - i - 1]);
	}

	return szTagId;
}

char *  GetSurReaderId(char * szReaderId, DWORD dwReaderIdLen, const BYTE * pData, DWORD dwDataLen) {
	if (0 == szReaderId || 0 == dwReaderIdLen) {
		return 0;
	}

	if (dwDataLen != 11) {
		szReaderId[0] = '\0';
		return szReaderId;
	}

	if (dwReaderIdLen < 12) {
		szReaderId[0] = '\0';
		return szReaderId;
	}

	szReaderId[0] = pData[0];
	szReaderId[1] = pData[1];
	szReaderId[2] = pData[2] + '0';
	szReaderId[3] = pData[3] + '0';
	szReaderId[4] = pData[4] + '0';
	szReaderId[5] = '0';

	for (int i = 0; i < 6; i++) {
		szReaderId[i + 6] = pData[i + 5] + '0';
	}

	return szReaderId;
}