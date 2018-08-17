#include <time.h>
#include "MyImage.h"
#include "business.h"
#include "LmnTelSvr.h"
#include "main.h"
#include "LmnExcel.h"

CMyImageUI::CMyImageUI(DuiLib::CPaintManagerUI *pManager, CDuiFrameWnd * pMainWnd) :
	m_temperature_pen (Gdiplus::Color(0, 255, 0), 1.0), 
	m_temperature_brush(Gdiplus::Color(0, 255, 0)) ,
	m_remark_pen(Gdiplus::Color(0, 255, 0), 3.0),
	m_remark_brush(Gdiplus::Color(0, 255, 0))
{
	m_pManager = pManager;
	m_nState = STATE_GRIDS;
	m_bSetParentScrollPos = FALSE;
	m_nIndex = -1;

	m_temperature_pen.SetColor( Gdiplus::Color( g_skin[MYIMAGE_TEMP_THREAD_COLOR_INDEX] ) );
	m_temperature_brush.SetColor(Gdiplus::Color(g_skin[MYIMAGE_TEMP_DOT_COLOR_INDEX]));
	m_remark_pen.SetColor(Gdiplus::Color(g_skin[MYIMAGE_REMARK_THREAD_COLOR_INDEX]));
	m_remark_brush.SetColor(Gdiplus::Color(g_skin[MYIMAGE_REMARK_BRUSH_INDEX]));

	m_pMainWnd = pMainWnd;
	m_dwNextTempIndex = 0;
	m_dwCurTempIndex = -1;
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
	// ����Ƕ����ģʽ��ģ�⸸���ڵĹ������Ѵ�
	if ( m_nState == STATE_GRIDS) {
		int nMinWidth = GetMinWidth();
		if (nMinWidth > width) {
			tParentScrollPos.cx = nMinWidth - width;
			rect.left -= tParentScrollPos.cx;
		}
		nRadius = RADIUS_SIZE_IN_GRID;
	}


	/* ��ʼ��ͼ */
	int nMinTemp     = g_dwMyImageMinTemp[m_nIndex];
	int nGridCount   = MAX_TEMPERATURE - nMinTemp;

	int nGridHeight = height / nGridCount;
	int nReminder   = height % nGridCount;
	int nVMargin = MIN_MYIMAGE_VMARGIN;
	//if ( m_nState == STATE_MAXIUM ) {
	//	nVMargin += 20;      // scroll bar�ĸ߶�
	//}
	if (nVMargin * 2 > nReminder ) {
		int nSpared = (nVMargin * 2 - nReminder - 1) / nGridCount + 1;
		nGridHeight -= nSpared;
	}
	

	// �м��¶ȵ�λ��
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

	// �����̶���(ˮƽ����)
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

		//if (it == m_vTempData.end() - 1) {
		//	JTelSvrPrint("my image draw temperature point, x = %d, y = %d", nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top);
		//}
		DrawTempPoint(graphics, nX + g_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, hDC, nRadius );

		// �������㹻�󣬻���time
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

	// ���߿�
	::SelectObject(hDC, g_skin.GetGdiObject(COMMON_PEN_INDEX));
	::FillRect(hDC, &rectLeft, (HBRUSH)g_skin.GetGdiObject(COMMON_BRUSH_INDEX));
	::Rectangle(hDC, rectLeft.left, rectLeft.top, rectLeft.right, rectLeft.bottom);
	::MoveToEx(hDC, rectLeft.left + width - 1, rectLeft.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.bottom);
	::MoveToEx(hDC, rectLeft.left, rectLeft.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.top);
	::MoveToEx(hDC, rectLeft.left, rectLeft.bottom -1, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.bottom -1);

	// ���̶�ֵ
	nVInterval = MIN_TEMP_V_INTERVAL;
	for (int i = 0; i < nGridCount + 1; i++) {
		if (nVInterval >= MIN_TEMP_V_INTERVAL) {
			int  nTop = nFirstTop + i * nGridHeight;
			int  nTemperature = nFistTemperature - i;
			strText.Format("%d��", nTemperature);
			::TextOut( hDC, rectLeft.right + g_dwMyImageTempTextOffsetX,
				       nTop + rect.top + g_dwMyImageTempTextOffsetY,
				       strText, strText.GetLength());
			nVInterval = nGridHeight;
		}
		else {
			nVInterval += nGridHeight;
		}
		
	}

	
	// ����������
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

	// ��������ע�͵Ŀ�
	::SetBkMode(hDC, TRANSPARENT);
	if (m_nState == STATE_MAXIUM) {
		for (it = m_vTempData.begin(); it != m_vTempData.end(); it++) {
			TempData * pItem = *it;

			if (pItem->szRemark[0] != '\0') {
				int nDiff = (int)(pItem->tTime - tFistTime);
				int nX = (int)(((double)nDiff / g_dwCollectInterval[m_nIndex]) * g_dwTimeUnitWidth);
				int nY = (int)((nMiddleTemp * 100.0 - (double)pItem->dwTemperature) / 100.0 * nGridHeight);

				int nX1 = nX + g_dwMyImageLeftBlank + rect.left;
				int nY1 = nY + middle + rect.top;

				CGraphicsRoundRectPath  RoundRectPath;
				RoundRectPath.AddRoundRect(nX1 - EDT_REMARK_WIDTH / 2, nY1 + EDT_REMARK_Y_OFFSET,
					EDT_REMARK_WIDTH, EDT_REMARK_HEIGHT, 5, 5);
				graphics.DrawPath(&m_remark_pen, &RoundRectPath);
				graphics.FillPath(&m_remark_brush, &RoundRectPath);

				strText = pItem->szRemark;
				RECT rectRemark;
				rectRemark.left = nX1 - EDT_REMARK_WIDTH / 2;
				rectRemark.top = nY1 + EDT_REMARK_Y_OFFSET;
				rectRemark.right = rectRemark.left + EDT_REMARK_WIDTH;
				rectRemark.bottom = rectRemark.top + EDT_REMARK_HEIGHT;
				::DrawText(hDC, strText, strText.GetLength(), &rectRemark, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
			}
		}
	}
	
	//CGraphicsRoundRectPath  RoundRectPath;
	//RoundRectPath.AddRoundRect(rectLeft.right, rectLeft.top, 100, 100, 5, 5);
	//graphics.DrawPath(&m_temperature_pen, &RoundRectPath);

	return true;
}

void CMyImageUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_BUTTONDOWN)
	{
		//����UIManager�����Ϣ��Ҫ����
		m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
		return;
	}

	DuiLib::CControlUI::DoEvent(event);
}

