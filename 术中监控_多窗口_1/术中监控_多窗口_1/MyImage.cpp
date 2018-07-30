#include <time.h>
#include "MyImage.h"
#include "business.h"
#include "LmnTelSvr.h"
#include "main.h"

CMyImageUI::CMyImageUI(DuiLib::CPaintManagerUI *pManager, CDuiFrameWnd * pMainWnd) :
	m_temperature_pen (Gdiplus::Color(0, 255, 0), 1.0), 
	m_temperature_brush(Gdiplus::Color(0, 255, 0)) 
{
	m_pManager = pManager;
	m_nState = STATE_GRIDS;
	m_bSetParentScrollPos = FALSE;
	m_nIndex = -1;

	m_temperature_pen.SetColor( Gdiplus::Color( g_skin[MYIMAGE_TEMP_THREAD_COLOR_INDEX] ) );
	m_temperature_brush.SetColor(Gdiplus::Color(g_skin[MYIMAGE_TEMP_DOT_COLOR_INDEX]));

	m_pMainWnd = pMainWnd;
	m_dwNextTempIndex = 0;
	m_dwCurTempIndex = -1;
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
	int  height = rect.bottom - rect.top; //pParent->GetHeight();
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
	int nVMargin = MIN_MYIMAGE_VMARGIN;
	//if ( m_nState == STATE_MAXIUM ) {
	//	nVMargin += 20;      // scroll bar的高度
	//}
	if (nVMargin * 2 > nReminder ) {
		int nSpared = (nVMargin * 2 - nReminder - 1) / nGridCount + 1;
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

		//if (it == m_vTempData.end() - 1) {
		//	JTelSvrPrint("my image draw temperature point, x = %d, y = %d", nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top);
		//}
		DrawTempPoint(graphics, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, hDC, nRadius );

		// 如果间隔足够大，画出time
		if (nX - nLastTimeTextLeft >= MIN_TEXT_INTERVAL) {
			struct tm tTmTime;
			localtime_s(&tTmTime, &pItem->tTime);
			strText.Format("%02d:%02d:%02d", tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);
			int nTextX = g_dwMyImageLeftBlank + rect.left + nX - 30;
			if ( nTextX > rectLeft.left ) {
				::TextOut(hDC, nTextX,
					middle + (nGridCount / 2) * nGridHeight + rect.top + g_dwMyImageTimeTextOffsetY,
					strText, strText.GetLength());
			}			
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

	// 画出类似注释的框
	//CGraphicsRoundRectPath  RoundRectPath;
	//RoundRectPath.AddRoundRect(rectLeft.right, rectLeft.top, 100, 100, 5, 5);
	//graphics.DrawPath(&m_temperature_pen, &RoundRectPath);

	return true;
}

void CMyImageUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_BUTTONDOWN)
	{
		//告诉UIManager这个消息需要处理
		m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
		return;
	}

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
	pTemp->dwIndex = m_dwNextTempIndex;
	m_dwNextTempIndex++;
	// skip -1 index
	if (m_dwNextTempIndex == -1) {
		m_dwNextTempIndex = 0;
	}
	pTemp->szRemark[0] = '\0';

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

	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();	

	if (m_nState == STATE_MAXIUM) {
		m_temperature_pen.SetWidth(3.0);
		pParent->EnableScrollBar(false, true);	
		if (g_dwSkinIndex == SKIN_BLACK) {
			pParent->SetAttribute("hscrollbarstyle", "button1normalimage=\"file='scrollbar.bmp' source='0,0,16,16' mask='#FFFF00FF'\" button1hotimage=\"file='scrollbar.bmp' source='18,0,34,16' mask='#FFFF00FF'\" button1pushedimage=\"file='scrollbar.bmp' source='36,0,52,16' mask='#FFFF00FF'\" button1disabledimage=\"file='scrollbar.bmp' source='54,0,70,16' mask='#FFFF00FF'\" button2normalimage=\"file='scrollbar.bmp' source='0,18,16,34' mask='#FFFF00FF'\" button2hotimage=\"file='scrollbar.bmp' source='18,18,34,34' mask='#FFFF00FF'\" button2pushedimage=\"file='scrollbar.bmp' source='36,18,52,34' mask='#FFFF00FF'\" button2disabledimage=\"file='scrollbar.bmp' source='54,18,70,34' mask='#FFFF00FF'\" thumbnormalimage=\"file='scrollbar.bmp' source='0,36,16,52' corner='2,2,2,2' mask='#FFFF00FF'\" thumbhotimage=\"file='scrollbar.bmp' source='18,36,34,52' corner='2,2,2,2' mask='#FFFF00FF'\" thumbpushedimage=\"file='scrollbar.bmp' source='36,36,52,52' corner='2,2,2,2' mask='#FFFF00FF'\" thumbdisabledimage=\"file='scrollbar.bmp' source='54,36,70,52' corner='2,2,2,2' mask='#FFFF00FF'\" railnormalimage=\"file='scrollbar.bmp' source='0,54,16,70' corner='2,2,2,2' mask='#FFFF00FF'\" railhotimage=\"file='scrollbar.bmp' source='18,54,34,70' corner='2,2,2,2' mask='#FFFF00FF'\" railpushedimage=\"file='scrollbar.bmp' source='36,54,52,70' corner='2,2,2,2' mask='#FFFF00FF'\" raildisabledimage=\"file='scrollbar.bmp' source='54,54,70,70' corner='2,2,2,2' mask='#FFFF00FF'\" bknormalimage=\"file='scrollbar.bmp' source='0,72,16,88' corner='2,2,2,2' mask='#FFFF00FF'\" bkhotimage=\"file='scrollbar.bmp' source='18,72,34,88' corner='2,2,2,2' mask='#FFFF00FF'\" bkpushedimage=\"file='scrollbar.bmp' source='36,72,52,88' corner='2,2,2,2' mask='#FFFF00FF'\" bkdisabledimage=\"file='scrollbar.bmp' source='54,72,70,88' corner='2,2,2,2' mask='#FFFF00FF'\"");
		}
		else {
			pParent->SetAttribute("hscrollbarstyle", "button1normalimage=\"file='scrollbar_1.bmp' source='0,0,16,16' mask='#FFFF00FF'\" button1hotimage=\"file='scrollbar_1.bmp' source='18,0,34,16' mask='#FFFF00FF'\" button1pushedimage=\"file='scrollbar_1.bmp' source='36,0,52,16' mask='#FFFF00FF'\" button1disabledimage=\"file='scrollbar_1.bmp' source='54,0,70,16' mask='#FFFF00FF'\" button2normalimage=\"file='scrollbar_1.bmp' source='0,18,16,34' mask='#FFFF00FF'\" button2hotimage=\"file='scrollbar_1.bmp' source='18,18,34,34' mask='#FFFF00FF'\" button2pushedimage=\"file='scrollbar_1.bmp' source='36,18,52,34' mask='#FFFF00FF'\" button2disabledimage=\"file='scrollbar_1.bmp' source='54,18,70,34' mask='#FFFF00FF'\" thumbnormalimage=\"file='scrollbar_1.bmp' source='0,36,16,52' corner='2,2,2,2' mask='#FFFF00FF'\" thumbhotimage=\"file='scrollbar_1.bmp' source='18,36,34,52' corner='2,2,2,2' mask='#FFFF00FF'\" thumbpushedimage=\"file='scrollbar_1.bmp' source='36,36,52,52' corner='2,2,2,2' mask='#FFFF00FF'\" thumbdisabledimage=\"file='scrollbar_1.bmp' source='54,36,70,52' corner='2,2,2,2' mask='#FFFF00FF'\" railnormalimage=\"file='scrollbar_1.bmp' source='0,54,16,70' corner='2,2,2,2' mask='#FFFF00FF'\" railhotimage=\"file='scrollbar_1.bmp' source='18,54,34,70' corner='2,2,2,2' mask='#FFFF00FF'\" railpushedimage=\"file='scrollbar_1.bmp' source='36,54,52,70' corner='2,2,2,2' mask='#FFFF00FF'\" raildisabledimage=\"file='scrollbar_1.bmp' source='54,54,70,70' corner='2,2,2,2' mask='#FFFF00FF'\" bknormalimage=\"file='scrollbar_1.bmp' source='0,72,16,88' corner='2,2,2,2' mask='#FFFF00FF'\" bkhotimage=\"file='scrollbar_1.bmp' source='18,72,34,88' corner='2,2,2,2' mask='#FFFF00FF'\" bkpushedimage=\"file='scrollbar_1.bmp' source='36,72,52,88' corner='2,2,2,2' mask='#FFFF00FF'\" bkdisabledimage=\"file='scrollbar_1.bmp' source='54,72,70,88' corner='2,2,2,2' mask='#FFFF00FF'\"");
		}
		CBusiness::GetInstance()->UpdateScrollAsyn(m_nIndex, 200);
	}
	else {
		m_temperature_pen.SetWidth(1.0);
		pParent->EnableScrollBar(false, false);		
	}
	
}

