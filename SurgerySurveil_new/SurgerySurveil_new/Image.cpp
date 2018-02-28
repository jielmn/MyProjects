#include <time.h>
#include "Image.h"


CMyImageUI::CMyImageUI(CPaintManagerUI *pManager) {
	m_pManager = pManager;
	
	m_hPen = ::CreatePen(0, 1, RGB(0, 128, 64));
	m_hPen1 = ::CreatePen(0, 2, RGB(0, 128, 64));
	m_hPen2 = ::CreatePen(0, 1, RGB(0, 0, 255));


	DWORD   dwValue;

	g_cfg->GetConfig("LEFT", dwValue, 50);
	m_nLeft = (int)dwValue;

	g_cfg->GetConfig("GRID SIZE", dwValue, 50);
	m_nGridSize = dwValue;

	g_cfg->GetConfig("text offset x", dwValue, -25);
	m_nTextOffsetX = dwValue;

	g_cfg->GetConfig("text offset y", dwValue, -8);
	m_nTextOffsetY = dwValue;

	g_cfg->GetConfig("time offset x", dwValue, 1);
	m_nTimeOffsetX = dwValue;

	g_cfg->GetConfig("time offset y", dwValue, 1);
	m_nTimeOffsetY = dwValue;

	g_cfg->GetConfig("time unit length", dwValue, 1);
	m_nTimeUnitLen = dwValue;

	g_cfg->GetConfig("max time span", dwValue, 72);
	m_nMaxTimeSpan = dwValue;
	
	
}

CMyImageUI::~CMyImageUI() {
	ClearVector(m_vTempData);
}

bool CMyImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	RECT rect   = this->GetPos();
	int  width  = rect.right - rect.left;
	int  height = rect.bottom - rect.top;

	int RIGHT = width - m_nLeft;

	// 38度位置
	int middle = height / 2;

	::SelectObject(hDC, m_hPen);
	::SetTextColor(hDC, RGB(68, 122, 161));

	::MoveToEx(hDC, m_nLeft + rect.left, rect.top, 0);
	::LineTo( hDC, m_nLeft + rect.left, rect.bottom );

	int nFirstTop = middle - m_nGridSize * 4;
	int nFistTemperature = 42;
	CDuiString strText;

	for (int i = 0; i < 9; i++) {
		int  nTop = nFirstTop + i * m_nGridSize;
		int  nTemperature = nFistTemperature - i;

		::MoveToEx(hDC, m_nLeft + rect.left, nTop + rect.top, 0);
		::LineTo(hDC, rect.right, nTop + rect.top);

		
		strText.Format("%d", nTemperature);
		::TextOut(hDC, m_nLeft + rect.left + m_nTextOffsetX, nTop + rect.top + m_nTextOffsetY, strText, strText.GetLength());
	}

	time_t  tFistTime = 0;
	if ( m_vTempData.size() > 0 ) {
		TempData * pFist = m_vTempData[0];

		struct tm tTmTime;
		localtime_s(&tTmTime, &pFist->tTime);

		strText.Format("%02d:%02d:%02d", tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);
		::TextOut(hDC, m_nLeft + rect.left + m_nTimeOffsetX, middle + 4 * m_nGridSize + rect.top + m_nTimeOffsetY, strText, strText.GetLength());

		tFistTime = pFist->tTime;
	}

	vector<TempData *>::iterator it;
	for (it = m_vTempData.begin(); it != m_vTempData.end(); it++) {
		TempData * pItem = *it;

		int nDiff = (int)(pItem->tTime - tFistTime);
		int nX = nDiff * m_nTimeUnitLen;
		int nY = (int)((pItem->dwTemperature - 3800.0) / 100.0 * m_nGridSize);

		DrawTempPoint(nX + m_nLeft + rect.left, nY + middle + rect.top, hDC, 4);
		
		if (it == m_vTempData.begin()) {
			::MoveToEx(hDC, nX + m_nLeft + rect.left, nY + middle + rect.top, 0);
		}
		else {
			::LineTo(hDC, nX + m_nLeft + rect.left, nY + middle + rect.top);
		}
	}


	return true;
	
}

void CMyImageUI::DoEvent(TEventUI& event) {


	CControlUI::DoEvent(event);
}

void  CMyImageUI::AddTemp(DWORD dwTemp) {
	time_t now = time(0);

	if (m_vTempData.size() > 0) {
		TempData * pFist = m_vTempData[0];
		// 如果超过最大跨度
		if ( (int)(now - pFist->tTime) > m_nMaxTimeSpan ) {
			ClearVector(m_vTempData);
		}
	}

	TempData * pTemp = new TempData;
	pTemp->dwTemperature = dwTemp;
	pTemp->tTime = now;
	m_vTempData.push_back(pTemp);

	Invalidate();
}

void   CMyImageUI::DrawTempPoint(int x, int y, HDC hDc, int RADIUS /*= 6*/) {
	::Ellipse(hDc, x - RADIUS, y - RADIUS, x + RADIUS, y + RADIUS);

	int nTmp = (int)(0.707 * RADIUS);

	POINT points[2] = { { x - nTmp, y - nTmp },{ x + nTmp, y + nTmp } };
	::Polyline(hDc, points, 2);

	POINT points1[2] = { { x + nTmp, y - nTmp },{ x - nTmp, y + nTmp } };
	::Polyline(hDc, points1, 2);
}