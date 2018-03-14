#pragma once

#include <windows.h>

extern int DrawBarcode(HDC hdc, int x, int y, int w, int h, const char * szText, HFONT hFont = 0, int hText = 0, const char * szTextPrefix = 0);

extern int DrawBarcode128(HDC hdc, int x, int y, int w, int h, const char * szText, HFONT hFont = 0, int hText = 0, const char * szTextPrefix = 0);