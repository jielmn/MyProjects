#include <time.h>
#include "MyImage.h"
#include "business.h"

CMyImageUI::CMyImageUI(DuiLib::CPaintManagerUI *pManager) {
	m_pManager = pManager;
	m_nState = MYIMAGE_STATE_GRID;
	m_bSetParentScrollPos = FALSE;
	m_nIndex = -1;
}

CMyImageUI::~CMyImageUI() {
	ClearVector(m_vTempData);
}

bool CMyImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	DuiLib::CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	// ���ڵ�: vertical layout
	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	SIZE tParentScrollPos   = pParent->GetScrollPos();
	SIZE tParentScrollRange = pParent->GetScrollRange();

	// ֻ�����״̬�²ŵ���scroll
	if ( m_nState == MYIMAGE_STATE_MAXIUM ) {
		if (tParentScrollPos.cx != tParentScrollRange.cx) {
			if (m_bSetParentScrollPos)
			{
				CBusiness::GetInstance()->UpdateScrollAsyn(m_nIndex);
				m_bSetParentScrollPos = FALSE;
			}
		}
	}
	
	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	RECT rect   = this->GetPos();
	int  width  = pParent->GetWidth();
	int  height = pParent->GetHeight();
	DuiLib::CDuiString strText;


	/* ��ʼ��ͼ */
	int nMinTemp     = g_dwMyImageMinTemp[m_nIndex];
	int nGridCount   = MAX_TEMPERATURE - nMinTemp;

	int nGridHeight = height / nGridCount;
	int nReminder   = height % nGridCount;
	if ( MIN_MYIMAGE_VMARGIN * 2 > nReminder ) {
		int nSpared = (MIN_MYIMAGE_VMARGIN * 2 - nReminder - 1) / nGridCount + 1;
		nGridHeight -= nSpared;
	}
	

	// �м��¶ȵ�λ��
	int middle = height / 2;

	::SetTextColor(hDC, DWORD2RGB(g_skin[COMMON_TEXT_COLOR_INDEX]));
	::SelectObject(hDC, g_skin.GetPen(COMMON_THREAD_INDEX));

	RECT rectLeft;
	rectLeft.left   = rect.left + tParentScrollPos.cx;
	rectLeft.top    = rect.top;
	rectLeft.right  = rectLeft.left + g_dwMyImageLeftBlank;
	rectLeft.bottom = rect.bottom;

	int nFirstTop = middle - nGridHeight * (nGridCount / 2);
	int nFistTemperature = MAX_TEMPERATURE;

	// �����̶���(ˮƽ����)
	for (int i = 0; i < nGridCount + 1; i++) {
		int  nTop = nFirstTop + i * nGridHeight;
		int  nTemperature = nFistTemperature - i;
		::MoveToEx(hDC, rectLeft.right, nTop + rect.top, 0);
		::LineTo(hDC, rect.right, nTop + rect.top);
	}

	
	time_t  tFistTime = 0;
	int     nLastTimeTextLeft = 0;
	if (m_vTempData.size() > 0) {
		TempData * pFist = m_vTempData[0];
		struct tm tTmTime;
		localtime_s(&tTmTime, &pFist->tTime);
		// ��һ�����ʱ��
		strText.Format("%02d:%02d:%02d", tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);
		::TextOut( hDC, g_dwMyImageLeftBlank + rect.left + g_dwMyImageTimeTextOffsetX, 
			       middle + ( nGridCount / 2) * nGridHeight + rect.top + g_dwMyImageTimeTextOffsetY, 
			       strText, strText.GetLength() );
		tFistTime = pFist->tTime;
		nLastTimeTextLeft = 0;
	}

	assert(g_dwCollectInterval[m_nIndex] > 0);
	int nMiddleTemp = (MAX_TEMPERATURE + nMinTemp) / 2;

	vector<TempData *>::iterator it;
	for (it = m_vTempData.begin(); it != m_vTempData.end(); it++) {
		TempData * pItem = *it;

		int nDiff = (int)(pItem->tTime - tFistTime);
		int nX    = (int)(((double)nDiff / g_dwCollectInterval[m_nIndex]) * g_dwTimeUnitWidth);
		int nY    = (int)((nMiddleTemp * 100.0 - (double)pItem->dwTemperature) / 100.0 * nGridHeight);

		DrawTempPoint(graphics, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, hDC );

		// �������㹻�󣬻���time
		if (nX - nLastTimeTextLeft >= MIN_TEXT_INTERVAL) {
			struct tm tTmTime;
			localtime_s(&tTmTime, &pItem->tTime);
			strText.Format("%02d:%02d:%02d", tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);
			::TextOut( hDC, g_dwMyImageLeftBlank + rect.left + nX - 30, 
				       middle + (nGridCount / 2) * nGridHeight + rect.top + g_dwMyImageTimeTextOffsetY, 
				       strText, strText.GetLength() );
			nLastTimeTextLeft = nX;
		}

		if (it == m_vTempData.begin()) {
			::MoveToEx(hDC, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, 0);
		}
		else {
			POINT pt;
			::GetCurrentPositionEx(hDC, &pt);
			graphics.DrawLine( &g_skin.GetPenEx(TEMPERATURE_THREAD_INDEX), pt.x, pt.y, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top);
			::MoveToEx(hDC, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, 0);
		}
	}

	/*
	::SelectObject(hDC, m_hPen);
	::FillRect(hDC, &rectLeft, m_hBrush);
	::Rectangle(hDC, rectLeft.left, rectLeft.top, rectLeft.right, rectLeft.bottom);
	::MoveToEx(hDC, rectLeft.left + width - 1, rectLeft.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.bottom);

	for (int i = 0; i < m_nGridCount + 1; i++) {
		int  nTop = nFirstTop + i * m_nGridSize;
		int  nTemperature = nFistTemperature - i;

		// ���̶�ֵ
		strText.Format("%d��", nTemperature);
		::TextOut(hDC, rectLeft.right + m_nTextOffsetX, nTop + rect.top + m_nTextOffsetY, strText, strText.GetLength());
	}

	// ����������
	::SelectObject(hDC, m_hPen1);
	int nY = (int)((nMiddleTemp * 100.0 - (double)g_dwLowTempAlarm[m_nChartIndex]) / 100.0 * m_nGridSize);
	::MoveToEx(hDC, rectLeft.right, middle + nY + rect.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, middle + nY + rect.top);
	strText = "���±���";
	::TextOut(hDC, rectLeft.right + 5, middle + nY + rect.top + 5, strText, strText.GetLength());

	::SelectObject(hDC, m_hPen2);
	nY = (int)((nMiddleTemp * 100.0 - (double)g_dwHighTempAlarm[m_nChartIndex]) / 100.0 * m_nGridSize);
	::MoveToEx(hDC, rectLeft.right, middle + nY + rect.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, middle + nY + rect.top);
	strText = "���±���";
	::TextOut(hDC, rectLeft.right + 5, middle + nY + rect.top - 20, strText, strText.GetLength());
	*/

	return true;
}

