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
	else if (0 == strcmp(pstrClass, "ImageLabel")) {
		return new CImageLabelUI;
	}
	else if (0 == strcmp(pstrClass, "MyHandImage")) {
		return new CMyHandImage;
	}
	else if (0 == strcmp(pstrClass, "MyDateTime")) {
		return new CMyDateUI;
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
	                  PatientData * pData, DWORD dwSize, time_t tFirstDay) {
	char buf[256];
	char year[16];
	char month[16];
	char day[16];
	CXml2ChartUI * pItem = 0;
	CDuiString strText;

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
		pItem->SetText(PreviewNum(buf, sizeof(buf), pInfo->m_age));
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

	// 病室
	pItem = xmlChart.FindChartUIByName("ward");
	if (pItem) {
		pItem->SetText(pInfo->m_szWard);
	}

	// 床号
	pItem = xmlChart.FindChartUIByName("bed_no");
	if (pItem) {
		pItem->SetText(pInfo->m_szBedNo);
	}

	// 空数据不要打印
	int nStartIndex = GetPatientDataStartIndex(pData, dwSize);
	tFirstDay = tFirstDay + 3600 * 24 * nStartIndex;

	// 日期 
	for (int i = 0; i < 7; i++) {
		strText.Format("date_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if ( pItem ) {
			time_t t = tFirstDay + 3600 * 24 * i;
			Date2String_md( buf, sizeof(buf), &t );
			pItem->SetText(buf);
		}
	}

	// 住院日数
	if ( pInfo->m_in_hospital > 0 ) {
		time_t t1 = GetAnyDayZeroTime(pInfo->m_in_hospital);
		for  (int i = 0; i < 7 - nStartIndex; i++ ) {
			time_t t2 = GetAnyDayZeroTime(tFirstDay + 3600 * 24 * i);
			if ( t2 >= t1) {
				int nInDays = (int)(t2 - t1) / (3600 * 24);
				strText.Format("in_date_%d", i + 1);
				pItem = xmlChart.FindChartUIByName(strText);
				if (pItem) {
					strText.Format("%d", nInDays);
					pItem->SetText((const char *)strText);
				}
			}
		}
	}

	// 手术后日数
	if (pInfo->m_surgery > 0) {
		time_t t1 = GetAnyDayZeroTime(pInfo->m_surgery);
		for (int i = 0; i < 7 - nStartIndex; i++) {
			time_t t2 = GetAnyDayZeroTime(tFirstDay + 3600 * 24 * i);
			if (t2 >= t1) {
				int nInDays = (int)(t2 - t1) / (3600 * 24); 
				strText.Format("sur_date_%d", i + 1);
				pItem = xmlChart.FindChartUIByName(strText);
				if (pItem) {
					strText.Format("%d", nInDays);
					pItem->SetText((const char *)strText);
				}
			}
		}
	}

	// 呼吸
	for (int i = 0; i < 7 - nStartIndex; i++) {
		for (int j = 0; j < 6; j++) {
			strText.Format("breath_%d_%d", i + 1, j + 1);
			pItem = xmlChart.FindChartUIByName(strText);
			if (pItem) {
				pItem->SetText(PreviewNum(buf, sizeof(buf), pData[i+ nStartIndex].m_breath[j] ));
			}
		}
	}

	// 大便次数
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("defecate_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(PreviewNum(buf, sizeof(buf), pData[i+ nStartIndex].m_defecate));
		}
	}

	// 尿量
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("urine_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(PreviewNum(buf, sizeof(buf), pData[i + nStartIndex].m_urine));
		}
	}

	// 入量
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("income_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(PreviewNum(buf, sizeof(buf), pData[i + nStartIndex].m_income));
		}
	}

	// 出量
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("output_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(PreviewNum(buf, sizeof(buf), pData[i + nStartIndex].m_output));
		}
	}

	// 血压
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("blood_pressure_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i + nStartIndex].m_szBloodPressure);
		}
	}

	// 体重
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("weight_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i + nStartIndex].m_szWeight);
		}
	}

	// 过敏
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("irritability_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i + nStartIndex].m_szIrritability);
		}
	}
}

int GetPatientDataStartIndex(PatientData * pData, DWORD dwSize) {
	assert(dwSize >= 7);
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 6; j++) {
			if (pData[i].m_pulse[j] > 0) {
				return i;
			}
			if (pData[i].m_breath[j] > 0) {
				return i;
			}
			if (pData[i].m_temp[j] > 0) {
				return i;
			}
		}

		if (pData[i].m_defecate > 0)
			return i;

		if (pData[i].m_urine > 0)
			return i;

		if (pData[i].m_income > 0)
			return i;

		if (pData[i].m_output > 0)
			return i;

		if (pData[i].m_szBloodPressure[0] != '\0')
			return i;

		if (pData[i].m_szWeight[0] != '\0')
			return i;

		if (pData[i].m_szIrritability[0] != '\0')
			return i;		
	}

	return 6;
}

