#include <time.h>
#include "common.h"

CGlobalData  g_data;

BOOL CALLBACK YourEnumProc(HWND hWnd, LPARAM lParam)
{
	DWORD dwProcessId;
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	LPWNDINFO pInfo = (LPWNDINFO)lParam;
	if (dwProcessId == pInfo->dwProcessId)
	{
		char szClassName[256];
		::GetClassName(hWnd, szClassName, sizeof(szClassName));

		if ( 0 == StrICmp(szClassName, "CEFCLIENT") ) {
			pInfo->hWnd = hWnd;
			return FALSE;
		}		
	}
	return TRUE;
}

HWND GetProcessMainWnd(DWORD dwProcessId)
{
	WNDINFO wi;
	wi.dwProcessId = dwProcessId;
	wi.hWnd = NULL;
	EnumWindows(YourEnumProc, (LPARAM)&wi);
	return wi.hWnd;
}