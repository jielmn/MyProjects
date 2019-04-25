#include <time.h>
#include <stdio.h>   
#include <windows.h>  
#include <setupapi.h>
#include "common.h"
#include "EditableButtonUI.h"
#include "MyImageUI.h"

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")


CGlobalData  g_data;
std::vector<TArea *>  g_vArea;
DWORD g_ReaderIndicator[MAX_READERS_PER_GRID] 
	= { 0xFF00FF00, 0xFF1b9375, 0xFF00FFFF, 0xFF51786C, 0xFFFFFF00, 0xFFCA5100 };

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