#include <assert.h>
#include <string>
#include <vector>
#include <algorithm>

#include "LmnString.h"
#include "39BarCode.h"

using namespace std;

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

// int x, int y, int w, int h 条码的范围
// hText：文字高度, szTextPrefix: 文字前缀
int DrawBarcode(HDC hdc, int x, int y, int w, int h, const char * szOriText, HFONT hFont /* = 0 */, int hText /*= 0*/, const char * szTextPrefix /*= 0*/) {
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
	//if (element_width == 0) {
	//	element_width = 2;
	//}

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

	HFONT hOld = 0;
	if (hFont) {
		hOld = (HFONT)::SelectObject(hdc, hFont);
	}

	if ( hText ) {
		rect.left   = x;
		rect.right  = x + element_width * (len + 2) * 13;
		rect.top    = y + h;
		rect.bottom = rect.top + hText;

		CLmnString  str;
		if (szTextPrefix) {
			str = szTextPrefix;
		}
		str += szOriText;
		::DrawText(hdc, str, str.GetLength() , &rect, DT_LEFT | DT_TOP | DT_SINGLELINE);
	}
	

	if (hOld) {
		::SelectObject(hdc, hOld);
	}
	return 0;
}


















typedef struct tagBarCode128 {
	char   ch;
	char   achC[2];
	int    barcode[6];
}BarCode128;

