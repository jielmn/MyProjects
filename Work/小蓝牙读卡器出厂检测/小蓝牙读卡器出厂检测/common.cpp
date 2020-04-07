#include <time.h>
#include "common.h"

CGlobalData  g_data;

DWORD GetIntFromDb(const char * szValue, int nDefault /*= 0*/) {
	DWORD dwValue = nDefault;
	if (szValue)
		sscanf_s(szValue, "%lu", &dwValue);
	return dwValue;
}

char * GetStrFromdDb(char * buf, DWORD dwSize, const char * szValue) {
	assert(dwSize > 0);
	if (szValue)
		STRNCPY(buf, szValue, dwSize);
	else if (dwSize > 0)
		buf[0] = '\0';
	return buf;
}