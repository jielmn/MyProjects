#include <time.h>
#include <stdio.h>   
#include <windows.h>  
#include <setupapi.h>
#include "common.h"
#include "EditableButtonUI.h"
#include "MyImageUI.h"
#include "ModeButtonUI.h"
#include "resource.h"
#include "SixGridsUI.h"
#include "LmnGdi.h"
#include "WaitingBarUI.h"

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ole32.lib")

typedef struct tagGridTemp {
	int   m_nTemp;
	int   m_nType;        // 1, 太低了，低于35℃；2. 太高了，高于42℃
	int   m_nDTemp;       // 降温温度
	int   m_nDType;       // 1, 太低了，低于35℃；2. 太高了，高于42℃
}GridTemp;

typedef struct tagGridPulse {
	int   m_nPulse;
	BOOL  m_bConfilct;    // 是否和体温冲突（在图纸上重合）
}GridPulse;



CGlobalData  g_data;
CSkin        g_skin;

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
	else if (0 == strcmp(pstrClass, "ImageLabel")) {
		return new CImageLabelUI;
	}
	else if (0 == strcmp(pstrClass, "MyHandImage")) {
		return new CMyHandImage;
	}
	else if (0 == strcmp(pstrClass, "MyDateTime")) {
		return new CMyDateUI;
	}
	else if (0 == strcmp(pstrClass, "Temp")) {
		return new CTempUI;
	}
	else if (0 == strcmp("WaitingBar", pstrClass)) {
		return new CWaitingBarUI;
	}
	else if (0 == strcmp("NewTag", pstrClass)) {
		return new CNewTagUI;
	}
	else if (0 == strcmp("MaxiumCube", pstrClass)) {
		pUI = builder.Create("MaxiumCubeUI.xml", (UINT)0, this, m_pManager);
		return pUI;  
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

char * Time2String_hm(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d:%02d", tmp.tm_hour, tmp.tm_min);
	return szDest;
}

char * Date2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d-%02d-%02d", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday);
	return szDest;
}

char * Date2String_md(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d-%02d", tmp.tm_mon + 1, tmp.tm_mday);
	return szDest;
}

char * DateTime2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d-%02d-%02d %02d:%02d:%02d", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday,
		tmp.tm_hour, tmp.tm_min, tmp.tm_sec );
	return szDest;
}

char * DateTime2StringCn(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%04d年%02d月%02d日%02d时%02d分%02d秒", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday, tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
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

// 任意时间的当天零点时间
time_t  GetAnyDayZeroTime(time_t t) {
	struct tm tTmTime;
	localtime_s(&tTmTime, &t);

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

void  SavePatientName(DWORD i) {
	CDuiString  strText;
	strText.Format("%s %lu", CFG_PATIENT_NAME, i + 1);
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_szPatientName, 0);
	g_data.m_cfg->Save();
}

void  SaveAlarmTemp(DWORD i, DWORD j, int m) {
	CDuiString strText;
	CModeButton::Mode  mode = (CModeButton::Mode)m;
	DWORD dwValue = 0;

	if ( mode == CModeButton::Mode_Hand ) {
		strText.Format("%s %lu", CFG_HAND_READER_LOW_TEMP_ALARM, i + 1);
		dwValue = DEFAULT_LOW_TEMP_ALARM;
		g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm, &dwValue);

		strText.Format("%s %lu", CFG_HAND_READER_HIGH_TEMP_ALARM, i + 1);
		dwValue = DEFAULT_HIGH_TEMP_ALARM;
		g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm, &dwValue);
	}
	else {
		strText.Format("%s %lu %lu", CFG_LOW_TEMP_ALARM, i + 1, j + 1);
		dwValue = DEFAULT_LOW_TEMP_ALARM;
		g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm, &dwValue);

		strText.Format("%s %lu %lu", CFG_HIGH_TEMP_ALARM, i + 1, j + 1);
		dwValue = DEFAULT_HIGH_TEMP_ALARM;
		g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm, &dwValue);
	}

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

int  GetWeekDay(time_t t) {
	struct tm tTmTime;
	localtime_s(&tTmTime, &t);
	return tTmTime.tm_wday;
}

const char * GetWeekDayName(int nWeekIndex) {
	switch (nWeekIndex) {
	case 0:
		return "星期日";
	case 1:
		return "星期一";
	case 2:
		return "星期二";
	case 3:
		return "星期三";
	case 4:
		return "星期四";
	case 5:
		return "星期五";
	case 6:
		return "星期六";
	}
	return "";
}

const char * GetWeekDayShortName(int nWeekIndex) {
	switch (nWeekIndex) {
	case 0:
		return "周日";
	case 1:
		return "周一";
	case 2:
		return "周二";
	case 3:
		return "周三";
	case 4:
		return "周四";
	case 5:
		return "周五";
	case 6:
		return "周六";
	}
	return "";
}

void  OnEdtRemarkKillFocus( ) {
	DuiLib::CDuiString  strRemark = g_data.m_edRemark->GetText();
	bool bVisible = g_data.m_edRemark->IsVisible();

	// 如果是由于g_edRemark->SetVisible(false);导致的又一次堆栈调用，则跳过
	if (!bVisible) {
		return;
	}

	g_data.m_edRemark->SetText("");
	g_data.m_edRemark->SetVisible(false);

	CMyImageUI * pImage = (CMyImageUI *)g_data.m_edRemark->GetTag();
	assert(pImage);
	pImage->SetRemark(strRemark);
}

void  OnEdtHandRemarkKillFocus() {
	DuiLib::CDuiString  strRemark = g_data.m_edHandRemark->GetText();
	bool bVisible = g_data.m_edHandRemark->IsVisible();

	if (!bVisible) {
		return;
	}

	g_data.m_edHandRemark->SetText("");
	g_data.m_edHandRemark->SetVisible(false);

	CMyHandImage * pImage = (CMyHandImage *)g_data.m_edHandRemark->GetTag();
	assert(pImage);
	pImage->SetRemark(strRemark);
}

void CGraphicsRoundRectPath::AddRoundRect(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY)
{
	INT elWid = 2 * cornerX;
	INT elHei = 2 * cornerY;

	AddArc(x, y, elWid, elHei, 180, 90); // 左上角圆弧
	AddLine(x + cornerX, y, x + width - cornerX, y); // 上边

	AddArc(x + width - elWid, y, elWid, elHei, 270, 90); // 右上角圆弧
	AddLine(x + width, y + cornerY, x + width, y + height - cornerY);// 右边

	AddArc(x + width - elWid, y + height - elHei, elWid, elHei, 0, 90); // 右下角圆弧
																		//AddLine(x + width - cornerX, y + height, x + cornerX, y + height); // 下边

	int x1 = x + width - cornerX;
	int y1 = y + height;
	int x2 = x + width / 2 + 10;
	int y2 = y1;
	AddLine(x1, y1, x2, y2);
	x1 = x2;
	y1 = y2;
	x2 = x + width / 2;
	y2 = y1 + 10;
	AddLine(x1, y1, x2, y2);
	x1 = x2;
	y1 = y2;
	x2 = x + width / 2 - 10;
	y2 = y + height;
	AddLine(x1, y1, x2, y2);


	AddArc(x, y + height - elHei, elWid, elHei, 90, 90);
	AddLine(x, y + cornerY, x, y + height - cornerY);
}