static BarCode128 table_128[] = {
	{ ' ', { '0','0' }, { 2, 1, 2, 2, 2, 2 } },
	{ '!', { '0','1' }, { 2, 2, 2, 1, 2, 2 } },
	{ '"', { '0','2' }, { 2, 2, 2, 2, 2, 1 } },
	{ '#', { '0','3' }, { 1, 2, 1, 2, 2, 3 } },
	{ '$', { '0','4' }, { 1, 2, 1, 3, 2, 2 } },
	{ '%', { '0','5' }, { 1, 3, 1, 2, 2, 2 } },
	{ '&', { '0','6' }, { 1, 2, 2, 2, 1, 3 } },
	{ '\'',{ '0','7' }, { 1, 2, 2, 3, 1, 2 } },
	{ '(', { '0','8' }, { 1, 3, 2, 2, 1, 2 } },
	{ ')', { '0','9' }, { 2, 2, 1, 2, 1, 3 } },

	{ '*', { '1','0' }, { 2, 2, 1, 3, 1, 2 } },
	{ '+', { '1','1' }, { 2, 3, 1, 2, 1, 2 } },
	{ ',', { '1','2' }, { 1, 1, 2, 2, 3, 2 } },
	{ '-', { '1','3' }, { 1, 2, 2, 1, 3, 2 } },
	{ '.', { '1','4' }, { 1, 2, 2, 2, 3, 1 } },
	{ '/', { '1','5' }, { 1, 1, 3, 2, 2, 2 } },
	{ '0', { '1','6' }, { 1, 2, 3, 1, 2, 2 } },
	{ '1', { '1','7' }, { 1, 2, 3, 2, 2, 1 } },
	{ '2', { '1','8' }, { 2, 2, 3, 2, 1, 1 } },
	{ '3', { '1','9' }, { 2, 2, 1, 1, 3, 2 } },

	{ '4', { '2','0' }, { 2, 2, 1, 2, 3, 1 } },
	{ '5', { '2','1' }, { 2, 1, 3, 2, 1, 2 } },
	{ '6', { '2','2' }, { 2, 2, 3, 1, 1, 2 } },
	{ '7', { '2','3' }, { 3, 1, 2, 1, 3, 1 } },
	{ '8', { '2','4' }, { 3, 1, 1, 2, 2, 2 } },
	{ '9', { '2','5' }, { 3, 2, 1, 1, 2, 2 } },
	{ ':', { '2','6' }, { 3, 2, 1, 2, 2, 1 } },
	{ ';', { '2','7' }, { 3, 1, 2, 2, 1, 2 } },
	{ '<', { '2','8' }, { 3, 2, 2, 1, 1, 2 } },
	{ '=', { '2','9' }, { 3, 2, 2, 2, 1, 1 } },

	{ '>', { '3','0' }, { 2, 1, 2, 1, 2, 3 } },
	{ '?', { '3','1' }, { 2, 1, 2, 3, 2, 1 } },
	{ '@', { '3','2' }, { 2, 3, 2, 1, 2, 1 } },
	{ 'A', { '3','3' }, { 1, 1, 1, 3, 2, 3 } },
	{ 'B', { '3','4' }, { 1, 3, 1, 1, 2, 3 } },
	{ 'C', { '3','5' }, { 1, 3, 1, 3, 2, 1 } },
	{ 'D', { '3','6' }, { 1, 1, 2, 3, 1, 3 } },
	{ 'E', { '3','7' }, { 1, 3, 2, 1, 1, 3 } },
	{ 'F', { '3','8' }, { 1, 3, 2, 3, 1, 1 } },
	{ 'G', { '3','9' }, { 2, 1, 1, 3, 1, 3 } },

	{ 'H', { '4','0' }, { 2, 3, 1, 1, 1, 3 } },
	{ 'I', { '4','1' }, { 2, 3, 1, 3, 1, 1 } },
	{ 'J', { '4','2' }, { 1, 1, 2, 1, 3, 3 } },
	{ 'K', { '4','3' }, { 1, 1, 2, 3, 3, 1 } },
	{ 'L', { '4','4' }, { 1, 3, 2, 1, 3, 1 } },
	{ 'M', { '4','5' }, { 1, 1, 3, 1, 2, 3 } },
	{ 'N', { '4','6' }, { 1, 1, 3, 3, 2, 1 } },
	{ 'O', { '4','7' }, { 1, 3, 3, 1, 2, 1 } },
	{ 'P', { '4','8' }, { 3, 1, 3, 1, 2, 1 } },
	{ 'Q', { '4','9' }, { 2, 1, 1, 3, 3, 1 } },

	{ 'R', { '5','0' }, { 2, 3, 1, 1, 3, 1 } },
	{ 'S', { '5','1' }, { 2, 1, 3, 1, 1, 3 } },
	{ 'T', { '5','2' }, { 2, 1, 3, 3, 1, 1 } },
	{ 'U', { '5','3' }, { 2, 1, 3, 1, 3, 1 } },
	{ 'V', { '5','4' }, { 3, 1, 1, 1, 2, 3 } },
	{ 'W', { '5','5' }, { 3, 1, 1, 3, 2, 1 } },
	{ 'X', { '5','6' }, { 3, 3, 1, 1, 2, 1 } },
	{ 'Y', { '5','7' }, { 3, 1, 2, 1, 1, 3 } },
	{ 'Z', { '5','8' }, { 3, 1, 2, 3, 1, 1 } },
	{ '[', { '5','9' }, { 3, 3, 2, 1, 1, 1 } },

	{ '\\',{ '6','0' }, { 3, 1, 4, 1, 1, 1 } },
	{ ']', { '6','1' }, { 2, 2, 1, 4, 1, 1 } },
	{ '^', { '6','2' }, { 4, 3, 1, 1, 1, 1 } },
	{ '_', { '6','3' }, { 1, 1, 1, 2, 2, 4 } },
	{ '`', { '6','4' }, { 1, 1, 1, 4, 2, 2 } },
	{ 'a', { '6','5' }, { 1, 2, 1, 1, 2, 4 } },
	{ 'b', { '6','6' }, { 1, 2, 1, 4, 2, 1 } },
	{ 'c', { '6','7' }, { 1, 4, 1, 1, 2, 2 } },
	{ 'd', { '6','8' }, { 1, 4, 1, 2, 2, 1 } },
	{ 'e', { '6','9' }, { 1, 1, 2, 2, 1, 4 } },

	{ 'f', { '7','0' }, { 1, 1, 2, 4, 1, 2 } },
	{ 'g', { '7','1' }, { 1, 1, 2, 1, 1, 4 } },
	{ 'h', { '7','2' }, { 1, 2, 2, 4, 1, 1 } },
	{ 'i', { '7','3' }, { 1, 4, 2, 1, 1, 2 } },
	{ 'j', { '7','4' }, { 1, 4, 2, 2, 1, 1 } },
	{ 'k', { '7','5' }, { 2, 4, 1, 2, 1, 1 } },
	{ 'l', { '7','6' }, { 2, 2, 1, 1, 1, 4 } },
	{ 'm', { '7','7' }, { 4, 1, 3, 1, 1, 1 } },
	{ 'n', { '7','8' }, { 2, 4, 1, 1, 1, 2 } },
	{ 'o', { '7','9' }, { 1, 3, 4, 1, 1, 1 } },

	{ 'p', { '8','0' }, { 1, 1, 1, 2, 4, 2 } },
	{ 'q', { '8','1' }, { 1, 2, 1, 1, 4, 2 } },
	{ 'r', { '8','2' }, { 1, 2, 1, 2, 4, 1 } },
	{ 's', { '8','3' }, { 1, 1, 4, 2, 1, 2 } },
	{ 't', { '8','4' }, { 1, 2, 4, 1, 1, 2 } },
	{ 'u', { '8','5' }, { 1, 2, 4, 2, 1, 1 } },
	{ 'v', { '8','6' }, { 4, 1, 1, 2, 1, 2 } },
	{ 'w', { '8','7' }, { 4, 2, 1, 1, 1, 2 } },
	{ 'x', { '8','8' }, { 4, 2, 1, 2, 1, 1 } },
	{ 'y', { '8','9' }, { 2, 1, 2, 1, 4, 1 } },

	{ 'z', { '9','0' }, { 2, 1, 4, 1, 2, 1 } },
	{ '{', { '9','1' }, { 4, 1, 2, 1, 2, 1 } },
	{ '|', { '9','2' }, { 1, 1, 1, 1, 4, 3 } },
	{ '}', { '9','3' }, { 1, 1, 1, 3, 4, 1 } },
	{ '~', { '9','4' }, { 1, 3, 1, 1, 4, 1 } },
	{ (char)200, { '9','5' }, { 1, 1, 4, 1, 1, 3 } },
	{ (char)201, { '9','6' }, { 1, 1, 4, 3, 1, 1 } },
	{ (char)202, { '9','7' }, { 4, 1, 1, 1, 1, 3 } },
	{ (char)203, { '9','8' }, { 4, 1, 1, 3, 1, 1 } },
	{ (char)204, { '9','9' }, { 1, 1, 3, 1, 4, 1 } },

	{ (char)205,{ 'a','0' },{ 1, 1, 4, 1, 3, 1 } },
	{ (char)206,{ 'a','1' },{ 3, 1, 1, 1, 4, 1 } },
	{ (char)207,{ 'a','2' },{ 4, 1, 1, 1, 3, 1 } },

};

