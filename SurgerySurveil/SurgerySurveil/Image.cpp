#include "Image.h"

#define  GRID_SIZE     20
#define  SUB_GRID_CNT  5              // 每个小格子可以分5步走完
#define  TEXT_LEFT_MARGIN  10

CMyImageUI::CMyImageUI(CPaintManagerUI *pManager) {
	m_pManager = pManager;
	m_nSubGridIndex = 0;
	m_hPen  = ::CreatePen(0, 1, RGB(0, 128, 64));
	m_hPen1 = ::CreatePen(0, 2, RGB(0, 128, 64));
	m_hPen2 = ::CreatePen(0, 1, RGB(0, 0, 255));

	m_dwTimeIndex = 0;
	m_dwTempDataLimit = 500;                // m_vTempData的个数限制在400个

	if (m_pManager) {
		m_pManager->SetTimer(this, MYIMAGE_TIMER_ID, 1000);
	}
}

CMyImageUI::~CMyImageUI() {
	ClearVector(m_vTempData);
}

bool CMyImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	RECT rect = this->GetPos();

	int  nCnt = 0;
	int  w = rect.right - rect.left;
	int  h = rect.bottom - rect.top;

	::SelectObject(hDC, m_hPen);
	int nSubGridSize = GRID_SIZE / SUB_GRID_CNT;

	nCnt = w / GRID_SIZE;
	for (int i = 0; i < nCnt+1; i++) {
		::MoveToEx(hDC, rect.left + GRID_SIZE * i - m_nSubGridIndex * nSubGridSize, rect.top, 0);
		::LineTo(hDC, rect.left + GRID_SIZE * i - m_nSubGridIndex * nSubGridSize, rect.bottom);
	}

	nCnt = h / GRID_SIZE;
	for (int i = 0; i < nCnt+1; i++) {
		::MoveToEx(hDC, rect.left, rect.top + GRID_SIZE * i, 0);
		::LineTo(hDC, rect.right, rect.top + GRID_SIZE * i);
	}

	// 写上摄氏度字样(34~42)
	::SetTextColor(hDC, RGB(68, 122, 161));
	int nSpan = nCnt / 8;    // 每个摄氏度占用几个小格子
	int nMiddle = nCnt / 2;  // 中间格子，显示38摄氏度

	::SelectObject(hDC, m_hPen1);
	::TextOut (hDC, rect.left + TEXT_LEFT_MARGIN, rect.top + GRID_SIZE * nMiddle, "38℃", 4);
	::MoveToEx(hDC, rect.left, rect.top + GRID_SIZE * nMiddle, 0);
	::LineTo(hDC, rect.right, rect.top + GRID_SIZE * nMiddle);

	CDuiString  str;
	for (int i = 0; i < 4; i++) {
		str.Format("%d℃", 38 + (i + 1));
		::TextOut(hDC, rect.left + TEXT_LEFT_MARGIN, rect.top + GRID_SIZE * ( nMiddle - nSpan*(i+1) ), (const char *)str, str.GetLength());
		::MoveToEx(hDC, rect.left, rect.top + GRID_SIZE * (nMiddle - nSpan*(i + 1)), 0);
		::LineTo(hDC, rect.right, rect.top + GRID_SIZE * (nMiddle - nSpan*(i + 1)));
	}

	for (int i = 0; i < 4; i++) {
		str.Format("%d℃", 38 - (i + 1));
		::TextOut(hDC, rect.left + TEXT_LEFT_MARGIN, rect.top + GRID_SIZE * (nMiddle + nSpan*(i + 1)), (const char *)str, str.GetLength());
		::MoveToEx(hDC, rect.left, rect.top + GRID_SIZE * (nMiddle + nSpan*(i + 1)), 0);
		::LineTo(hDC, rect.right, rect.top + GRID_SIZE * (nMiddle + nSpan*(i + 1)));
	}

	// 画点
	::SelectObject(hDC, m_hPen2);	
	int nSize = m_vTempData.size();
	for (int i = 0; i < nSize; i++) {
		TempData * pDateItem = m_vTempData[i];

		DWORD dwTimeDiff = m_dwTimeIndex - pDateItem->dwTimeIndex;
		int x = rect.right - dwTimeDiff * nSubGridSize;
		int y = (int)( (int)(3800 - pDateItem->dwTemperature) / 100.0 * GRID_SIZE * nSpan ) + (rect.top + GRID_SIZE * nMiddle);

		DrawTempPoint(x, y, hDC, 2);

		if (i > 0) {
			::LineTo(hDC, x, y);
		}
		::MoveToEx(hDC, x, y, 0);
	}

	return true;
	//return CControlUI::DoPaint(hDC, rcPaint, pStopControl);
}

void CMyImageUI::DoEvent(TEventUI& event) {
	if (event.Type == UIEVENT_TIMER && event.wParam == MYIMAGE_TIMER_ID) {
		m_nSubGridIndex++;
		if (m_nSubGridIndex >= SUB_GRID_CNT) {
			m_nSubGridIndex = 0;
		}
		m_dwTimeIndex++;
		Invalidate();
	}

	CControlUI::DoEvent(event);
}

void  CMyImageUI::AddTemp(DWORD dwTemp) {
	TempData * pData = new TempData;
	memset(pData, 0, sizeof(TempData));

	pData->dwTemperature = dwTemp;
	pData->dwTimeIndex = m_dwTimeIndex;

	m_vTempData.push_back(pData);

	// 如果超出大小，则删除第一个
	if (m_vTempData.size() > m_dwTempDataLimit) {
		m_vTempData.erase(m_vTempData.begin());
	}
}

void   CMyImageUI::DrawTempPoint(int x, int y, HDC hDc, int RADIUS /*= 6*/ ) {
	::Ellipse(hDc, x - RADIUS, y - RADIUS, x + RADIUS, y + RADIUS);

	int nTmp = (int)(0.707 * RADIUS);

	POINT points[2] = { { x - nTmp, y - nTmp },{ x + nTmp, y + nTmp } };
	::Polyline(hDc, points, 2);

	POINT points1[2] = { { x + nTmp, y - nTmp },{ x - nTmp, y + nTmp } };
	::Polyline(hDc, points1, 2);
}