char *  GetHandReaderId( char * szReaderId, DWORD dwReaderIdLen, const BYTE * pData ) {
	assert( szReaderId && dwReaderIdLen > 6 );
	DWORD  dwSn = (pData[0] << 24) | (pData[1] << 16) | (pData[2] << 8) | pData[3];
	SNPRINTF(szReaderId, dwReaderIdLen, "%06lu", dwSn);
	return szReaderId;
}

time_t  GetTimeDiff(time_t t1, time_t t2) {
	if (t1 > t2)
		return t1 - t2;
	else
		return t2 - t1;
}

const char * GetSexStr(int nSex) {
	if (nSex == 1)
		return "男";
	else if (nSex == 2)
		return "女";
	else
		return "";
}

char * PreviewNum(char * buf, DWORD dwSize, int nNum) {
	assert(dwSize > 0);
	assert(buf);

	if (0 >= nNum)
		buf[0] = '\0';
	else
		SNPRINTF(buf, dwSize, "%d", nNum);

	return buf;
}

void GetDateStr(char * year, DWORD d1, char * month, DWORD d2, char * day, DWORD d3, time_t t) {
	struct tm  tmp;
	localtime_s(&tmp, &t);

	SNPRINTF(year,  d1, "%04d", tmp.tm_year + 1900);
	SNPRINTF(month, d2, "%02d", tmp.tm_mon + 1);
	SNPRINTF(day,   d3, "%02d", tmp.tm_mday);
}

