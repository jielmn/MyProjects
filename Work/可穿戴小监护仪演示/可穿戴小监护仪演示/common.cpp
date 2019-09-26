#include <time.h>
#include "common.h"

CGlobalData  g_data;

BOOL  IsAbnormal(BYTE beat, BYTE beatV, BYTE byOxy, BYTE byOxyV, int nTemp) {
	if (beatV > 0) {
		if ( beat <= g_data.m_byMinBeat || beat >= g_data.m_byMaxBeat ) {
			return TRUE;
		}
	}

	if (byOxyV > 0) {
		if (byOxy <= g_data.m_byMinOxy) {
			return TRUE;
		}
	}

	if (nTemp >= g_data.m_nMaxTemp || nTemp <= g_data.m_nMinTemp)
		return TRUE;

	return FALSE;
}