#define CODE128_B   0
#define CODE128_C   1

typedef struct tagCodeSegment {
	int     nType;    // 0: CODE128B, 1: CODE128C
	string  strCode;
}CodeSegment;

static const int START_B[6] = { 2, 1, 1, 2, 1, 4 };
static const int START_C[6] = { 2, 1, 1, 2, 3, 2 };
static const int CODE_B[6]  = { 1, 1, 4, 1, 3, 1 };
static const int CODE_C[6]  = { 1, 1, 3, 1, 4, 1 };
static const int STOP[7]    = { 2, 3, 3, 1, 1, 1, 2 };



void FillCode128(HDC hdc, RECT & rect, const int barcode[], int length, int element_width) {
	for (int i = 0; i < length; i++) {
		rect.right = rect.left + barcode[i]* element_width;

		if (i % 2 == 0) {
			::FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		}
		else {
			::FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
		}

		rect.left = rect.right;
	}	
}

int FindInCode128(char ch) {
	int n = sizeof(table_128) / sizeof(BarCode128) - 8;
	for (int i = 0; i < n; i++) {
		if (table_128[i].ch == ch) {
			return i;
		}
	}
	return -1;
}

int FindInCode128(char ch0, char ch1) {
	int n = sizeof(table_128) / sizeof(BarCode128);
	for (int i = 0; i < n; i++) {
		if (table_128[i].achC[0] == ch0 && table_128[i].achC[1] == ch1 ) {
			return i;
		}
	}
	return -1;
}

