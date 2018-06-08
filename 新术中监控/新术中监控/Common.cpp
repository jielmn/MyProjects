#include <time.h>
#include "Common.h"

ILog    * g_log = 0;
IConfig * g_cfg = 0;
LmnToolkits::Thread *  g_thrd_db = 0;
LmnToolkits::Thread *  g_thrd_timer = 0;

char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d:%02d:%02d", tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	return szDest;
}