void PrepareXmlChart( CXml2ChartFile & xmlChart, PatientInfo * pInfo,
	                  PatientData * pData, DWORD dwSize, time_t tFirstDay, 
	                  const std::vector<PatientEvent * > & vEvents ) {
	char buf[256];
	char year[16];
	char month[16];
	char day[16];
	CXml2ChartUI * pItem = 0;
	CDuiString strText;
	int nIndex = 0;

	// 门诊号
	pItem = xmlChart.FindChartUIByName("outpatient_no");
	if (pItem) {
		pItem->SetText(pInfo->m_szOutpatientNo);
	}

	// 住院号
	pItem = xmlChart.FindChartUIByName("hospical_admission_no");
	if (pItem) {
		pItem->SetText(pInfo->m_szHospitalAdmissionNo);
	}

	// 姓名
	pItem = xmlChart.FindChartUIByName("patient_name");
	if (pItem) {
		pItem->SetText(pInfo->m_szPName);
	}

	// 性别
	pItem = xmlChart.FindChartUIByName("sex");
	if (pItem) {
		pItem->SetText(GetSexStr(pInfo->m_sex));
	}

	// 年龄
	pItem = xmlChart.FindChartUIByName("age");
	if (pItem) {
		pItem->SetText(pInfo->m_age);
	}

	// 入院日期
	if (pInfo->m_in_hospital > 0) {
		GetDateStr(year, sizeof(year), month, sizeof(month), day, sizeof(day),
			pInfo->m_in_hospital);
		// 年
		pItem = xmlChart.FindChartUIByName("in_date_y");
		if (pItem) {
			pItem->SetText(year);
		}
		// 月
		pItem = xmlChart.FindChartUIByName("in_date_m");
		if (pItem) {
			pItem->SetText(month);
		}
		// 日
		pItem = xmlChart.FindChartUIByName("in_date_d");
		if (pItem) {
			pItem->SetText(day);
		}
	}
	else {
		// 年
		pItem = xmlChart.FindChartUIByName("in_date_y");
		if (pItem) {
			pItem->SetText("");
		}
		// 月
		pItem = xmlChart.FindChartUIByName("in_date_m");
		if (pItem) {
			pItem->SetText("");
		}
		// 日
		pItem = xmlChart.FindChartUIByName("in_date_d");
		if (pItem) {
			pItem->SetText("");
		}
	}

	// 科别
	pItem = xmlChart.FindChartUIByName("medical_department");
	if (pItem) {
		pItem->SetText(pInfo->m_szMedicalDepartment);
	}

	if (pInfo->m_szMedicalDepartment2[0] != '\0') {
		pItem = xmlChart.FindChartUIByName("medical_department2");
		if (pItem)
			pItem->SetVisible(TRUE);

		pItem = xmlChart.FindChartUIByName("medical_department_to");
		if ( pItem )
			pItem->SetText(pInfo->m_szMedicalDepartment2);
	}
	else {
		pItem = xmlChart.FindChartUIByName("medical_department2");
		if ( pItem )
			pItem->SetVisible(FALSE);
	}

	// 病室
	pItem = xmlChart.FindChartUIByName("ward");
	if (pItem) {
		pItem->SetText(pInfo->m_szWard);
	}

	if (pInfo->m_szWard2[0] != '\0') {
		pItem = xmlChart.FindChartUIByName("ward2");
		if (pItem)
			pItem->SetVisible(TRUE);

		pItem = xmlChart.FindChartUIByName("ward_to");
		if (pItem)
			pItem->SetText(pInfo->m_szWard2);
	}
	else {
		pItem = xmlChart.FindChartUIByName("ward2");
		if (pItem)
			pItem->SetVisible(FALSE);
	}

	// 床号
	pItem = xmlChart.FindChartUIByName("bed_no");
	if (pItem) {
		pItem->SetText(pInfo->m_szBedNo);
	}

	if (pInfo->m_szBedNo2[0] != '\0') {
		pItem = xmlChart.FindChartUIByName("bed2");
		if (pItem)
			pItem->SetVisible(TRUE);

		pItem = xmlChart.FindChartUIByName("bed_no_to");
		if (pItem)
			pItem->SetText(pInfo->m_szBedNo2);
	}
	else {
		pItem = xmlChart.FindChartUIByName("bed2");
		if (pItem)
			pItem->SetVisible(FALSE);
	}

	//// 空数据不要打印
	//int nStartIndex = GetPatientDataStartIndex(pData, dwSize);
	//tFirstDay = tFirstDay + 3600 * 24 * nStartIndex;

	// 空数据也要打印
	int nYear = 0;
	int nMonth = 0;
	int nDay = 0;

	// 日期 
	for (int i = 0; i < 7; i++) {
		strText.Format("date_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if ( pItem ) {
			time_t t = tFirstDay + 3600 * 24 * i;
			struct tm  tmp;
			localtime_s(&tmp, &t);

			// 第一天显示全
			if (0 == i) {
				nYear = tmp.tm_year + 1900;
				nMonth = tmp.tm_mon + 1;
				nDay = tmp.tm_mday;

				SNPRINTF(buf, sizeof(buf), "%04d-%02d-%02d", 
					nYear, nMonth, nDay);
			}
			else {
				nDay = tmp.tm_mday;
				// 如果和第一天的月份不同
				if (tmp.tm_mon + 1 != nMonth) {
					nMonth = tmp.tm_mon + 1;
					// 如果连年份都不同
					if (tmp.tm_year+1900 != nYear) {
						nYear = tmp.tm_year + 1900;
						SNPRINTF(buf, sizeof(buf), 
							"%04d-%02d-%02d",
							nYear, nMonth, nDay );
					}
					// 年份相同
					else {
						SNPRINTF(buf, sizeof(buf),
							"%02d-%02d", nMonth, nDay);
					}
				}
				else {
					SNPRINTF(buf, sizeof(buf), "%02d", nDay);
				}
			}
			pItem->SetText(buf);
		}
	}

	// 住院日数
	if ( pInfo->m_in_hospital > 0 ) {
		time_t t1 = GetAnyDayZeroTime(pInfo->m_in_hospital);
		for  (int i = 0; i < 7; i++ ) {
			time_t t2 = GetAnyDayZeroTime(tFirstDay + 3600 * 24 * i);
			if ( t2 >= t1) {
				int nInDays = (int)(t2 - t1) / (3600 * 24);
				strText.Format("in_date_%d", i + 1);
				pItem = xmlChart.FindChartUIByName(strText);
				if (pItem) {
					strText.Format("%d", nInDays+1);
					pItem->SetText((const char *)strText);
				}
			}
			else {
				strText.Format("in_date_%d", i + 1);
				pItem = xmlChart.FindChartUIByName(strText);
				if (pItem) {
					pItem->SetText(""); 
				}
			}
		}
	}
	else {
		for (int i = 0; i < 7; i++) {
			strText.Format("in_date_%d", i + 1);
			pItem = xmlChart.FindChartUIByName(strText);
			if (pItem) {
				pItem->SetText("");
			}
		}
	}

	// 手术后日数
	// 术日  术2  分娩
	for (int i = 0; i < 7; i++) {
		time_t t1 = GetAnyDayZeroTime(tFirstDay + 3600 * 24 * i);

		std::vector<PatientEvent * >::const_iterator it;
		int nSurgeryIndex = 0;
		int nBirthIndex = 0;
		CDuiString strLine;

		for (it = vEvents.begin(); it != vEvents.end(); ++it) {
			PatientEvent * pEvent = *it;
			CDuiString strDayName;
			if ( pEvent->m_nType == PTYPE_SURGERY ) {
				nSurgeryIndex++;
				if (nSurgeryIndex == 1) {
					strDayName = "术日";
				}
				else {
					strDayName.Format("术%d", nSurgeryIndex);
				}
			}
			else if (pEvent->m_nType == PTYPE_BIRTH) {
				if ( nBirthIndex == 0 ) {
					nBirthIndex++;
					strDayName = "分娩";
				}
				else
					continue;
			}
			else {
				continue;
			}

			time_t t2 = GetAnyDayZeroTime(pEvent->m_time_1);
			if (t1 < t2)
				continue;

			if (t1 - t2 > 3600 * 24 * 14)
				continue;

			CDuiString strItem;
			if (t1 == t2) {
				strItem = strDayName;
			}
			else {
				strItem.Format("%d", (t1 - t2) / (3600 * 24) );
			}

			if (strLine.GetLength() == 0) {
				strLine = strItem;
			}
			else {
				strLine += "/";
				strLine += strItem;
			}
		}

		strText.Format("sur_date_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText((const char *)strLine);
		}
	}

	// 呼吸
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 6; j++) {
			strText.Format("breath_%d_%d", i + 1, j + 1);
			pItem = xmlChart.FindChartUIByName(strText);

			if (pData[i].m_breath[j][0] != '\0') {
				nIndex++;
				if (pItem) {
					pItem->SetText(pData[i].m_breath[j]);
					if (nIndex % 2 == 1) {
						pItem->SetVAlignType(VALIGN_TOP);
					}
					else {
						pItem->SetVAlignType(VALIGN_BOTTOM);
					}
				}
			}
			else {
				if (pItem) {
					pItem->SetText("");
				}
			}
		}
	}

	// 大便次数
	for (int i = 0; i < 7; i++) {
		strText.Format("defecate_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i].m_defecate);
		}
	}

	// 尿量
	for (int i = 0; i < 7; i++) {
		strText.Format("urine_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i].m_urine);
		}
	}

	// 入量
	for (int i = 0; i < 7; i++) {
		strText.Format("income_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i].m_income);
		}
	}

	// 出量
	for (int i = 0; i < 7; i++) {
		strText.Format("output_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i].m_output);
		}
	}

	// 血压
	for (int i = 0; i < 7; i++) {
		strText.Format("blood_pressure_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i].m_szBloodPressure);
		}
	}

	// 体重
	for (int i = 0; i < 7; i++) {
		strText.Format("weight_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i].m_szWeight);
		}
	}

	// 过敏
	for (int i = 0; i < 7; i++) {
		strText.Format("irritability_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i].m_szIrritability);
		}
	}

	pItem = xmlChart.FindChartUIByName("footer");
	if (pItem) {
		pItem->SetText(g_data.m_TempChartFooter);
	}

	// 页码(根据入院时间计算)
	if (pInfo->m_in_hospital > 0) {
		time_t tLastDay = GetAnyDayZeroTime(tFirstDay + 3600 * 24 * 7);
		time_t t = GetAnyDayZeroTime(pInfo->m_in_hospital);
		if ( tLastDay > t ) {
			int nPageIndex = (int)(tLastDay - t) / (3600 * 24 * 7);
			if (nPageIndex == 0)
				nPageIndex = 1;
			pItem = xmlChart.FindChartUIByName("page");
			if (pItem) {
				strText.Format("%d", nPageIndex);
				pItem->SetText((const char *)strText);
			}
		}
		else {
			pItem = xmlChart.FindChartUIByName("page");
			if (pItem) {
				pItem->SetText("");
			}
		}
	}
	else {
		pItem = xmlChart.FindChartUIByName("page");
		if (pItem) {
			pItem->SetText("");
		}
	}
}