void   CMyImageUI::DrawTempPoint(Graphics & g, int x, int y, HDC hDc, int RADIUS /*= DEFAULT_POINT_RADIUS*/ ) {	
	g.FillEllipse( &m_temperature_brush, x - RADIUS, y - RADIUS, 2 * RADIUS, 2 * RADIUS );
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

	// �ػ�
	MyInvalidate();
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
	m_remark_pen.SetColor(Gdiplus::Color(g_skin[MYIMAGE_REMARK_THREAD_COLOR_INDEX]));
	m_remark_brush.SetColor(Gdiplus::Color(g_skin[MYIMAGE_REMARK_BRUSH_INDEX]));

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
	
	// �ҵ�������ĸ���
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
			// ���֮ǰ�ڱ༭״̬
			if ( m_pMainWnd->m_edRemark->IsVisible() ) {
				m_pMainWnd->OnEdtRemarkKillFocus();
			}

			m_dwCurTempIndex = pItem->dwIndex;
			RECT rectRemark;
			rectRemark.left = nX1 - EDT_REMARK_WIDTH / 2 + 1;
			rectRemark.top = nY1 + EDT_REMARK_Y_OFFSET + 3;
			rectRemark.right = rectRemark.left + EDT_REMARK_WIDTH - 2;
			rectRemark.bottom = rectRemark.top + EDT_REMARK_HEIGHT - 6;
			m_pMainWnd->m_edRemark->SetPos(rectRemark);
			m_pMainWnd->m_edRemark->SetText(pItem->szRemark);
			m_pMainWnd->m_edRemark->SetVisible(true);
			m_pMainWnd->m_edRemark->SetFocus();
			g_bAutoScroll = FALSE;
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

void  CMyImageUI::SaveExcel(const char * szBed, const char * szPatientName) {
	CDuiString strText;

	if ( !CExcel::IfExcelInstalled() ) {
		::MessageBox(m_pMainWnd->GetHWND(), "û�м�⵽ϵͳ��װ��excel", "����excel", 0);
		return;
	}

	if ( m_vTempData.size() == 0 ) {
		strText.Format("��%d������û���¶����ݣ���������excel", m_nIndex + 1);
		::MessageBox( m_pMainWnd->GetHWND(), strText, "����excel", 0 );
		return;
	}

	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���  

	char szTime[256];
	time_t now = time(0);
	Date2String_1(szTime, sizeof(szTime), &now);
	SNPRINTF(strFilename, sizeof(strFilename), "%s_%s", szPatientName, szTime);

	ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С  
	ofn.hwndOwner = m_pMainWnd->GetHWND();//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������  
	ofn.lpstrFilter = TEXT("Excel Flie(*.xls)\0*.xls\0Excel Flie(*.xlsx)\0*.xlsx\0\0");//���ù���  
	ofn.nFilterIndex = 1;//����������  
	ofn.lpstrFile = strFilename;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL  
	ofn.nMaxFile = sizeof(strFilename);//����������  
	ofn.lpstrInitialDir = CPaintManagerUI::GetInstancePath();
	ofn.lpstrTitle = TEXT("��ѡ��һ���ļ�");//ʹ��ϵͳĬ�ϱ������ռ���  
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��  
	if (!GetSaveFileName(&ofn))
	{
		return;
	}

	CExcel  excel;
	excel.WriteGrid(0, 0, "����");
	excel.WriteGrid(0, 1, szBed);
	excel.WriteGrid(0, 3, "����");
	excel.WriteGrid(0, 4, szPatientName);

	excel.WriteGrid(2, 0, "ʱ��");
	excel.WriteGrid(2, 1, "����");
	excel.WriteGrid(2, 2, "ע��");

	std::vector<const char *> vValues;
	vector<TempData *>::iterator it;
	for (it = m_vTempData.begin(); it != m_vTempData.end(); it++) {
		TempData * pItem = *it;	

		char * p = new char[32];
		Time2String(p, 32, &pItem->tTime);
		vValues.push_back(p);

		p = new char[8];
		SNPRINTF(p, 8, "%.2f", pItem->dwTemperature / 100.0);
		vValues.push_back(p);

		if (pItem->szRemark[0] != 0) {
			p = new char[MAX_REMARK_LENGTH];
			STRNCPY(p, pItem->szRemark, MAX_REMARK_LENGTH);
			vValues.push_back(p);
		}
		else {
			vValues.push_back(0);
		}
	}

	excel.WriteRange(3, 0, m_vTempData.size() + 3 - 1, 2, vValues);
	int ret = excel.SaveAs(strFilename);
	excel.Quit();


	std::vector<const char *>::iterator  ix;
	for (ix = vValues.begin(); ix != vValues.end(); ++ix) {
		if (*ix != 0) {
			delete[] * ix;
		}
	}

	if (ret != 0) {
		strText.Format("��%d�����񱣴�excel����ʧ�ܡ���ȷ��excel�ļ�û�д�", m_nIndex + 1);
		::MessageBox(m_pMainWnd->GetHWND(), strText, "����excel", 0);
	}
}





CAlarmImageUI::CAlarmImageUI(DuiLib::CPaintManagerUI *pManager) {
	m_pManager = pManager;
	m_bSetBkImage = FALSE;
	m_nBkImageIndex = -1;
}

CAlarmImageUI::~CAlarmImageUI() {

}

bool CAlarmImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	return CControlUI::DoPaint(hDC, rcPaint, pStopControl);
}

void CAlarmImageUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == DuiLib::UIEVENT_TIMER && event.wParam == MYIMAGE_TIMER_ID) {
		if (m_bSetBkImage) {
			if (0 == m_nBkImageIndex) {
				if (m_nState == STATE_GRIDS) {
					this->SetBkImage("alarm_high_temp_2.png");
				}
				else {
					this->SetBkImage("alarm_high_temp_1.png"); 
				}
			}
			else if (1 == m_nBkImageIndex) {
				if (m_nState == STATE_GRIDS) {
					this->SetBkImage("alarm_low_temp_2.png");
				}
				else {
					this->SetBkImage("alarm_low_temp_1.png");
				}
			}
			else if (2 == m_nBkImageIndex) {
				if (m_nState == STATE_GRIDS) {
					this->SetBkImage("alarm_fail_2.png");
				}
				else {
					this->SetBkImage("alarm_fail_1.png");
				}
			}
			else {
				assert(0);
				this->SetBkImage("");
			}
		}
		else {
			this->SetBkImage("");
		}

		// �������FailureAlarm
		if ( 2 != m_nBkImageIndex ) {
			m_bSetBkImage = !m_bSetBkImage;
		}
		
	}

	CControlUI::DoEvent(event);
}

void   CAlarmImageUI::HighTempAlarm() {
	if (0 != m_nBkImageIndex) {
		m_nBkImageIndex = 0;
		m_bSetBkImage = TRUE;
		m_pManager->SetTimer(this, MYIMAGE_TIMER_ID, 500);
	}	
}

void   CAlarmImageUI::LowTempAlarm() {
	if (1 != m_nBkImageIndex) {
		m_nBkImageIndex = 1;
		m_bSetBkImage = TRUE;
		m_pManager->SetTimer(this, MYIMAGE_TIMER_ID, 500);
	}
}

void  CAlarmImageUI::FailureAlarm() {
	if (2 != m_nBkImageIndex) {
		m_nBkImageIndex = 2;
		m_bSetBkImage = TRUE;
		m_pManager->SetTimer(this, MYIMAGE_TIMER_ID, 500);
	}
}

void   CAlarmImageUI::StopAlarm() {
	if (-1 != m_nBkImageIndex) {
		m_nBkImageIndex = -1;
		this->SetBkImage("");
		m_pManager->KillTimer(this, MYIMAGE_TIMER_ID);
	}
}

void  CAlarmImageUI::SetState(int nNewState) {
	m_nState = nNewState;
}