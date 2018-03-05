#include "LmnString.h"
#include "39BarCode.h"

typedef struct tagBarCodeChar {
	char   ch;
	int    index[3];
}BarCodeChar;

static BarCodeChar table[] = {
	{ 48, { 4, 5, 7 } },
	{ 49, { 1, 4, 9 } },
	{ 50, { 3, 4, 9 } },
	{ 51, { 1, 3, 4 } },
	{ 52, { 4, 5, 9 } },
	{ 53, { 1, 4, 5 } },
	{ 54, { 3, 4, 5 } },
	{ 55, { 4, 7, 9 } },
	{ 56, { 1, 4, 7 } },
	{ 57, { 3, 4, 7 } },
	{ 65, { 1, 6, 9 } },
	{ 66, { 3, 6, 9 } },
	{ 67, { 1, 3, 6 } },
	{ 68, { 5, 6, 9 } },
	{ 69, { 1, 5, 6 } },
	{ 70, { 3, 5, 6 } },
	{ 71, { 6, 7, 9 } },
	{ 72, { 1, 6, 7 } },
	{ 73, { 3, 6, 7 } },
	{ 74, { 5, 6, 7 } },
	{ 75, { 1, 8, 9 } },
	{ 76, { 3, 8, 9 } },
	{ 77, { 1, 3, 8 } },
	{ 78, { 5, 8, 9 } },
	{ 79, { 1, 5, 8 } },
	{ 80, { 3, 5, 8 } },
	{ 81, { 7, 8, 9 } },
	{ 82, { 1, 7, 8 } },
	{ 83, { 3, 7, 8 } },
	{ 84, { 5, 7, 8 } },
	{ 85, { 1, 2, 9 } },
	{ 86, { 2, 3, 9 } },
	{ 87, { 1, 2, 3 } },
	{ 88, { 2, 5, 9 } },
	{ 89, { 1, 2, 5 } },
	{ 90, { 2, 3, 5 } },
	{ 45, { 2, 7, 9 } },
	{ 46, { 1, 2, 7 } },
	{ 32, { 2, 3, 7 } },
	{ 36, { 2, 4, 6 } },
	{ 47, { 2, 4, 8 } },
	{ 43, { 2, 6, 8 } },
	{ 37, { 4, 6, 8 } },
};

int DrawBarcode(HDC hdc, int x, int y, int w, int h, const char * szOriText) {
	if (0 == hdc || 0 == szOriText) {
		return 1;
	}

	if (w <= 0 || h <= 0) {
		return 1;
	}

	size_t  len = strlen(szOriText);

	if (0 == len) {
		return 0;
	}

	char * szText = new char[len + 1];
	memcpy(szText, szOriText, len + 1);

	Str2Upper(szText);

	for (int i = 0; i < (int)len; i++) {
		if ( !(szText[i] == 32 || szText[i] == 36 || szText[i] == 37 || szText[i] == 43 || (szText[i] >= 45 && szText[i] <= 49) || (szText[i] >= 50 && szText[i] <= 57)
			|| (szText[i] >= 65 && szText[i] <= 90)) ) {
			delete[] szText;
			return 2;
		}
	}

	// 计算宽度
	int  element_width = w / ((len + 2) * 13);

	// 长度不够，至少一个像素宽
	if (element_width == 0) {
		delete[] szText;
		return 3;
	}

	// 宽元素
	int  bold_width = 2 * element_width;

	// 单个元素的left, width
	int left  = 0;
	int width = 0;
	RECT  rect;
	rect.top = y;
	rect.bottom = y + h;
	

	// 画*号, 2 5 7
	for (int i = 0; i < 9; i++) {
		if (i == 1 || i == 4 || i == 6) {
			width = bold_width;
		}
		else {
			width = element_width;
		}

		rect.left   = left + x;
		rect.right  = rect.left + width;

		if (i % 2 == 0) {
			::FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		}
		else {
			::FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
		}

		left += width;
	}

	// 白色分割符
	width = element_width;
	rect.left = left + x;
	rect.right = rect.left + width;
	::FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	left += width;

	int table_elements_cnt = sizeof(table) / sizeof(table[0]);

	for (int i = 0; i < (int)len; i++) {
		int index = 0;
		for ( index = 0; index < table_elements_cnt; index++ ) {
			if (table[index].ch == szText[i]) {
				break;
			}
		}

		for (int j = 0; j < 9; j++) {

			if (j == table[index].index[0] - 1 || j == table[index].index[1] - 1 || j == table[index].index[2] - 1 ) {
				width = bold_width;
			}
			else {
				width = element_width;
			}

			rect.left = left + x;
			rect.right = rect.left + width;

			if (j % 2 == 0) {
				::FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
			}
			else {
				::FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
			}

			left += width;
		}

		// 白色分割符
		width = element_width;
		rect.left = left + x;
		rect.right = rect.left + width;
		::FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
		left += width;
	}

	// 画*号, 2 5 7
	for (int i = 0; i < 9; i++) {
		if (i == 1 || i == 4 || i == 6) {
			width = bold_width;
		}
		else {
			width = element_width;
		}

		rect.left = left + x;
		rect.right = rect.left + width;

		if (i % 2 == 0) {
			::FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		}
		else {
			::FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
		}

		left += width;
	}

	// 白色分割符
	width = element_width;
	rect.left = left + x;
	rect.right = rect.left + width;
	::FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
	left += width;

	delete[] szText;
	return 0;
}