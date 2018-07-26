#include <time.h>
#include "MyImage.h"
#include "business.h"

CMyImageUI::CMyImageUI(DuiLib::CPaintManagerUI *pManager) : 
	m_temperature_pen (Gdiplus::Color(0, 255, 0), 1.0), 
	m_temperature_brush(Gdiplus::Color(0, 255, 0)) 
{
	m_pManager = pManager;
	m_nState = STATE_GRIDS;
	m_bSetParentScrollPos = FALSE;
	m_nIndex = -1;

	m_temperature_pen.SetColor( Gdiplus::Color( g_skin[MYIMAGE_TEMP_THREAD_COLOR_INDEX] ) );
	m_temperature_brush.SetColor(Gdiplus::Color(g_skin[MYIMAGE_TEMP_DOT_COLOR_INDEX]));
}

CMyImageUI::~CMyImageUI() {
	ClearVector(m_vTempData);
}

bool CMyImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	DuiLib::CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	// 父节点: vertical layout
	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	SIZE tParentScrollPos   = pParent->GetScrollPos();
	SIZE tParentScrollRange = pParent->GetScrollRange();

	// 只有最大化状态下才调整scroll
	if ( m_nState == STATE_MAXIUM) {
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

	int nRadius = RADIUS_SIZE_IN_MAXIUM;
	// 如果是多格子模式，模拟父窗口的滚动条已打开
	if ( m_nState == STATE_GRIDS) {
		int nMinWidth = GetMinWidth();
		if (nMinWidth > width) {
			tParentScrollPos.cx = nMinWidth - width;
			rect.left -= tParentScrollPos.cx;
		}
		nRadius = RADIUS_SIZE_IN_GRID;
	}


	/* 开始作图 */
	int nMinTemp     = g_dwMyImageMinTemp[m_nIndex];
	int nGridCount   = MAX_TEMPERATURE - nMinTemp;

	int nGridHeight = height / nGridCount;
	int nReminder   = height % nGridCount;
	if ( MIN_MYIMAGE_VMARGIN * 2 > nReminder ) {
		int nSpared = (MIN_MYIMAGE_VMARGIN * 2 - nReminder - 1) / nGridCount + 1;
		nGridHeight -= nSpared;
	}
	

	// 中间温度的位置
	int middle = height / 2;

	::SetTextColor(hDC, DWORD2RGB(g_skin[COMMON_TEXT_COLOR_INDEX]));
	::SelectObject(hDC, g_skin.GetGdiObject(COMMON_PEN_INDEX));

	RECT rectLeft;
	rectLeft.left   = rect.left + tParentScrollPos.cx;
	rectLeft.top    = rect.top;
	rectLeft.right  = rectLeft.left + g_dwMyImageLeftBlank;
	rectLeft.bottom = rect.bottom;

	int nFirstTop = middle - nGridHeight * (nGridCount / 2);
	int nFistTemperature = MAX_TEMPERATURE;

	// 画出刻度线(水平横线)
	int nVInterval = MIN_TEMP_V_INTERVAL;
	for (int i = 0; i < nGridCount + 1; i++) {
		if ( nVInterval >= MIN_TEMP_V_INTERVAL ) {
			::SelectObject(hDC, g_skin.GetGdiObject(BRIGHTER_PEN_INDEX));
			nVInterval = nGridHeight;
		}
		else {
			::SelectObject(hDC, g_skin.GetGdiObject(COMMON_PEN_INDEX));
			nVInterval += nGridHeight;
		}
		int  nTop = nFirstTop + i * nGridHeight;
		int  nTemperature = nFistTemperature - i;
		::MoveToEx(hDC, rectLeft.right, nTop + rect.top, 0);
		::LineTo(hDC, rect.right, nTop + rect.top);
	}
	::SelectObject(hDC, g_skin.GetGdiObject(COMMON_PEN_INDEX));
	
	time_t  tFistTime = 0;
	int     nLastTimeTextLeft = 0;
	if (m_vTempData.size() > 0) {
		TempData * pFist = m_vTempData[0];
		struct tm tTmTime;
		localtime_s(&tTmTime, &pFist->tTime);
		// 第一个点的时间
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

		DrawTempPoint(graphics, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, hDC, nRadius );

		// 如果间隔足够大，画出time
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
			graphics.DrawLine(&m_temperature_pen, pt.x, pt.y, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top);
			::MoveToEx(hDC, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, 0);
		}
	}

	// 画边框
	::SelectObject(hDC, g_skin.GetGdiObject(COMMON_PEN_INDEX));
	::FillRect(hDC, &rectLeft, (HBRUSH)g_skin.GetGdiObject(COMMON_BRUSH_INDEX));
	::Rectangle(hDC, rectLeft.left, rectLeft.top, rectLeft.right, rectLeft.bottom);
	::MoveToEx(hDC, rectLeft.left + width - 1, rectLeft.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.bottom);
	::MoveToEx(hDC, rectLeft.left, rectLeft.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.top);
	::MoveToEx(hDC, rectLeft.left, rectLeft.bottom -1, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.bottom -1);

	// 画刻度值
	nVInterval = MIN_TEMP_V_INTERVAL;
	for (int i = 0; i < nGridCount + 1; i++) {
		if (nVInterval >= MIN_TEMP_V_INTERVAL) {
			int  nTop = nFirstTop + i * nGridHeight;
			int  nTemperature = nFistTemperature - i;
			strText.Format("%d℃", nTemperature);
			::TextOut( hDC, rectLeft.right + g_dwMyImageTempTextOffsetX,
				       nTop + rect.top + g_dwMyImageTempTextOffsetY,
				       strText, strText.GetLength());
			nVInterval = nGridHeight;
		}
		else {
			nVInterval += nGridHeight;
		}
		
	}

	
	// 画出报警线
	::SelectObject(hDC, g_skin.GetGdiObject(LOW_TEMP_PEN_INDEX));
	int nY = (int)((nMiddleTemp * 100.0 - (double)g_dwLowTempAlarm[m_nIndex]) / 100.0 * nGridHeight);
	::MoveToEx(hDC, rectLeft.right, middle + nY + rect.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, middle + nY + rect.top);
	if (m_nState == STATE_MAXIUM) {
		strText = LOW_TEMP_ALARM_TEXT;
		::TextOut( hDC, rectLeft.right + LOW_TEMP_ALARM_TEXT_OFFSET_X, 
			       middle + nY + rect.top + LOW_TEMP_ALARM_TEXT_OFFSET_Y, 
			       strText, strText.GetLength() );
	}
	

	::SelectObject(hDC, g_skin.GetGdiObject(HIGH_TEMP_PEN_INDEX));
	nY = (int)((nMiddleTemp * 100.0 - (double)g_dwHighTempAlarm[m_nIndex]) / 100.0 * nGridHeight);
	::MoveToEx(hDC, rectLeft.right, middle + nY + rect.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, middle + nY + rect.top);
	if (m_nState == STATE_MAXIUM) {
		strText = HIGH_TEMP_ALARM_TEXT;
		::TextOut( hDC, rectLeft.right + HIGH_TEMP_ALARM_TEXT_OFFSET_X, 
			       middle + nY + rect.top + HIGH_TEMP_ALARM_TEXT_OFFSET_Y,
			       strText, strText.GetLength() );
	}

	return true;
}

void CMyImageUI::DoEvent(DuiLib::TEventUI& event) {
	DuiLib::CControlUI::DoEvent(event);
}

void   CMyImageUI::DrawTempPoint(Graphics & g, int x, int y, HDC hDc, int RADIUS /*= DEFAULT_POINT_RADIUS*/ ) {
	g.FillEllipse( &m_temperature_brush, x - RADIUS, y - RADIUS, 2 * RADIUS, 2 * RADIUS );
}

// 计算图像需要的宽度
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

	// 重绘
	MyInvalidate();

	assert(m_nIndex >= 0);
	// 如果报警开关打开
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

void CMyImageUI::OnChangeSkin() {
	m_temperature_pen.SetColor(Gdiplus::Color(g_skin[MYIMAGE_TEMP_THREAD_COLOR_INDEX]));
	m_temperature_brush.SetColor(Gdiplus::Color(g_skin[MYIMAGE_TEMP_DOT_COLOR_INDEX]));
}