void  CMyImageUI::SetIndex(int nIndex) {
	m_nIndex = nIndex;
}

void CMyImageUI::OnChangeSkin() {
	m_temperature_pen.SetColor(Gdiplus::Color(g_skin[MYIMAGE_TEMP_THREAD_COLOR_INDEX]));
	m_temperature_brush.SetColor(Gdiplus::Color(g_skin[MYIMAGE_TEMP_DOT_COLOR_INDEX]));

	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	if (m_nState == STATE_MAXIUM) {
		if (g_dwSkinIndex == SKIN_BLACK) {
			pParent->SetAttribute("hscrollbarstyle", "button1normalimage=\"file='scrollbar.bmp' source='0,0,16,16' mask='#FFFF00FF'\" button1hotimage=\"file='scrollbar.bmp' source='18,0,34,16' mask='#FFFF00FF'\" button1pushedimage=\"file='scrollbar.bmp' source='36,0,52,16' mask='#FFFF00FF'\" button1disabledimage=\"file='scrollbar.bmp' source='54,0,70,16' mask='#FFFF00FF'\" button2normalimage=\"file='scrollbar.bmp' source='0,18,16,34' mask='#FFFF00FF'\" button2hotimage=\"file='scrollbar.bmp' source='18,18,34,34' mask='#FFFF00FF'\" button2pushedimage=\"file='scrollbar.bmp' source='36,18,52,34' mask='#FFFF00FF'\" button2disabledimage=\"file='scrollbar.bmp' source='54,18,70,34' mask='#FFFF00FF'\" thumbnormalimage=\"file='scrollbar.bmp' source='0,36,16,52' corner='2,2,2,2' mask='#FFFF00FF'\" thumbhotimage=\"file='scrollbar.bmp' source='18,36,34,52' corner='2,2,2,2' mask='#FFFF00FF'\" thumbpushedimage=\"file='scrollbar.bmp' source='36,36,52,52' corner='2,2,2,2' mask='#FFFF00FF'\" thumbdisabledimage=\"file='scrollbar.bmp' source='54,36,70,52' corner='2,2,2,2' mask='#FFFF00FF'\" railnormalimage=\"file='scrollbar.bmp' source='0,54,16,70' corner='2,2,2,2' mask='#FFFF00FF'\" railhotimage=\"file='scrollbar.bmp' source='18,54,34,70' corner='2,2,2,2' mask='#FFFF00FF'\" railpushedimage=\"file='scrollbar.bmp' source='36,54,52,70' corner='2,2,2,2' mask='#FFFF00FF'\" raildisabledimage=\"file='scrollbar.bmp' source='54,54,70,70' corner='2,2,2,2' mask='#FFFF00FF'\" bknormalimage=\"file='scrollbar.bmp' source='0,72,16,88' corner='2,2,2,2' mask='#FFFF00FF'\" bkhotimage=\"file='scrollbar.bmp' source='18,72,34,88' corner='2,2,2,2' mask='#FFFF00FF'\" bkpushedimage=\"file='scrollbar.bmp' source='36,72,52,88' corner='2,2,2,2' mask='#FFFF00FF'\" bkdisabledimage=\"file='scrollbar.bmp' source='54,72,70,88' corner='2,2,2,2' mask='#FFFF00FF'\"");
		}
		else {
			pParent->SetAttribute("hscrollbarstyle", "button1normalimage=\"file='scrollbar_1.bmp' source='0,0,16,16' mask='#FFFF00FF'\" button1hotimage=\"file='scrollbar_1.bmp' source='18,0,34,16' mask='#FFFF00FF'\" button1pushedimage=\"file='scrollbar_1.bmp' source='36,0,52,16' mask='#FFFF00FF'\" button1disabledimage=\"file='scrollbar_1.bmp' source='54,0,70,16' mask='#FFFF00FF'\" button2normalimage=\"file='scrollbar_1.bmp' source='0,18,16,34' mask='#FFFF00FF'\" button2hotimage=\"file='scrollbar_1.bmp' source='18,18,34,34' mask='#FFFF00FF'\" button2pushedimage=\"file='scrollbar_1.bmp' source='36,18,52,34' mask='#FFFF00FF'\" button2disabledimage=\"file='scrollbar_1.bmp' source='54,18,70,34' mask='#FFFF00FF'\" thumbnormalimage=\"file='scrollbar_1.bmp' source='0,36,16,52' corner='2,2,2,2' mask='#FFFF00FF'\" thumbhotimage=\"file='scrollbar_1.bmp' source='18,36,34,52' corner='2,2,2,2' mask='#FFFF00FF'\" thumbpushedimage=\"file='scrollbar_1.bmp' source='36,36,52,52' corner='2,2,2,2' mask='#FFFF00FF'\" thumbdisabledimage=\"file='scrollbar_1.bmp' source='54,36,70,52' corner='2,2,2,2' mask='#FFFF00FF'\" railnormalimage=\"file='scrollbar_1.bmp' source='0,54,16,70' corner='2,2,2,2' mask='#FFFF00FF'\" railhotimage=\"file='scrollbar_1.bmp' source='18,54,34,70' corner='2,2,2,2' mask='#FFFF00FF'\" railpushedimage=\"file='scrollbar_1.bmp' source='36,54,52,70' corner='2,2,2,2' mask='#FFFF00FF'\" raildisabledimage=\"file='scrollbar_1.bmp' source='54,54,70,70' corner='2,2,2,2' mask='#FFFF00FF'\" bknormalimage=\"file='scrollbar_1.bmp' source='0,72,16,88' corner='2,2,2,2' mask='#FFFF00FF'\" bkhotimage=\"file='scrollbar_1.bmp' source='18,72,34,88' corner='2,2,2,2' mask='#FFFF00FF'\" bkpushedimage=\"file='scrollbar_1.bmp' source='36,72,52,88' corner='2,2,2,2' mask='#FFFF00FF'\" bkdisabledimage=\"file='scrollbar_1.bmp' source='54,72,70,88' corner='2,2,2,2' mask='#FFFF00FF'\"");
		}
	}	
}