static void DrawTempPointImg(POINT pt, int radius, HDC hDC, HPEN hPen) {
	HPEN  hOld = (HPEN)SelectObject(hDC, hPen);

	radius = (int)round(radius * 0.707f);
	
	POINT temp_points[2];
	temp_points[0].x = pt.x - radius;
	temp_points[0].y = pt.y - radius;
	temp_points[1].x = pt.x + radius;
	temp_points[1].y = pt.y + radius;
	::Polyline(hDC, temp_points, 2);

	temp_points[0].x = pt.x - radius;
	temp_points[0].y = pt.y + radius;
	temp_points[1].x = pt.x + radius;
	temp_points[1].y = pt.y - radius;
	::Polyline(hDC, temp_points, 2);

	SelectObject(hDC, hOld);
}

static void DrawDesTempPointImg(POINT pt, int radius, HDC hDC, HPEN hPen) {
	HPEN  hOld = (HPEN)SelectObject(hDC, hPen);
	HBRUSH  hOldBrush = (HBRUSH)SelectObject(hDC, GetStockBrush(NULL_BRUSH));
	::Ellipse(hDC, pt.x - radius, pt.y - radius, pt.x + radius, pt.y + radius);
	SelectObject(hDC, hOld);
	SelectObject(hDC, hOldBrush);
}

static void DrawTempPointLowHighArrow( BOOL bLow, int x, int y, int nArrowH, int nArrowR, 
	                                   HDC hDC, HPEN hPen) {
	HPEN  hOld = (HPEN)SelectObject(hDC, hPen);
	if (bLow) {
		::MoveToEx(hDC, x, y+6, 0);
		::LineTo(hDC, x, y + nArrowH);
		::LineTo(hDC, x - nArrowR, y + nArrowH - nArrowR);
		::MoveToEx(hDC, x, y + nArrowH, 0);
		::LineTo(hDC, x + nArrowR, y + nArrowH - nArrowR);
	}
	else {
		::MoveToEx(hDC, x, y - 6, 0);
		::LineTo(hDC, x, y - nArrowH);
		::LineTo(hDC, x - nArrowR, y - nArrowH + nArrowR);
		::MoveToEx(hDC, x, y - nArrowH, 0);
		::LineTo(hDC, x + nArrowR, y - nArrowH + nArrowR);
	}
	SelectObject(hDC, hOld);
}

static void DrawLineCb_(void * pArg, POINT start, POINT end) {
	void ** args = (void **)pArg;
	HDC    hDC = (HDC)args[0];
	HPEN   hPen = (HPEN)args[1];

	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	::MoveToEx(hDC, start.x, start.y, 0);
	::LineTo(hDC, end.x, end.y);
	SelectObject(hDC, hOldPen);
}

// 画温度图
static void  DrawTempImg( int width, int height, int nUnitsX, int nUnitsY, int nMaxY, 
	                     int nLeft, int nTop, int nOffsetX, int nOffsetY, int nStartIndex, 
	                     const std::vector<GridTemp *> & vTemp, int radius, HDC hDC, HPEN hTempPointPen,
	                     HPEN hDashReadPen, HPEN hRedPen, HPEN  hBluePen) {
	if ( vTemp.size() == 0 )
		return;

	float x = (float)width / (float)nUnitsX;
	float y = (float)height / (float)nUnitsY;
	HPEN hOld = 0;

	// 有效个数
	POINT points[6 * 7];
	int cnt = 0;

	std::vector<GridTemp *>::const_iterator it;
	int index = nStartIndex;
	for (it = vTemp.begin(); it != vTemp.end(); ++it, index++) {
		GridTemp * pItem = *it;
		// 没有体温数据(空的)
		if (pItem->m_nTemp <= 0) {
			continue;
		}
		
		// 有体温数据
		POINT  tmp_point;
		tmp_point.x = (int)((index + 0.5f) * x) + nLeft + nOffsetX;
		tmp_point.y = (int)((nMaxY - pItem->m_nTemp) * y) + nTop + nOffsetY;
		DrawTempPointImg(tmp_point, radius, hDC, hTempPointPen);
		if (pItem->m_nType != 0) {
			DrawTempPointLowHighArrow(pItem->m_nType == 1, tmp_point.x, tmp_point.y, 
				(int)(40.0f * y), 6, hDC, hBluePen);
		}

		// 处理连线
		points[cnt].x = tmp_point.x;
		points[cnt].y = tmp_point.y;

		// 如果还有降温数据
		if ( pItem->m_nDTemp > 0 ) {
			POINT  des_temp_point;
			des_temp_point.x = tmp_point.x;
			des_temp_point.y = (int)((nMaxY - pItem->m_nDTemp) * y) + nTop + nOffsetY;

			int nVDistance = 0;
			// 如果降温比常温高
			if (pItem->m_nDTemp >= pItem->m_nTemp) {
				points[cnt].y = des_temp_point.y;
			}
			nVDistance = tmp_point.y - des_temp_point.y;

			// 如果distance大于radius, 画虚线
			if (nVDistance > radius || nVDistance < -radius) {
				//hOld = (HPEN)SelectObject(hDC, hDashReadPen);
				//::MoveToEx(hDC, tmp_point.x, tmp_point.y, 0);
				//::LineTo(hDC, des_temp_point.x, nVDistance > radius ? des_temp_point.y + radius : des_temp_point.y - radius);
				//SelectObject(hDC, hOld);

				void * args[2] = { 0 };
				args[0] = hDC;
				args[1] = hRedPen;

				POINT end;
				end.x = des_temp_point.x;
				end.y = (nVDistance > radius ? des_temp_point.y + radius : des_temp_point.y - radius);
				DashLineTo(tmp_point, end, 8, 3, 1, (void *)args, DrawLineCb_);
			}

			// 画降温点
			DrawDesTempPointImg(des_temp_point, radius, hDC, hRedPen);
		}
		cnt++;

	}

	if (cnt > 0) {
		hOld = (HPEN)SelectObject(hDC, hBluePen);
		::Polyline(hDC, points, cnt);
		SelectObject(hDC, hOld);
	}

}


static void DrawPulsePointImg( POINT pt, int radius, BOOL bConflict, HDC hDC, 
	                           HPEN hPen, HBRUSH hBrush ) {
	// 如果和体温点冲突，画个圆
	if (bConflict) {
		HPEN hOld = (HPEN)SelectObject(hDC, hPen);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockBrush(NULL_BRUSH) );
		::Ellipse(hDC, pt.x - radius, pt.y - radius, pt.x + radius, pt.y + radius);
		SelectObject(hDC, hOld);
		SelectObject(hDC, hOldBrush);
	}
	// 不冲突，画实心红圆点
	else {
		HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
		HBRUSH hOld = (HBRUSH)SelectObject(hDC, hBrush);
		::Ellipse(hDC, pt.x - radius, pt.y - radius, pt.x + radius, pt.y + radius);
		SelectObject(hDC, hOld);
		SelectObject(hDC, hOldPen);
	}
}

