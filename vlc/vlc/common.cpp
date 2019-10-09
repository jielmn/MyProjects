#include <time.h>
#include "common.h"
#include <assert.h>

CGlobalData  g_data;

char *  VlcTime2Str(char * szTime, DWORD dwTimeSize, int nTime) {
	assert(szTime);
	int tns = nTime / 1000;
	int thh = tns / 3600;
	int tmm = (tns % 3600) / 60;
	int tss = (tns % 60);
	SNPRINTF(szTime, dwTimeSize, "%02d:%02d:%02d", thh, tmm, tss);
	return szTime;
}