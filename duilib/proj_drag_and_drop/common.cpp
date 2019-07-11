#include <time.h>
#include "common.h"

CGlobalData  g_data;

BOOL RectOverlap(const RECT & rA, const RECT & rB) {

	if (!(rA.bottom < rB.top || rA.top > rB.bottom) && !(rA.right < rB.left || rA.left > rB.right))

		return TRUE;

	return FALSE;

};