// 画脉搏图
static void  DrawPulseImg(int width, int height, int nUnitsX, int nUnitsY, int nMaxY,
	                      int nLeft, int nTop, int nOffsetX, int nOffsetY, int nStartIndex,
	                      const std::vector<GridPulse *> & vPulse, int radius, 
	                      HDC hDC, HPEN hRedPen, HBRUSH hRedBrush) {
	if (vPulse.size() == 0)
		return;

	float x = (float)width / (float)nUnitsX;
	float y = (float)height / (float)nUnitsY;
	HPEN hOld = 0;

	// 有效个数
	POINT points[6 * 7];
	int cnt = 0;

	std::vector<GridPulse *>::const_iterator it;
	int index = nStartIndex;
	for (it = vPulse.begin(); it != vPulse.end(); ++it, index++) {
		GridPulse * pItem = *it;
		// 没有数据(空的)
		if (pItem->m_nPulse <= 0) {
			continue;
		}

		// 有数据
		POINT  pulse_point;
		pulse_point.x = (int)((index + 0.5f) * x) + nLeft + nOffsetX;
		pulse_point.y = (int)((nMaxY - pItem->m_nPulse) * y) + nTop + nOffsetY;
		DrawPulsePointImg(pulse_point, radius, pItem->m_bConfilct, hDC, hRedPen, hRedBrush);

		// 处理连线
		points[cnt].x = pulse_point.x;
		points[cnt].y = pulse_point.y;
		cnt++;
	}

	if (cnt > 1) {
		hOld = (HPEN)SelectObject(hDC, hRedPen);
		::Polyline(hDC, points, cnt);
		SelectObject(hDC, hOld);
	}
}

