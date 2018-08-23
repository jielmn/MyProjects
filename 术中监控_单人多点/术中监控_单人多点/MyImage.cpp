#include "MyImage.h"

CMyImageUI::CMyImageUI() {
	m_hCommonThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x66, 0x66, 0x66));
	m_hBrighterThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x99, 0x99, 0x99));
}

CMyImageUI::~CMyImageUI() {
	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		ClearVector(m_vTempData[i]);
	}
}

bool CMyImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	DuiLib::CControlUI::DoPaint(hDC, rcPaint, pStopControl);
	DuiLib::CDuiString strText;

	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	// 父节点: vertical layout
	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	SIZE tParentScrollPos = pParent->GetScrollPos();
	SIZE tParentScrollRange = pParent->GetScrollRange();

	RECT rect   = this->GetPos();
	int  width  = pParent->GetWidth();
	int  height = rect.bottom - rect.top; 
	
	/* 开始作图 */
	int nMinTemp = g_data.m_dwMyImageMinTemp;
	int nGridCount = MAX_TEMPERATURE - nMinTemp;

	int nGridHeight = height / nGridCount;
	int nReminder = height % nGridCount;
	int nVMargin = MIN_MYIMAGE_VMARGIN;

	if (nVMargin * 2 > nReminder) {
		int nSpared = (nVMargin * 2 - nReminder - 1) / nGridCount + 1;
		nGridHeight -= nSpared;
	}

	// 中间温度的位置
	int middle = height / 2;

	::SetTextColor(hDC, RGB(255,255,255) );
	::SelectObject(hDC, m_hCommonThreadPen );

	RECT rectLeft;
	rectLeft.left   = rect.left + tParentScrollPos.cx;
	rectLeft.top    = rect.top;
	rectLeft.right  = rectLeft.left + g_data.m_dwMyImageLeftBlank;
	rectLeft.bottom = rect.bottom;

	int nFirstTop = middle - nGridHeight * (nGridCount / 2);
	int nFistTemperature = MAX_TEMPERATURE;

	// 画出刻度线(水平横线)
	int nVInterval = MIN_TEMP_V_INTERVAL;
	for (int i = 0; i < nGridCount + 1; i++) {
		if (nVInterval >= MIN_TEMP_V_INTERVAL) {
			::SelectObject(hDC, m_hBrighterThreadPen);
			nVInterval = nGridHeight;
		}
		else {
			::SelectObject(hDC, m_hCommonThreadPen);
			nVInterval += nGridHeight;
		}
		int  nTop = nFirstTop + i * nGridHeight;
		int  nTemperature = nFistTemperature - i;
		::MoveToEx(hDC, rectLeft.right, nTop + rect.top, 0);
		::LineTo(hDC, rect.right, nTop + rect.top);
	}

	::SelectObject(hDC, m_hCommonThreadPen );

	/* 画温度曲线 */
	time_t  tFirstTime = GetFirstTime();
	time_t  tLastTime = GetLastTime();
	int nMiddleTemp = (MAX_TEMPERATURE + nMinTemp) / 2;

	vector<TempData *>::iterator it;
	for ( DWORD i = 0; i < MAX_READERS_COUNT; i++ ) {
		vector<TempData *> & vTempData = m_vTempData[i];

		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;

			int nDiff = (int)(pItem->tTime - tFirstTime);
			int nX = (int)(((double)nDiff / g_data.m_dwCollectInterval) * g_data.m_dwCollectIntervalWidth);
			int nY = (int)((nMiddleTemp * 100.0 - (double)pItem->dwTemperature) / 100.0 * nGridHeight);

			//DrawTempPoint(graphics, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, hDC, nRadius);

			//if (it == vTempData.begin()) {
			//	::MoveToEx(hDC, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, 0);
			//}
			//else {
			//	POINT pt;
			//	::GetCurrentPositionEx(hDC, &pt);
			//	graphics.DrawLine(&m_temperature_pen, pt.x, pt.y, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top);
			//	::MoveToEx(hDC, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, 0);
			//}
		}
	}
	

	return true;
}

void CMyImageUI::DoEvent(DuiLib::TEventUI& event) {
	DuiLib::CControlUI::DoEvent(event);	
}

void  CMyImageUI::AddTemp(DWORD dwIndex, DWORD dwTemp) {

}

time_t  CMyImageUI::GetFirstTime() {
	time_t  tTime = 0x7FFFFFFF;
	BOOL    bFind = FALSE;

	for ( DWORD i = 0; i < MAX_READERS_COUNT; i++ ) {
		if ( m_vTempData[i].size() > 0  ) {
			if (!bFind) {
				tTime = m_vTempData[i].at(0)->tTime;
				bFind = TRUE;
			}
			else if ( m_vTempData[i].at(0)->tTime < tTime ) {
				tTime = m_vTempData[i].at(0)->tTime;
			}
		}
	}

	if (bFind) {
		return tTime;
	}		
	else {
		return -1;
	}
}

time_t  CMyImageUI::GetLastTime() {
	time_t  tTime = -1;
	BOOL    bFind = FALSE;

	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		if (m_vTempData[i].size() > 0) {
			if (!bFind) {
				tTime = m_vTempData[i].at(m_vTempData[i].size()-1)->tTime;
				bFind = TRUE;
			}
			else if (m_vTempData[i].at(m_vTempData[i].size() - 1)->tTime > tTime) {
				tTime = m_vTempData[i].at(m_vTempData[i].size() - 1)->tTime;
			}
		}
	}

	if (bFind) {
		return tTime;
	}
	else {
		return -1;
	}
}