void  CMyImageUI::OnMyClick(const POINT * pPoint) {
	if (m_nState != STATE_MAXIUM) {
		return;
	}
	
	// 找到点击了哪个点
	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	RECT rect   = this->GetPos();
	int  width  = pParent->GetWidth();
	int  height = rect.bottom - rect.top;

	int nMinTemp    = g_dwMyImageMinTemp[m_nIndex];
	int nGridCount  = MAX_TEMPERATURE - nMinTemp;
	int nMiddleTemp = (MAX_TEMPERATURE + nMinTemp) / 2;

	int nGridHeight = height / nGridCount;
	int nReminder   = height % nGridCount;
	int nVMargin    = MIN_MYIMAGE_VMARGIN;
	if (nVMargin * 2 > nReminder) {
		int nSpared = (nVMargin * 2 - nReminder - 1) / nGridCount + 1;
		nGridHeight -= nSpared;
	}

	time_t  tFistTime = 0;
	if (m_vTempData.size() > 0) {
		TempData * pFist = m_vTempData[0];
		tFistTime = pFist->tTime;
	}

	int middle  = height / 2;
	int nRadius = RADIUS_SIZE_IN_MAXIUM;
	vector<TempData *>::iterator it;
	for (it = m_vTempData.begin(); it != m_vTempData.end(); it++) {
		TempData * pItem = *it;

		int nDiff = (int)(pItem->tTime - tFistTime);
		int nX = (int)(((double)nDiff / g_dwCollectInterval[m_nIndex]) * g_dwTimeUnitWidth);
		int nY = (int)((nMiddleTemp * 100.0 - (double)pItem->dwTemperature) / 100.0 * nGridHeight);

		int nX1 = nX + g_dwMyImageLeftBlank + rect.left;
		int nY1 = nY + middle + rect.top;

		if ( pPoint->x >= nX1 - RADIUS_SIZE_IN_MAXIUM && pPoint->x <= nX1 + RADIUS_SIZE_IN_MAXIUM
			 && pPoint->y >= nY1 - RADIUS_SIZE_IN_MAXIUM && pPoint->y <= nY1 + RADIUS_SIZE_IN_MAXIUM ) {
			// JTelSvrPrint("you clicked the point!");
			// 如果之前在编辑状态
			if ( m_pMainWnd->m_edRemark->IsVisible() ) {
				m_pMainWnd->OnEdtRemarkKillFocus();
			}

			m_dwCurTempIndex = pItem->dwIndex;
			RECT rectRemark;
			rectRemark.left = nX1 - EDT_REMARK_WIDTH / 2;
			rectRemark.top = nY1 + EDT_REMARK_Y_OFFSET;
			rectRemark.right = rectRemark.left + EDT_REMARK_WIDTH;
			rectRemark.bottom = rectRemark.top + EDT_REMARK_HEIGHT;
			m_pMainWnd->m_edRemark->SetPos(rectRemark);
			m_pMainWnd->m_edRemark->SetText(pItem->szRemark);
			m_pMainWnd->m_edRemark->SetVisible(true);
			m_pMainWnd->m_edRemark->SetFocus();
			break;
		}
	}
}

void  CMyImageUI::SetRemark(DuiLib::CDuiString & strRemark) {
	assert(m_dwCurTempIndex != -1);
	if (m_dwCurTempIndex == -1) {
		return;
	}

	vector<TempData *>::iterator it;
	for (it = m_vTempData.begin(); it != m_vTempData.end(); it++) {
		TempData * pItem = *it;
		if (pItem->dwIndex == m_dwCurTempIndex) {
			STRNCPY(pItem->szRemark, strRemark, sizeof(pItem->szRemark));
			break;
		}
	}
}