void CMyImageUI::DoEvent(DuiLib::TEventUI& event) {
	DuiLib::CControlUI::DoEvent(event);
}

void   CMyImageUI::DrawTempPoint(Graphics & g, int x, int y, HDC hDc, int RADIUS /*= DEFAULT_POINT_RADIUS*/ ) {
	g.FillEllipse( &g_skin.GetBrushEx(TEMPERATURE_BRUSH_INDEX), x - RADIUS, y - RADIUS, 2 * RADIUS, 2 * RADIUS );
}

// ����ͼ����Ҫ�Ŀ��
int   CMyImageUI::CalcMinWidth() {
	DWORD dwCnt = m_vTempData.size();
	if (dwCnt <= 1) {
		return 0;
	}

	assert(m_nIndex >= 0);
	DWORD  dwInterval        = g_dwCollectInterval[m_nIndex];
	DWORD  dwUnitWidth       = g_dwTimeUnitWidth;
	DWORD  dwLeftColumnWidth = g_dwMyImageLeftBlank;

	int nWidth = (int)((double)(m_vTempData[dwCnt - 1]->tTime - m_vTempData[0]->tTime) / dwInterval * dwUnitWidth);
	return nWidth + dwLeftColumnWidth + g_dwMyImageRightBlank;
}

void  CMyImageUI::AddTemp(DWORD dwTemp) {
	time_t now = time(0);

	TempData * pTemp = new TempData;
	pTemp->dwTemperature = dwTemp;
	pTemp->tTime = now;

	if ( m_vTempData.size() >= g_dwMaxPointsCount ) {
		vector<TempData *>::iterator it = m_vTempData.begin();
		TempData * pData = *it;
		m_vTempData.erase(it);
		delete pData;
	}
	m_vTempData.push_back(pTemp);

	// �ػ�
	MyInvalidate();

	assert(m_nIndex >= 0);
	// ����������ش�
	if ( !g_bAlarmOff ) {
		if (dwTemp < g_dwLowTempAlarm[m_nIndex] || dwTemp > g_dwHighTempAlarm[m_nIndex]) {
			CBusiness::GetInstance()->AlarmAsyn(g_szAlarmFilePath);
		}
	}	
}

void  CMyImageUI::MyInvalidate() {
	int nMinWidth = CalcMinWidth();
	this->SetMinWidth(nMinWidth);
	Invalidate();
	m_bSetParentScrollPos = TRUE;
}

void  CMyImageUI::SetState(int nNewState) {
	m_nState = nNewState;
}

void  CMyImageUI::SetIndex(int nIndex) {
	m_nIndex = nIndex;
}