static int  GetPatientDataImagePoints( POINT * points, DWORD  dwSize, int w, int h,
	                                   int nUnitsX, int nUnitsY, int nMaxY, 
									   int nLeft, int nTop, int nOffsetX, int nOffsetY,
	                                   PatientData * pData, DWORD dwDataSize, int nType = 0 ) {
	assert(dwDataSize >= 7);
	assert(dwSize >= 6 * 7);

	float x = (float)w / (float)nUnitsX;
	float y = (float)h / (float)nUnitsY;
	int cnt = 0;
	int nStartIndex = GetPatientDataStartIndex(pData, dwDataSize);

	for ( int i = 0; i < 7 - nStartIndex; i++ ) {
		// 温度
		if (nType == 0) {
			for (int j = 0; j < 6; j++) {
				if ( pData[i + nStartIndex].m_temp[j] >= 3400 && pData[i + nStartIndex].m_temp[j] <= 4200) {
					points[cnt].x = (int)((i * 6 + j + 0.5f) * x) + nLeft + nOffsetX;
					points[cnt].y = (int)((nMaxY - pData[i + nStartIndex].m_temp[j]) * y) + nTop + nOffsetY;
					cnt++;
				}
			}			
		}
		// 脉搏
		else {
			for (int j = 0; j < 6; j++) {
				if (pData[i + nStartIndex].m_pulse[j] >= 20 && pData[i + nStartIndex].m_pulse[j] <= 192 ) {
					points[cnt].x = (int)((i * 6 + j + 0.5f) * x) + nLeft + nOffsetX;
					points[cnt].y = (int)((nMaxY - pData[i + nStartIndex].m_pulse[j]) * y) + nTop + nOffsetY;
					cnt++;
				}
			}
		}
	}

	return cnt;
}

void PrintXmlChart( HDC hDC, CXml2ChartFile & xmlChart, int nOffsetX, int nOffsetY, 
	                PatientData * pData, DWORD dwDataSize ) {
	SetBkMode(hDC, TRANSPARENT);
	DrawXml2ChartUI(hDC, xmlChart.m_ChartUI, nOffsetX, nOffsetY);

	CXml2ChartUI * pMain = xmlChart.FindChartUIByName("main");
	if (0 == pMain)
		return;

	HPEN   hOld_pen = 0;
	HBRUSH hOld_brush = 0;
	HPEN pen   = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
	HPEN pen_1 = ::CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
	HPEN pen_2 = ::CreatePen(PS_SOLID, 2, RGB(0, 0, 0xFF));
	HBRUSH hBrush = ::CreateSolidBrush( RGB(0xFF, 0, 0));

	int radius = 3;

	RECT r = pMain->GetAbsoluteRect();
	int w = r.right - r.left;
	int h = r.bottom - r.top;

	POINT points[6 * 7];
	int cnt = 0;

	// 计算体温曲线
	cnt = GetPatientDataImagePoints( points, 6 * 7, w, h, 42, 860, 4260, r.left, r.top,
		                       nOffsetX, nOffsetY, pData, dwDataSize );	
	hOld_pen = (HPEN)::SelectObject(hDC, pen);
	::Polyline(hDC, points, cnt);
	
	// 画体温点
	::SelectObject(hDC, pen_2);
	for (int i = 0; i < cnt; i++) {
		POINT temp_points[2];		

		temp_points[0].x = points[i].x - radius;
		temp_points[0].y = points[i].y - radius;
		temp_points[1].x = points[i].x + radius;
		temp_points[1].y = points[i].y + radius;
		::Polyline(hDC, temp_points, 2);

		temp_points[0].x = points[i].x - radius;
		temp_points[0].y = points[i].y + radius;
		temp_points[1].x = points[i].x + radius;
		temp_points[1].y = points[i].y - radius;
		::Polyline(hDC, temp_points, 2);
	}	

	
	// 计算脉搏曲线
	cnt = GetPatientDataImagePoints( points, 6 * 7, w, h, 42, 172, 192, r.left, r.top,
		nOffsetX, nOffsetY, pData, dwDataSize, 1);
	::SelectObject(hDC, pen_1);
	::Polyline(hDC, points, cnt);

	// 画脉搏点
	hOld_brush = (HBRUSH)::SelectObject(hDC, hBrush);
	for (int i = 0; i < cnt; i++) {
		::Ellipse( hDC, points[i].x - radius, points[i].y - radius,
			       points[i].x + radius, points[i].y + radius );
	}

	::SelectObject(hDC, hOld_pen);
	::SelectObject(hDC, hOld_brush);

	DeleteObject(pen);
	DeleteObject(pen_1);
	DeleteObject(pen_2);
	DeleteObject(hBrush);
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