#include <time.h>
#include "common.h"

HWND    g_hWnd = 0;
ILog    * g_log = 0;
IConfig * g_cfg = 0;
LmnToolkits::Thread *  g_thrd_db = 0;
LmnToolkits::Thread *  g_thrd_vport[MAX_VPORT_COUNT];
int     g_nInit[MAX_VPORT_COUNT];

//char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t) {
//	struct tm  tmp;
//	localtime_s(&tmp, t);
//
//	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d:%02d:%02d", tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
//	return szDest;
//}