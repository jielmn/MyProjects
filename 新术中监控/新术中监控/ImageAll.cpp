#include <time.h>
#include "ImageAll.h"

CImageAll::CImageAll(DuiLib::CPaintManagerUI *pManager) {

	m_hPenGrid   = ::CreatePen(PS_SOLID, 1, RGB(0, 128, 64));
	m_hPenThread = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 255));

	m_brush = ::CreateSolidBrush(RGB(255, 255, 255));

	m_bSetParentScrollPos = FALSE;
}

CImageAll::~CImageAll() {
	ClearVector(m_vTempData);   
	DeleteObject(m_hPenGrid);
	DeleteObject(m_hPenThread);
}

bool CImageAll::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	DuiLib::CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	DuiLib::CHorizontalLayoutUI * pParent = (DuiLib::CHorizontalLayoutUI *)this->GetParent();
	SIZE tParentScrollPos   = pParent->GetScrollPos();
	SIZE tParentScrollRange = pParent->GetScrollRange();

	if ( tParentScrollPos.cx != tParentScrollRange.cx ) {
		if (m_bSetParentScrollPos)
		{
			pParent->SetScrollPos(tParentScrollRange);
			m_bSetParentScrollPos = FALSE;
		}		
	}

	RECT rect = this->GetPos();

	int  width = pParent->GetWidth(); //rect.right - rect.left;
	int  height = rect.bottom - rect.top;

	DWORD  dwTemp = 0;
	DWORD  dwMinTemp = 0;
	DWORD  dwMaxTemp = 0;
	DWORD  dwBarHeight = 0;
	DWORD  dwLeftColumnWidth = 0;
	DuiLib::CDuiString  strText;

	g_cfg->GetConfig("min temperature", dwMinTemp, 20);
	g_cfg->GetConfig("max temperature", dwMaxTemp, 42);
	if ( dwMinTemp >= dwMaxTemp ) {
		return true;
	}

	HPEN hOldPen = (HPEN)::SelectObject(hDC, m_hPenGrid);

	g_cfg->GetConfig("image bar height", dwBarHeight, 20);
	DWORD  dwTempPartHeight = height - dwBarHeight - 20;

	int  nTempSpan = dwMaxTemp - dwMinTemp;
	int  nTempHighPerUnit = dwTempPartHeight / nTempSpan;
	int  nTempHighMargin = (dwTempPartHeight - nTempHighPerUnit * nTempSpan ) / 2;
	int  nMinTempY = rect.bottom - dwBarHeight - 10 - nTempHighMargin;

	// 温度水平线
	int nY = nMinTempY;
	for ( int i = 0; i <= nTempSpan; i++ ) {
		::MoveToEx(hDC, rect.left+1, nY, 0);
		::LineTo(hDC, rect.right-1, nY);
		nY -= nTempHighPerUnit;
	}

	g_cfg->GetConfig("image left column width", dwLeftColumnWidth, 60);

	DWORD  dwInterval = 0;
	DWORD  dwUnitWidth = 0;
	g_cfg->GetConfig("image collect interval", dwInterval, DEFAULT_COLLECT_INTERVAL);
	if (dwInterval < 5) {
		dwInterval = 5;
	}

	g_cfg->GetConfig("image unit width", dwUnitWidth, IMAGE_UNIT_WIDTH);
	if (dwUnitWidth == 0) {
		dwUnitWidth = IMAGE_UNIT_WIDTH;
	}
	
	// 画曲线
	DWORD dwCnt = m_vTempData.size();
	int nX = 0;

	for ( DWORD i = 0; i < dwCnt; i++ ) {
		int nTempDiff   = m_vTempData[i]->dwTemperature - dwMinTemp * 100;
		int nHeightDiff = (int)(nTempDiff / 100.0 * nTempHighPerUnit);
		nY = nMinTempY - nHeightDiff;
		nX = (int)((double)(m_vTempData[i]->tTime - m_vTempData[0]->tTime) / dwInterval * dwUnitWidth) + dwLeftColumnWidth + rect.left;

		DrawTempPoint(nX, nY, hDC);
		if ( 0== i ) {
			::MoveToEx(hDC, nX, nY, 0);
		}
		else {
			::LineTo(hDC, nX, nY);
		}
	}


	RECT rectLeft;
	rectLeft.left = rect.left + tParentScrollPos.cx;
	rectLeft.top = rect.top;
	rectLeft.right = rectLeft.left + dwLeftColumnWidth;
	rectLeft.bottom = rect.bottom;

	::SetTextColor(hDC, RGB(0x44, 0x7A, 0xA1));

	// 画左边区域（温度水平线的刻度）
	::FillRect(hDC, &rectLeft, m_brush);	
	::MoveToEx(hDC, rect.left+dwLeftColumnWidth+ tParentScrollPos.cx, rect.top, 0);
	::LineTo(hDC, rect.left + dwLeftColumnWidth+ tParentScrollPos.cx, rect.bottom);

	nY = nMinTempY;
	for (int i = 0; i <= nTempSpan; i++) {
		strText.Format("%lu℃", dwMinTemp+i);
		::TextOut(hDC, rect.left + dwLeftColumnWidth + tParentScrollPos.cx - 42, nY - 8, (const char *)strText, strText.GetLength());
		nY -= nTempHighPerUnit;
	}

	RECT rectBorder;
	rectBorder.left = rectLeft.left;
	rectBorder.right = rectBorder.left + width;
	rectBorder.top = rect.top;
	rectBorder.bottom = rect.bottom;
	::Rectangle(hDC, rectBorder.left, rectBorder.top, rectBorder.right, rectBorder.bottom);

	::SelectObject(hDC, hOldPen);
	return true;
}