// range: A-Z, a-z, 0-9,其他一些可见字符
//        32 - 126
// 连续6个数字以上使用CODE128C
int DrawBarcode128(HDC hdc, int x, int y, int w, int h, const char * szOriText, HFONT hFont /*= 0*/, int hText /*= 0*/, const char * szTextPrefix /*= 0*/) {

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

	for (int i = 0; i < (int)len; i++) {
		// 超出范围
		if ( !(szOriText[i] >= 32 && szOriText[i] <= 126) ) {
			return 2;
		}
	}

	vector<CodeSegment *> v;
	CodeSegment * pSegment = 0;   //保存code128B, 128C
	string   strNumbers;   // 保存临时数字

	for (int i = 0; i < (int)len; i++) {
		// 如果是数字
		if ( szOriText[i] >= '0' && szOriText[i] <= '9' ) {
			strNumbers += szOriText[i];
		}
		// 如果非数字
		else {
			// 存储的数字
			if ( strNumbers.length() > 0 ) {
				// 如果到达用code128C的要求
				if (strNumbers.length() >= 6) {
					// 如果前有segment 128B没有处理完
					if ( pSegment ) {
						v.push_back(pSegment);
					}

					// 是2的倍数
					if (strNumbers.length() % 2 == 0) {
						pSegment = new CodeSegment;
						pSegment->nType = CODE128_C;
						pSegment->strCode = strNumbers;
						v.push_back(pSegment);

						pSegment = 0;
					}
					else {
						pSegment = new CodeSegment;
						pSegment->nType = CODE128_C;
						pSegment->strCode = strNumbers.substr( 0, strNumbers.length() - 1 );						
						v.push_back(pSegment);

						pSegment = new CodeSegment;
						pSegment->nType = CODE128_B;
						pSegment->strCode = strNumbers.at(strNumbers.length()-1);
					}
				}
				else {
					if (0 == pSegment) {
						pSegment = new CodeSegment;
						pSegment->nType = CODE128_B;
					}
					else {
						assert(pSegment->nType == CODE128_B);
					}
					pSegment->strCode += strNumbers;
				}
				strNumbers = "";
			}

			if (0 == pSegment) {
				pSegment = new CodeSegment;
				pSegment->nType = CODE128_B;
			}
			else {
				assert(pSegment->nType == CODE128_B);							
			}
			pSegment->strCode += szOriText[i];


		}
	}

	// 存储的数字
	if (strNumbers.length() > 0) {
		// 如果到达用code128C的要求
		if (strNumbers.length() >= 6) {
			// 如果前有segment 128B没有处理完
			if (pSegment) {
				v.push_back(pSegment);
			}

			// 是2的倍数
			if (strNumbers.length() % 2 == 0) {
				pSegment = new CodeSegment;
				pSegment->nType = CODE128_C;
				pSegment->strCode = strNumbers;
				v.push_back(pSegment);

				pSegment = 0;
			}
			else {
				pSegment = new CodeSegment;
				pSegment->nType = CODE128_C;
				pSegment->strCode = strNumbers.substr(0, strNumbers.length() - 1);
				v.push_back(pSegment);

				pSegment = new CodeSegment;
				pSegment->nType = CODE128_B;
				pSegment->strCode = strNumbers.at(strNumbers.length() - 1);
			}
		}
		else {
			if (0 == pSegment) {
				pSegment = new CodeSegment;
				pSegment->nType = CODE128_B;
			}
			else {
				assert(pSegment->nType == CODE128_B);
			}
			pSegment->strCode += strNumbers;
		}
		strNumbers = "";
	}

	if (pSegment) {
		v.push_back(pSegment);
	}
	

	// 计算宽度
	int  code_cnt = 0;
	vector<CodeSegment *>::iterator it;

	for (it = v.begin(); it != v.end(); it++) {
		CodeSegment * pSegment = *it;
		if (pSegment->nType == CODE128_B) {
			code_cnt += pSegment->strCode.length();
		}
		else {
			code_cnt += pSegment->strCode.length() / 2;
		}
	}

	code_cnt += v.size() + 1 + 1;

	int  element_width = w / (code_cnt * 11+1);
	//if (element_width == 0) {
	//	element_width = 2;
	//}

	// 长度不够，至少一个像素宽
	if (element_width == 0) {
		for (it = v.begin(); it != v.end(); it++) {
			CodeSegment * pSegment = *it;
			delete pSegment;
		}
		v.clear();
		return 3;
	}

	RECT  rect;
	rect.top = y;
	rect.bottom = y + h;
	rect.left = x;
	rect.right = 0;

	int i = 0;
	int check_sum = 0;
	for (it = v.begin(); it != v.end(); it++) {
		CodeSegment * pSegment = *it;

		if (pSegment->nType == CODE128_B) {
			// 开始 211214
			if (0 == i) {
				FillCode128(hdc, rect, START_B, 6, element_width);
				check_sum += 104;
			}
			else {
				FillCode128(hdc, rect, CODE_B, 6, element_width);
				check_sum += 100 * i;
			}
			i++;

			for (int j = 0; j < (int)pSegment->strCode.length(); j++,i++) {
				char ch = pSegment->strCode[j];
				int index = FindInCode128(ch);
				assert(index >= 0);
				FillCode128(hdc, rect, table_128[index].barcode, 6, element_width);
				check_sum += index * i;
			}
		}
		else {
			// 开始
			if (0 == i) {
				FillCode128(hdc, rect, START_C, 6, element_width);
				check_sum += 105;
			}
			else {
				FillCode128(hdc, rect, CODE_C, 6, element_width);
				check_sum += 99 * i;
			}
			i++;

			for (int j = 0; j < (int)pSegment->strCode.length(); j+=2, i++) {
				char ch0 = pSegment->strCode[j];
				char ch1 = pSegment->strCode[j+1];

				int index = FindInCode128(ch0, ch1);
				assert(index >= 0);
				FillCode128(hdc, rect, table_128[index].barcode, 6, element_width);
				check_sum += index * i;
			}
		}
	}
	

	//校验
	int check_index = check_sum % 103;
	FillCode128(hdc, rect, table_128[check_index].barcode, 6, element_width);

	// 结束
	FillCode128(hdc, rect, STOP, 7, element_width);

	
	for (it = v.begin(); it != v.end(); it++) {
		CodeSegment * pSegment = *it;
		delete pSegment;
	}
	v.clear();


	HFONT hOld = 0;
	if (hFont) {
		hOld = (HFONT)::SelectObject(hdc, hFont);
	}

	if (hText) {
		rect.left   = x;
		rect.top    = y + h;
		rect.bottom = rect.top + hText;

		CLmnString  str;
		if (szTextPrefix) {
			str = szTextPrefix;
		}
		str += szOriText;
		::DrawText(hdc, str, str.GetLength(), &rect, DT_LEFT | DT_TOP | DT_SINGLELINE);
	}


	if (hOld) {
		::SelectObject(hdc, hOld);
	}

	return 0;
}