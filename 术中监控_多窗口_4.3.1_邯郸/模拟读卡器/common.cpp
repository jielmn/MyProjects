#include <time.h>
#include "common.h"
#include <assert.h>

CGlobalData  g_data;

char *  Data2String(char * buf, DWORD dwBufSize, const BYTE * pData, DWORD dwDataLen) {
	assert(buf);
	assert(pData);
	assert(dwBufSize > 0);
	assert(dwDataLen > 0);
	assert(dwBufSize >= dwDataLen * 3 + 1);

	char * p = buf;
	for ( DWORD i = 0; i < dwDataLen; i++ ) {
		SNPRINTF(p, 4, "%02X ", pData[i]);
		p += 3;
	}

	return buf;
}

char * DateTime2String(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d-%02d-%02d %02d:%02d:%02d", tmp.tm_year + 1900, tmp.tm_mon + 1, tmp.tm_mday,
		tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
	return szDest;
}