void CImageAll::DoEvent(DuiLib::TEventUI& event) {
	DuiLib::CControlUI::DoEvent(event);
}

void   CImageAll::DrawTempPoint(int x, int y, HDC hDc, int RADIUS /*= 6*/) {
	::Ellipse(hDc, x - RADIUS, y - RADIUS, x + RADIUS, y + RADIUS);

	int nTmp = (int)(0.707 * RADIUS);

	POINT points[2] = { { x - nTmp, y - nTmp },{ x + nTmp, y + nTmp } };
	::Polyline(hDc, points, 2);

	POINT points1[2] = { { x + nTmp, y - nTmp },{ x - nTmp, y + nTmp } };
	::Polyline(hDc, points1, 2);
}

int  CImageAll::CalcMinWidth() {
	DWORD dwCnt = m_vTempData.size();
	if (dwCnt <= 1) {
		return 0;
	}

	DWORD  dwInterval = 0;
	DWORD  dwUnitWidth = 0;

	g_cfg->GetConfig( "image collect interval", dwInterval, DEFAULT_COLLECT_INTERVAL );
	if ( dwInterval < 5 ) {
		dwInterval = 5;
	}

	g_cfg->GetConfig( "image unit width", dwUnitWidth, IMAGE_UNIT_WIDTH );
	if ( dwUnitWidth == 0 ) {
		dwUnitWidth = IMAGE_UNIT_WIDTH;
	}

	DWORD  dwLeftColumnWidth = 0;
	g_cfg->GetConfig("image left column width", dwLeftColumnWidth, 60);

	int nWidth = (int)( (double)(m_vTempData[dwCnt - 1]->tTime - m_vTempData[0]->tTime) / dwInterval * dwUnitWidth );
	return nWidth + dwLeftColumnWidth;
}


void  CImageAll::AddTempData(DWORD dwTemp) {
	time_t  now = time(0);

	TempData  * pItem = new TempData;
	pItem->dwTemperature = dwTemp;
	pItem->tTime = now;
	m_vTempData.push_back(pItem);

	int nMinWidth = CalcMinWidth();
	this->SetMinWidth(nMinWidth);

	this->Invalidate();

	// 在重绘的时候，设置scroll position
	m_bSetParentScrollPos = TRUE;
}