void PrintXmlChart( HDC hDC, CXml2ChartFile & xmlChart, int nOffsetX, int nOffsetY, 
	                PatientData * pData, DWORD dwDataSize, time_t tFirstDay,
	                const std::vector<PatientEvent * > & vEvents, 
	                const PatientInfo & patient_info) {
	SetBkMode(hDC, TRANSPARENT);
	DrawXml2ChartUI(hDC, xmlChart.m_ChartUI, nOffsetX, nOffsetY);

	CXml2ChartUI * pMain = xmlChart.FindChartUIByName("main");
	if (0 == pMain)
		return;

	RECT rect   = pMain->GetAbsoluteRect();
	int width   = rect.right  - rect.left;
	int height  = rect.bottom - rect.top;
	int radius  = 4;
	int pulse_radius = 5;

	HPEN hBluePen = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
	HPEN hRedPen = ::CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
	HPEN hDashReadPen = ::CreatePen(PS_DASH, 1, RGB(0xFF, 0, 0));
	HPEN hTempPointPen = ::CreatePen(PS_SOLID, 2, RGB(0, 0, 0xFF));
	HBRUSH hRedBrush = ::CreateSolidBrush(RGB(0xFF, 0, 0));
	// 创建字体
	LOGFONT  logfont;
	GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &logfont);
	logfont.lfWeight  = FW_NORMAL;
	logfont.lfItalic  = false;
	logfont.lfCharSet = DEFAULT_CHARSET;
	STRNCPY(logfont.lfFaceName, "宋体", sizeof(logfont.lfFaceName));
	logfont.lfHeight = 11;
	HFONT hfont = CreateFontIndirect(&logfont);
	// END 创建字体

	tFirstDay = GetAnyDayZeroTime(tFirstDay) + 3600 * 2;
	time_t tLastDay = tFirstDay + 3600 * 24 * 7;

	
	GridEvent    events_type[6 * 7];
	memset(events_type, 0, sizeof(events_type));

	GridEvent    events_type2[6 * 7][2];
	memset(events_type2, 0, sizeof(events_type2));


	// 获得每个小个子属于的事件
	std::vector<PatientEvent * >::const_iterator it;
	for (it = vEvents.begin(); it != vEvents.end(); ++it) {
		PatientEvent * pEvent = *it;
		if (pEvent->m_nType == PTYPE_HOLIDAY) {
			// 如果超出一周范围
			if (pEvent->m_time_1 >= tLastDay) {
				continue;
			}
			// 如果超出一周范围
			else if ( pEvent->m_time_2 < tFirstDay ) {
				continue;
			}

			if ( pEvent->m_time_1 < tFirstDay ) {
				// 全包含
				if (pEvent->m_time_2 >= tLastDay) {
					for (int i = 0; i < 42; i++) {
						SetGridEvent(events_type2, i, pEvent->m_nType, tFirstDay + 3600 * 4 * i);
					}
				}
				else {
					int index = (int)(pEvent->m_time_2 - tFirstDay) / (3600 * 4);
					assert(index >= 0 && index < 42);
					for (int i = 0; i <= index; i++) {
						SetGridEvent(events_type2, i, pEvent->m_nType, tFirstDay + 3600 * 4 * i);
					}
				}
			}
			else {
				int start_index = (int)(pEvent->m_time_1 - tFirstDay) / (3600 * 4);
				assert(start_index >= 0 && start_index < 42);
				int end_index = (int)(pEvent->m_time_2 - tFirstDay) / (3600 * 4);
				for (int i = start_index; (i < 42) && (i <= end_index); i++) {
					SetGridEvent(events_type2, i, pEvent->m_nType, tFirstDay + 3600 * 4 * i);
				}
			}
		}
		else {
			// 如果超出这一周的时间范围
			if (pEvent->m_time_1 < tFirstDay || pEvent->m_time_1 >= tLastDay ) {
				continue;
			}
			int index = (int)(pEvent->m_time_1 - tFirstDay) / (3600 * 4);
			assert(index >= 0 && index < 42);
			SetGridEvent( events_type2, index, pEvent->m_nType, pEvent->m_time_1 );
		}
	}

	// 加上入院和出院事件
	if ( patient_info.m_in_hospital > 0 ) {
		if (patient_info.m_in_hospital >= tFirstDay && patient_info.m_in_hospital < tLastDay) {
			int index = (int)(patient_info.m_in_hospital - tFirstDay) / (3600 * 4);
			SetGridEvent(events_type2, index, PTYPE_IN_HOSPITAL, patient_info.m_in_hospital);
		}
	}

	if (patient_info.m_out_hospital > 0) {
		if (patient_info.m_out_hospital >= tFirstDay && patient_info.m_out_hospital < tLastDay) {
			int index = (int)(patient_info.m_out_hospital - tFirstDay) / (3600 * 4);
			SetGridEvent(events_type2, index, PTYPE_OUT_HOSPITAL, patient_info.m_out_hospital);
		}
	}

	SortGridEvent(events_type2);

	ConvertGridEvent(events_type2, events_type);

	// END 加上入院和出院事件


	GridTemp  grid_temps[42];
	memset(grid_temps, 0, sizeof(grid_temps));

	const int  IMG_MIN_TEMP = 3500;
	const int  IMG_MAX_TEMP = 4200;

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 6; j++) {
			// 如果有温度
			if (pData[i].m_temp[j] > 0) {
				grid_temps[i * 6 + j].m_nTemp = pData[i].m_temp[j];
				// 如果常温低于35度
				if (grid_temps[i * 6 + j].m_nTemp < IMG_MIN_TEMP) {
					grid_temps[i * 6 + j].m_nTemp = IMG_MIN_TEMP;
					grid_temps[i * 6 + j].m_nType = 1;
				}
				// 如果常温大于42度
				else if (grid_temps[i * 6 + j].m_nTemp > IMG_MAX_TEMP) {
					grid_temps[i * 6 + j].m_nTemp = IMG_MAX_TEMP;
					grid_temps[i * 6 + j].m_nType = 2;
				}

				// 如果有降温
				if (pData[i].m_descend_temp[j] > 0) {
					grid_temps[i * 6 + j].m_nDTemp = pData[i].m_descend_temp[j];

					// 如果降温低于35度
					if (grid_temps[i * 6 + j].m_nDTemp < IMG_MIN_TEMP) {
						grid_temps[i * 6 + j].m_nDTemp = IMG_MIN_TEMP;
						grid_temps[i * 6 + j].m_nDType = 1;
					}
					// 如果降温大于42度
					else if (grid_temps[i * 6 + j].m_nDTemp > IMG_MAX_TEMP) {
						grid_temps[i * 6 + j].m_nDTemp = IMG_MAX_TEMP;
						grid_temps[i * 6 + j].m_nDTemp = 2;
					}
				}
			}			
		}
	}

	GridPulse  grid_pulses[42];
	memset(grid_pulses, 0, sizeof(grid_pulses));

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 6; j++) {
			// 如果有脉搏
			if (pData[i].m_pulse[j] > 0) {
				grid_pulses[i * 6 + j].m_nPulse = pData[i].m_pulse[j];

				// 如果脉搏低于20
				if (grid_pulses[i * 6 + j].m_nPulse < 20) {
					grid_pulses[i * 6 + j].m_nPulse = 20;
				}
				// 如果脉搏大于192
				else if (grid_pulses[i * 6 + j].m_nPulse > 192) {
					grid_pulses[i * 6 + j].m_nPulse = 192;
				}
				else {
					// 如果同一时间格子内有体温
					if (grid_temps[i * 6 + j].m_nTemp > 0) {
						float f1 = (float)(192 - grid_pulses[i * 6 + j].m_nPulse) / (float)(192 - 20);
						float f2 = (float)(4260 - grid_temps[i * 6 + j].m_nTemp) / (float)(4260 - 3400);
						float fDiff = ( f1 > f2 ? f1 - f2 : f2 - f1);
						// 如果两者垂直间相隔不到3像素
						if ( (int)(fDiff * height) <= 3) {
							grid_pulses[i * 6 + j].m_bConfilct = TRUE;
						}
					}
				}

			}
		}
	}

	// 分割曲线的位置
	std::vector<int >    vSplit;

	// 假期把温度和脉搏数据分为几段
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 6; j++) {
			int index = i * 6 + j;
			// 如果没有事件
			if (events_type[index].m_nType == 0) {
				continue;
			}

			// 如果不是假期事件
			if (events_type[index].m_nType != PTYPE_HOLIDAY) {
				continue;
			}

			vSplit.push_back(index);
		}
	}

	// 体温数据
	int nStartIndex = 0;
	std::vector<GridTemp *>  vTemp;
	std::vector<int >::iterator  ia;

	for (ia = vSplit.begin(); ia != vSplit.end(); ++ia) {
		int nSplit = *ia;
		vTemp.clear();
		for (int i = nStartIndex; i < nSplit; ++i) {
			vTemp.push_back(&grid_temps[i]);
		}
		DrawTempImg( width, height, 42, 4260 - 3400, 4260, rect.left, rect.top,
			         nOffsetX, nOffsetY, nStartIndex, vTemp, radius, hDC, hTempPointPen, 
			         hDashReadPen,hRedPen, hBluePen);
		nStartIndex = nSplit + 1;
	}

	vTemp.clear();
	for (int i = nStartIndex; i < 42; ++i) {
		vTemp.push_back(&grid_temps[i]);
	}
	DrawTempImg(width, height, 42, 4260 - 3400, 4260, rect.left, rect.top,
		nOffsetX, nOffsetY, nStartIndex, vTemp, radius, hDC, hTempPointPen,
		hDashReadPen, hRedPen, hBluePen);
	// END 体温数据


	// 脉搏数据
	nStartIndex = 0;
	std::vector<GridPulse *>  vPulse;

	for (ia = vSplit.begin(); ia != vSplit.end(); ++ia) {
		int nSplit = *ia;
		vPulse.clear();
		for (int i = nStartIndex; i < nSplit; ++i) {
			vPulse.push_back(&grid_pulses[i]);
		}
		DrawPulseImg(width, height, 42, 192 - 20, 192, rect.left, rect.top,
			nOffsetX, nOffsetY, nStartIndex, vPulse, pulse_radius, hDC, hRedPen, hRedBrush);
		nStartIndex = nSplit + 1;
	}

	vPulse.clear();
	for (int i = nStartIndex; i < 42; ++i) {
		vPulse.push_back(&grid_pulses[i]);
	}
	DrawPulseImg(width, height, 42, 192 - 20, 192, rect.left, rect.top,
		nOffsetX, nOffsetY, nStartIndex, vPulse, pulse_radius, hDC, hRedPen, hRedBrush);
	// END 脉搏数据


	// 写事件信息
	float fGridW = (float)width / (float)42;
	float fGridH = (float)height / (float)43;
	HFONT  hOldFont = (HFONT)SelectObject(hDC, hfont);
	SetTextColor(hDC, RGB(255, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(hDC, hRedPen);

	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 6; j++) {
			int index = i * 6 + j;
			// 如果没有事件
			if (events_type[index].m_nType == 0) {
				continue;
			}

			CDuiString  strType;
			BOOL        bNeedTime = TRUE;

			switch (events_type[index].m_nType)
			{
			case PTYPE_HOLIDAY:
				strType = "请假";
				bNeedTime = FALSE;
				break;
			case PTYPE_SURGERY:
				strType = "手术";
				bNeedTime = FALSE;
				break;
			case PTYPE_BIRTH:
				strType = "分娩";
				break;
			case PTYPE_TURN_IN:
				strType = "转入";
				break;
			case PTYPE_TURN_OUT:
				strType = "转出";
				break;
			case PTYPE_DEATH:
				strType = "呼吸心跳停止";
				break;
			case PTYPE_IN_HOSPITAL:
				strType = "入院";
				break;
			case PTYPE_OUT_HOSPITAL:
				strType = "出院";
				break;
			default:
				break;
			}

			float  ox = index * fGridW + rect.left + nOffsetX;
			float  oy = 3 * fGridH + rect.top + nOffsetY;

			for (int k = 0; k < strType.GetLength(); k+=2) {
				int n = k / 2;
				CDuiString s = strType.Mid(k, 2);
				RECT  gridRect = { (int)ox, (int)(oy + n * fGridH), (int)(ox + fGridW),
					               (int)(oy + n * fGridH + fGridH) };
				// 以下两种方法，还是有时显得不够居中
				//int nGridWidth = gridRect.right - gridRect.left;
				//int nGridHeight = gridRect.bottom - gridRect.top;
				//RECT rcText;
				//::DrawText(hDC, s, s.GetLength(), &rcText, DT_CALCRECT);
				//int  nStrWidth = rcText.right - rcText.left;
				//int  nStrHeight = rcText.bottom - rcText.top;
				//::TextOut( hDC, gridRect.left + (nGridWidth - nStrWidth) / 2, 
				//	       gridRect.top + (nGridHeight - nStrHeight) / 2, s, s.GetLength());
				::DrawText(hDC, s, s.GetLength(), &gridRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}

			// 如果需要时间
			if (bNeedTime) {
				// 画破折号
				int nTypeLen = strType.GetLength() / 2;
				float x = ox + 0.5f * fGridW;
				float y = oy + nTypeLen * fGridH;
				MoveToEx(hDC, (int)x, (int)y, 0);
				LineTo(hDC, (int)x, (int)(y + 2.0f * fGridH));

				char szTime[256];
				Time2String_hm_cn(szTime, sizeof(szTime), &events_type[index].m_tTime);
				CDuiString strTime = szTime;

				oy = y + 2.0f * fGridH;
				for (int k = 0; k < strTime.GetLength(); k += 2) {
					int n = k / 2;
					CDuiString s = strTime.Mid(k, 2);
					RECT  gridRect = { (int)ox, (int)(oy + n * fGridH), (int)(ox + fGridW),
						(int)(oy + n * fGridH + fGridH) };
					::DrawText(hDC, s, s.GetLength(), &gridRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				}
			}

		}
	}
	// END 写事件信息
	SelectObject(hDC, hOldFont);
	SelectObject(hDC, hOldPen);


	DeleteObject(hBluePen);
	DeleteObject(hRedPen);
	DeleteObject(hDashReadPen);
	DeleteObject(hTempPointPen);
	DeleteObject(hRedBrush);
	DeleteObject(hfont);
}

void LoadXmlChart(CXml2ChartFile & xmlChart) {

	HRSRC hResource = ::FindResource(0, MAKEINTRESOURCE(IDR_XML1), "XML");
	if (hResource != NULL) {
		// 加载资源
		HGLOBAL hg = LoadResource(0, hResource);
		if (hg != NULL) {
			// 锁定资源
			LPVOID pData = LockResource(hg);
			if (pData != NULL) {
				// 获取资源大小
				DWORD dwSize = SizeofResource(0, hResource);
				xmlChart.ReadXmlChartStr((const char *)pData, dwSize);
			}
		}
	}
}

BOOL IsToday(const SYSTEMTIME & s) {
	time_t now = time(0);

	struct tm  tmp;
	localtime_s(&tmp, &now);

	if (tmp.tm_year + 1900 == s.wYear && tmp.tm_mon + 1 == s.wMonth && tmp.tm_mday == s.wDay) {
		return TRUE;
	}
	
	return FALSE;
}

char * Date2String(char * szDest, DWORD dwDestSize, const SYSTEMTIME & s) {
	_snprintf_s(szDest, dwDestSize, dwDestSize, "%04d-%02d-%02d", (int)s.wYear, (int)s.wMonth, (int)s.wDay);
	return szDest;
}

time_t SysTime2Time(const SYSTEMTIME & s) {
	struct tm tTmTime;

	tTmTime.tm_year = (int)s.wYear - 1900;
	tTmTime.tm_mon  = (int)s.wMonth - 1;
	tTmTime.tm_mday = (int)s.wDay;
	tTmTime.tm_hour = (int)s.wHour;
	tTmTime.tm_min  = (int)s.wMinute;
	tTmTime.tm_sec  = (int)s.wSecond;

	return mktime(&tTmTime);
}

SYSTEMTIME Time2SysTime(const time_t & t) {
	struct tm tTmTime;
	localtime_s(&tTmTime, &t);

	SYSTEMTIME s;
	memset(&s, 0, sizeof(SYSTEMTIME));

	s.wYear  = (WORD)(tTmTime.tm_year + 1900);
	s.wMonth = (WORD)(tTmTime.tm_mon + 1);
	s.wDay   = (WORD)tTmTime.tm_mday;
	s.wHour  = (WORD)tTmTime.tm_hour;
	s.wMinute = (WORD)tTmTime.tm_min;
	s.wSecond = (WORD)tTmTime.tm_sec;
	s.wDayOfWeek = (WORD)tTmTime.tm_wday;

	return s;
}

DWORD GetIntFromDb(const char * szValue, int nDefault /*= 0*/) {
	DWORD dwValue = nDefault;
	if ( szValue )
		sscanf_s( szValue, "%lu", &dwValue);
	return dwValue;
}

char * GetStrFromdDb(char * buf, DWORD dwSize, const char * szValue) {
	assert(dwSize > 0);
	if (szValue) {
		Utf8ToAnsi(buf, dwSize, szValue);
	}		
	else if (dwSize > 0) {
		buf[0] = '\0';
	}		
	return buf;
}

static  const char * GetCnDigital(int n) {
	assert(n >= 0 && n < 10);
	const char * arrDigital[10] = { "零","一","二","三", "四","五","六","七","八","九" };
	return arrDigital[n];
}

static  const char * GetCnDigital_1(int n) {
	assert(n >= 1 && n < 10);
	const char * arrDigital[9] = { "十","二十","三十", "四十","五十","六十","七十","八十","九十" };
	return arrDigital[n-1];
}

extern char * Time2String_hm_cn(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	CDuiString  strText;
	if (tmp.tm_hour < 10) {
		strText += GetCnDigital(tmp.tm_hour);
		strText += "时";
	}
	else {
		int a = tmp.tm_hour / 10;
		int b = tmp.tm_hour % 10;
		strText += GetCnDigital_1(a);
		if (b > 0) {
			strText += GetCnDigital(b);
		}
		strText += "时";
	}

	if (tmp.tm_min < 10) {
		strText += GetCnDigital(tmp.tm_min);
		strText += "分";
	}
	else {
		int a = tmp.tm_min / 10;
		int b = tmp.tm_min % 10;
		strText += GetCnDigital_1(a);
		if (b > 0) {
			strText += GetCnDigital(b);
		}
		strText += "分";
	}
	
	STRNCPY(szDest, strText, dwDestSize);
	return szDest;
}

static int  GetGridEventPriority(int nType) {
	switch (nType)
	{
	case PTYPE_SURGERY:
		return 20;
	case PTYPE_BIRTH:
		return 2;
	case PTYPE_TURN_IN:
		return 2;
	case PTYPE_TURN_OUT:
		return 2;
	case PTYPE_DEATH:
		return 40;
	case PTYPE_HOLIDAY:
		return 1;
	case PTYPE_IN_HOSPITAL:
		return 10;
	case PTYPE_OUT_HOSPITAL:
		return 10;
	default:
		return 0;
	}
	return 0;
}

void  SetGridEvent( GridEvent events_type2[6 * 7][2], int nIndex, int nType, time_t t ) {
	assert(nIndex >= 0 && nIndex < 6 * 7);
	BOOL bFindEmpty = FALSE;
	for (int i = 0; i < 2; i++) {
		if (events_type2[nIndex][i].m_nType == 0) {
			events_type2[nIndex][i].m_nType = nType;
			events_type2[nIndex][i].m_tTime = t;
			bFindEmpty = TRUE;
			break;
		}
	}

	if (bFindEmpty)
		return;

	int nPrio = GetGridEventPriority(nType);
	int nMinPrio   = -1;
	int nFindIndex = -1;

	for (int i = 0; i < 2; i++) {
		int nItemPrio = GetGridEventPriority(events_type2[nIndex][i].m_nType);
		// 如果是开始
		if (nFindIndex == -1) {
			nFindIndex = i;
			nMinPrio = nItemPrio;
		}
		else {
			if ( nItemPrio < nMinPrio ) {
				nFindIndex = i;
				nMinPrio = nItemPrio;
			}
		}
	}
	
	assert(nFindIndex >= 0 && nMinPrio >= 0);

	// 如果优先级大，挤出去
	if ( nPrio > nMinPrio ) {
		events_type2[nIndex][nFindIndex].m_nType = nType;
		events_type2[nIndex][nFindIndex].m_tTime = t;
	}
}

void  SortGridEvent(GridEvent events_type2[6 * 7][2]) {
	for ( int i = 0; i < 42; i++ ) {
		if (events_type2[i][0].m_tTime == 0)
			continue;

		if (events_type2[i][1].m_tTime == 0)
			continue;

		if ( events_type2[i][0].m_tTime > events_type2[i][1].m_tTime ) {
			GridEvent tmp;
			memcpy(&tmp, &events_type2[i][0], sizeof(GridEvent));
			memcpy(&events_type2[i][0], &events_type2[i][1], sizeof(GridEvent));
			memcpy(&events_type2[i][1], &tmp, sizeof(GridEvent));
		}
	}
}

void  ConvertGridEvent(GridEvent events_type2[6 * 7][2], GridEvent events_type[6 * 7]) {
	for (int i = 0; i < 42; i++) {
		memcpy(&events_type[i], &events_type2[i][0], sizeof(GridEvent));
		// 如果当前时间段，没有第二事件
		if ( events_type2[i][1].m_nType <= 0 ) {
			continue;
		}

		// 如果当前时间段，还有事件
		// 没有格子了
		if (i == 42 - 1) {
			break;
		}

		// 如果右边的相邻格子没有事件
		if ( events_type2[i + 1][0].m_nType == 0 ) {
			memcpy(&events_type[i+1], &events_type2[i][1], sizeof(GridEvent));
			i++;
		}
		else {
			BOOL  bGreat = TRUE;
			int nPrio = GetGridEventPriority(events_type2[i][1].m_nType);

			for (int j = 0; j < 2; j++) {
				int nItemPrio = GetGridEventPriority(events_type2[i + 1][j].m_nType);
				if (nPrio <= nItemPrio) {
					bGreat = FALSE;
					break;
				}
			}

			if (bGreat) {
				memcpy(&events_type[i + 1], &events_type2[i][1], sizeof(GridEvent));
				i++;
			}
		}
	}
}

const char * GetEventTypeStr(int nType) {
	switch (nType)
	{
	case PTYPE_HOLIDAY:
		return "请假";
	case PTYPE_SURGERY:
		return "手术";
	case PTYPE_BIRTH:
		return "分娩";
	case PTYPE_TURN_IN:
		return "转入";
	case PTYPE_TURN_OUT:
		return "转出";
	case PTYPE_DEATH:
		return "呼吸心跳停止";
	case PTYPE_IN_HOSPITAL:
		return "入院";
	case PTYPE_OUT_HOSPITAL:
		return "出院";
	default:
		return "";
	}
	return "";
}

char *  VlcTime2Str(char * szTime, DWORD dwTimeSize, int nTime) {
	assert(szTime);
	int tns = nTime / 1000;
	int thh = tns / 3600;
	int tmm = (tns % 3600) / 60;
	int tss = (tns % 60);
	SNPRINTF(szTime, dwTimeSize, "%02d:%02d:%02d", thh, tmm, tss);
	return szTime;
}

void InitComPort(CLmnSerialPort * pPort) {
	BYTE buf[256] = { 0 };
	//LmnSleep(1000);
	memcpy(buf, "\x00\x01\x0A\x55\x01\x01\xDD\xAA", 8);
	buf[1] = (BYTE)g_data.m_nReaderNoStart;
	buf[2] = (BYTE)g_data.m_nReaderNoEnd;
	DWORD  dwWriteSize = 8;
	pPort->Write(buf, dwWriteSize);
}

CSkin::CSkin(DWORD dwTheme /* =0 */) {
	m_dwTheme = dwTheme;
}

void  CSkin::SetTheme(DWORD  dwTheme) {
	m_dwTheme = dwTheme;
}

DWORD   CSkin::operator[] (COLOR_ENUM  dwColorIndex) {
	// 缺省主题
	if ( m_dwTheme == 0 ) {
		switch (dwColorIndex)
		{
		case CSkin::MAIN_BACKGROUND:
			return 0xFF434248; 
			break;
		case CSkin::LABEL:
			return 0xFFFFFFFF;
			break;
		case CSkin::EDIT:
			return 0xFFFFFFFF;
			break;
		default:
			break;
		}
	}
	else {
		switch (dwColorIndex)
		{
		case CSkin::MAIN_BACKGROUND:
			return 0xFFFFFFFF;
			break;
		case CSkin::LABEL:
			return 0xFF000000;
			break;
		case CSkin::EDIT:
			return 0xFFF0F0F0;
			break;
		default:
			break;
		}
	}
	return 0;
}