#include <time.h>
#include "MyImage.h"
#include "skin.h"
#include "business.h"
#include "LmnExcel.h"

CMyImageUI::CMyImageUI(E_TYPE e) :	m_remark_pen(Gdiplus::Color(0x803D5E49), 3.0),
							m_remark_brush(Gdiplus::Color(0x803D5E49)), m_type(e) {
	m_hCommonThreadPen   = ::CreatePen(PS_SOLID, 1, g_data.m_skin.GetRgb(CMySkin::COMMON_PEN) );
	m_hBrighterThreadPen = ::CreatePen(PS_SOLID, 1, g_data.m_skin.GetRgb(CMySkin::BRIGHT_PEN));
	m_hCommonBrush       = ::CreateSolidBrush(g_data.m_skin.GetRgb(CMySkin::COMMON_BRUSH));
	m_hLowTempAlarmPen   = ::CreatePen(PS_DASH, 1, g_data.m_skin.GetRgb(CMySkin::LOW_ALARM_PEN));
	m_hHighTempAlarmPen  = ::CreatePen(PS_DASH, 1, g_data.m_skin.GetRgb(CMySkin::HIGH_ALARM_PEN));
	
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		m_temperature_pen[i] = new Pen(Gdiplus::Color(g_data.m_argb[i]), 3.0);
		m_temperature_brush[i] = new SolidBrush(Gdiplus::Color(g_data.m_argb[i]));
	}

	//m_dwSelectedReaderIndex = 0;	
	OnReaderSelected(0);

	m_dwNextTempIndex = 0;

	CBusiness * pBusiness = CBusiness::GetInstance();	
	m_sigUpdateScroll.connect(pBusiness, &CBusiness::OnUpdateScroll);
	m_sigAlarm.connect(pBusiness, &CBusiness::OnAlarm);

	m_dwCurTempIndex = -1;
}

CMyImageUI::~CMyImageUI() {
	DeleteObject(m_hCommonThreadPen );
	DeleteObject(m_hBrighterThreadPen);
	DeleteObject(m_hCommonBrush);
	DeleteObject(m_hLowTempAlarmPen);
	DeleteObject(m_hHighTempAlarmPen);
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		delete m_temperature_pen[i];
		delete m_temperature_brush[i];
		ClearVector(m_vTempData[i]);
	}
}

bool CMyImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	DuiLib::CControlUI::DoPaint(hDC, rcPaint, pStopControl);
	DuiLib::CDuiString strText;

	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	SIZE tParentScrollPos = pParent->GetScrollPos();
	SIZE tParentScrollRange = pParent->GetScrollRange();
	RECT rect   = this->GetPos();
	int  width  = pParent->GetWidth();
	int  height = rect.bottom - rect.top;
	DWORD  dwIndex = this->GetTag();

	// 只有最大化状态下才调整scroll
	if (m_type == TYPE_MAX) {
		if (tParentScrollPos.cx != tParentScrollRange.cx) {
			if (m_bSetParentScrollPos)
			{
				m_sigUpdateScroll.emit(dwIndex);
				m_bSetParentScrollPos = FALSE;
			}
		}
	}

	int nRadius = RADIUS_SIZE_IN_MAXIUM;
	// 如果是多格子模式，模拟父窗口的滚动条已打开
	if (m_type == TYPE_GRID) {
		int nMinWidth = GetMinWidth();
		if (nMinWidth > width) {
			tParentScrollPos.cx = nMinWidth - width;
			rect.left -= tParentScrollPos.cx;
		}
		nRadius = RADIUS_SIZE_IN_GRID;
	}

	POINT cursor_point;
	GetCursorPos(&cursor_point);
	::ScreenToClient(g_data.m_hWnd, &cursor_point );

	/* 开始作图 */
	int nMinTemp = GetMinTemp(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwMinTemp);
	//int nGridCount = MAX_TEMPERATURE - nMinTemp;
	int nMaxTemp = GetMaxTemp(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwMaxTemp);
	int nGridCount = nMaxTemp - nMinTemp;

	int nGridHeight = height / nGridCount;
	int nReminder = height % nGridCount;
	int nVMargin = MIN_MYIMAGE_VMARGIN;

	if (nVMargin * 2 > nReminder) {
		int nSpared = (nVMargin * 2 - nReminder - 1) / nGridCount + 1;
		nGridHeight -= nSpared;
	}

	// 中间温度的位置
	int middle = height / 2;

	::SetTextColor(hDC, g_data.m_skin.GetRgb(CMySkin::COMMON_TEXT_COLOR));
	::SelectObject(hDC, m_hCommonThreadPen);

	RECT rectLeft;
	rectLeft.left = rect.left + tParentScrollPos.cx;
	rectLeft.top = rect.top;
	rectLeft.right = rectLeft.left + MYIMAGE_LEFT_BLANK;
	rectLeft.bottom = rect.bottom;

	int nFirstTop = middle - nGridHeight * (nGridCount / 2);
	//int nFistTemperature = MAX_TEMPERATURE;
	int nFistTemperature = nMaxTemp;
	DWORD  dwCollectInterval = GetCollectInterval(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwCollectInterval);

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
	::SelectObject(hDC, m_hCommonThreadPen);

	/* 画温度曲线 */
	time_t  tFirstTime = GetFirstTime();
	time_t  tLastTime = GetLastTime();
	int nMiddleTemp = (nMaxTemp + nMinTemp) / 2;

	vector<TempData *>::iterator it;
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];

		Gdiplus::Point * points = 0;
		// 如果画曲线
		if ( g_data.m_bCurve ) {
			points = new Gdiplus::Point[vTempData.size()];
		}
		int j = 0;
		for (it = vTempData.begin(),j = 0; it != vTempData.end(); it++, j++) {
			TempData * pItem = *it;

			int nDiff = (int)(pItem->tTime - tFirstTime);
			int nX    = (int)(((double)nDiff / dwCollectInterval )
				        * g_data.m_dwCollectIntervalWidth );
			int nY    = (int)((nMiddleTemp * 100.0 - (double)pItem->dwTemperature) / 100.0 * nGridHeight);

			if (g_data.m_bCurve) {
				points[j].X = nX + MYIMAGE_LEFT_BLANK + rect.left;
				points[j].Y = nY + middle + rect.top;
			}
			DrawTempPoint(i, graphics, nX + MYIMAGE_LEFT_BLANK + rect.left, nY + middle + rect.top, hDC, nRadius );

			if ( !g_data.m_bCurve ) {
				if (it == vTempData.begin()) {
					::MoveToEx(hDC, nX + MYIMAGE_LEFT_BLANK + rect.left, nY + middle + rect.top, 0);
				}
				else {
					POINT pt;
					::GetCurrentPositionEx(hDC, &pt);
					graphics.DrawLine(m_temperature_pen[i], pt.x, pt.y, nX + MYIMAGE_LEFT_BLANK + rect.left, nY + middle + rect.top);
					::MoveToEx(hDC, nX + MYIMAGE_LEFT_BLANK + rect.left, nY + middle + rect.top, 0);
				}
			}			
		}

		if (g_data.m_bCurve) {
			graphics.DrawCurve(m_temperature_pen[i], points, vTempData.size());
			delete[] points;
		}
	}
	
	// 从第一个10秒整数，画时间
	if (tFirstTime >= 0) {
		int  remainder = tFirstTime % 10;
		time_t  tFirstTime_1 = tFirstTime;
		if (remainder > 0) {
			tFirstTime_1 += 10 - remainder;
		}

		int nLastX = -1;
		time_t t = 0;
		for (t = tFirstTime_1; t < tLastTime; t += dwCollectInterval ) {
			int nDiff = (int)(t - tFirstTime);
			int nX = (int)(((double)nDiff / dwCollectInterval ) * g_data.m_dwCollectIntervalWidth);

			if (nLastX == -1 || nX - nLastX >= 300) {
				int nTextX = MYIMAGE_LEFT_BLANK + rect.left + nX + 1;
				if (nTextX > rectLeft.left) {
					struct tm tTmTime;
					localtime_s(&tTmTime, &t);

					strText.Format("  %02d:%02d:%02d", tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);					
					::TextOut(hDC, nTextX,
						middle + (nGridCount / 2) * nGridHeight + rect.top + 5,
						strText, strText.GetLength());
					strText.Format("%02d-%02d-%02d", tTmTime.tm_year + 1900, tTmTime.tm_mon + 1, tTmTime.tm_mday);
					::TextOut(hDC, nTextX,
						middle + (nGridCount / 2) * nGridHeight + rect.top + 20,
						strText, strText.GetLength());

					nLastX = nX;
				}
			}
		}

		t = tLastTime;
		int nDiff = (int)(t - tFirstTime);
		int nX = (int)(((double)nDiff / dwCollectInterval ) * g_data.m_dwCollectIntervalWidth);
		if (nLastX == -1 || nX - nLastX >= 80) {
			int nTextX = MYIMAGE_LEFT_BLANK + rect.left + nX;
			if (nTextX > rectLeft.left) {
				struct tm tTmTime;
				localtime_s(&tTmTime, &t);

				strText.Format("  %02d:%02d:%02d", tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);				
				::TextOut(hDC, nTextX,
					middle + (nGridCount / 2) * nGridHeight + rect.top + 5,
					strText, strText.GetLength());
				strText.Format("%02d-%02d-%02d", tTmTime.tm_year + 1900, tTmTime.tm_mon + 1, tTmTime.tm_mday);
				::TextOut(hDC, nTextX,
					middle + (nGridCount / 2) * nGridHeight + rect.top + 20,
					strText, strText.GetLength());

				nLastX = nX;
			}
		}

	}

	// 画边框
	::SelectObject(hDC, m_hCommonThreadPen);
	::FillRect(hDC, &rectLeft, m_hCommonBrush);
	::Rectangle(hDC, rectLeft.left, rectLeft.top, rectLeft.right, rectLeft.bottom);
	::MoveToEx(hDC, rectLeft.left + width - 1, rectLeft.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.bottom);
	::MoveToEx(hDC, rectLeft.left, rectLeft.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.top);
	::MoveToEx(hDC, rectLeft.left, rectLeft.bottom - 1, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.bottom - 1);

	// 画刻度值
	nVInterval = MIN_TEMP_V_INTERVAL;
	for (int i = 0; i < nGridCount + 1; i++) {
		if (nVInterval >= MIN_TEMP_V_INTERVAL) {
			int  nTop = nFirstTop + i * nGridHeight;
			int  nTemperature = nFistTemperature - i;
			strText.Format("%d℃", nTemperature);
			::TextOut(hDC, rectLeft.right + (-40),
				nTop + rect.top + (-8),
				strText, strText.GetLength());
			nVInterval = nGridHeight;
		}
		else {
			nVInterval += nGridHeight;
		}
	}

	// 画出报警线
	::SelectObject(hDC, m_hLowTempAlarmPen);
	int nY = (int)((nMiddleTemp * 100.0 - (double)g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[m_dwSelectedReaderIndex].m_dwLowTempAlarm ) / 100.0 * nGridHeight);
	::MoveToEx(hDC, rectLeft.right, middle + nY + rect.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, middle + nY + rect.top);
	if ( g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[m_dwSelectedReaderIndex].m_szName[0] == '\0'
		 || 0 == strcmp(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[m_dwSelectedReaderIndex].m_szName, "--")) {
		strText.Format("No.%lu_低温报警", m_dwSelectedReaderIndex + 1);
	}
	else {
		strText.Format("%s_低温报警", g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[m_dwSelectedReaderIndex].m_szName);
	}
	::TextOut(hDC, rectLeft.right + 5,
		middle + nY + rect.top + 5,
		strText, strText.GetLength());


	::SelectObject(hDC, m_hHighTempAlarmPen);
	nY = (int)((nMiddleTemp * 100.0 - (double)g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[m_dwSelectedReaderIndex].m_dwHighTempAlarm) / 100.0 * nGridHeight);
	::MoveToEx(hDC, rectLeft.right, middle + nY + rect.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, middle + nY + rect.top);
	if (g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[m_dwSelectedReaderIndex].m_szName[0] == '\0'
		|| 0 == strcmp(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[m_dwSelectedReaderIndex].m_szName, "--")) {
		strText.Format("No.%lu_高温报警", m_dwSelectedReaderIndex + 1);
	}
	else {
		strText.Format("%s_高温报警", g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[m_dwSelectedReaderIndex].m_szName);
	}
	::TextOut(hDC, rectLeft.right + 5,
		middle + nY + rect.top + (-20),
		strText, strText.GetLength());

	// 画出类似注释的框
	::SetBkMode(hDC, TRANSPARENT);

	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];

		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;

			if (pItem->szRemark[0] != '\0') {
				int nDiff = (int)(pItem->tTime - tFirstTime);
				int nX = (int)(((double)nDiff / dwCollectInterval) * g_data.m_dwCollectIntervalWidth);
				int nY = (int)((nMiddleTemp * 100.0 - (double)pItem->dwTemperature) / 100.0 * nGridHeight);

				int nX1 = nX + MYIMAGE_LEFT_BLANK + rect.left;
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
				if (rectRemark.left < rectLeft.left) {
					rectRemark.left = rectLeft.left;
				}
				if (rectRemark.right < rectRemark.left) {
					rectRemark.right = rectRemark.left;
				}
				::DrawText(hDC, strText, strText.GetLength(), &rectRemark, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
		}
	}

	// 画十字线
	RECT rCross;
	rCross.left = rectLeft.right;
	rCross.right = rectLeft.left + width - 1;
	rCross.top = rectLeft.top;
	rCross.bottom = rectLeft.bottom;

	// 如果有点
	if ( g_data.m_CfgData.m_bCrossAnchor && (tFirstTime > 0) 
		&& ::PtInRect( &rCross, cursor_point ) ) {
		::SelectObject(hDC, m_hCommonThreadPen);
		::MoveToEx(hDC, cursor_point.x, rectLeft.top, 0);
		::LineTo(hDC, cursor_point.x, rectLeft.bottom);
		::MoveToEx(hDC, rectLeft.right, cursor_point.y, 0);
		::LineTo(hDC, rectLeft.left + width - 1, cursor_point.y);

		float  fTempCursor = nFistTemperature - (float)(cursor_point.y - rect.top - nFirstTop) / nGridHeight;
		float f1 = (cursor_point.x - rect.left - MYIMAGE_LEFT_BLANK)* dwCollectInterval / (float)g_data.m_dwCollectIntervalWidth;
		time_t tCursor = (time_t)f1 + tFirstTime;

		struct tm tTmTime;
		localtime_s(&tTmTime, &tCursor);

		strText.Format("%.2f℃,%02d:%02d:%02d", fTempCursor, tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec );
		int nFirstTop = middle - nGridHeight * (nGridCount / 2);
		::TextOut(hDC, cursor_point.x + 5, cursor_point.y - 20, strText, strText.GetLength());
	}
	

	return true;
}

void CMyImageUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_BUTTONDOWN)
	{
		//告诉UIManager这个消息需要处理
		m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
		return;
	}
	else if (event.Type == UIEVENT_MOUSEMOVE) {
		if (g_data.m_CfgData.m_bCrossAnchor)
			this->Invalidate();
	}
	DuiLib::CControlUI::DoEvent(event);
}

void  CMyImageUI::OnChangeSkin() {
	DeleteObject(m_hCommonThreadPen);
	DeleteObject(m_hBrighterThreadPen);
	DeleteObject(m_hCommonBrush);
	DeleteObject(m_hLowTempAlarmPen);
	DeleteObject(m_hHighTempAlarmPen);

	m_hCommonThreadPen = ::CreatePen(PS_SOLID, 1, g_data.m_skin.GetRgb(CMySkin::COMMON_PEN));
	m_hBrighterThreadPen = ::CreatePen(PS_SOLID, 1, g_data.m_skin.GetRgb(CMySkin::BRIGHT_PEN));
	m_hCommonBrush = ::CreateSolidBrush(g_data.m_skin.GetRgb(CMySkin::COMMON_BRUSH));
	m_hLowTempAlarmPen = ::CreatePen(PS_DASH, 1, g_data.m_skin.GetRgb(CMySkin::LOW_ALARM_PEN));
	m_hHighTempAlarmPen = ::CreatePen(PS_DASH, 1, g_data.m_skin.GetRgb(CMySkin::HIGH_ALARM_PEN));
}

time_t  CMyImageUI::GetFirstTime() {
	time_t  tTime = 0x7FFFFFFF;
	BOOL    bFind = FALSE;

	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		if (m_vTempData[i].size() > 0) {
			if (!bFind) {
				tTime = m_vTempData[i].at(0)->tTime;
				bFind = TRUE;
			}
			else if (m_vTempData[i].at(0)->tTime < tTime) {
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

	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		if (m_vTempData[i].size() > 0) {
			if (!bFind) {
				tTime = m_vTempData[i].at(m_vTempData[i].size() - 1)->tTime;
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

void  CMyImageUI::DrawTempPoint(int nIndex, Graphics & g, int x, int y, HDC hDc, int RADIUS /*= DEFAULT_POINT_RADIUS*/) {
	g.FillEllipse(m_temperature_brush[nIndex], x - RADIUS, y - RADIUS, 2 * RADIUS, 2 * RADIUS);
}

void  CMyImageUI::AddTemp(DWORD dwIndex, DWORD dwTemp) {
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
	//STRNCPY(pTemp->szRemark, "123", sizeof(pTemp->szRemark));

	DWORD dwGridIndex = GetTag();
	// 如果报警开关打开
	if ( !g_data.m_CfgData.m_bAlarmVoiceOff ) {
		if (   dwTemp < g_data.m_CfgData.m_GridCfg[dwGridIndex].m_ReaderCfg[dwIndex].m_dwLowTempAlarm 
			|| dwTemp > g_data.m_CfgData.m_GridCfg[dwGridIndex].m_ReaderCfg[dwIndex].m_dwHighTempAlarm ) {
			m_sigAlarm.emit();
		}
	}

	if (m_vTempData[dwIndex].size() >= MAX_POINTS_COUNT ) {
		vector<TempData *>::iterator it = m_vTempData[dwIndex].begin();
		TempData * pData = *it;
		m_vTempData[dwIndex].erase(it);
		delete pData;
	}
	m_vTempData[dwIndex].push_back(pTemp);

	// 重绘
	MyInvalidate();
}

void  CMyImageUI::MyInvalidate() {
	int nMinWidth = CalcMinWidth();
	this->SetMinWidth(nMinWidth);
	Invalidate();
	m_bSetParentScrollPos = TRUE;
}

// 计算图像需要的宽度
int   CMyImageUI::CalcMinWidth() {
	time_t  tFirstTime = GetFirstTime();
	if (tFirstTime < 0) {
		return 0;
	}

	time_t  tLastTime = GetLastTime();
	assert(tLastTime >= 0);

	DWORD  dwIndex = GetTag();
	//DWORD  dwInterval = g_data.m_CfgData.m_GridCfg[dwIndex].m_dwCollectInterval;
	DWORD  dwInterval = GetCollectInterval(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwCollectInterval);
	DWORD  dwUnitWidth = g_data.m_dwCollectIntervalWidth;
	DWORD  dwLeftColumnWidth = MYIMAGE_LEFT_BLANK;

	int nWidth = (int)((double)(tLastTime - tFirstTime) / dwInterval * dwUnitWidth);
	return nWidth + dwLeftColumnWidth + MYIMAGE_RIGHT_BLANK;
}

void  CMyImageUI::OnReaderSelected(DWORD  dwSelectedIndex) {
	m_dwSelectedReaderIndex = dwSelectedIndex;

	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		if (i == m_dwSelectedReaderIndex) {
			if ( m_type == TYPE_MAX)
				m_temperature_pen[i]->SetWidth(3.0);
			else 
				m_temperature_pen[i]->SetWidth(2.0);
		}
		else {
			if (m_type == TYPE_MAX)
				m_temperature_pen[i]->SetWidth(2.0);
			else
				m_temperature_pen[i]->SetWidth(2.0);
		}
	}
}

void   CMyImageUI::OnMyClick(const POINT * pPoint) {
	DWORD  dwIndex = this->GetTag();

	// 找到点击了哪个点
	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	RECT rect = this->GetPos();
	int  width = pParent->GetWidth();
	int  height = rect.bottom - rect.top;

	int nMinTemp = GetMinTemp(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwMinTemp);
	int nMaxTemp = GetMaxTemp(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwMaxTemp);
	int nGridCount = nMaxTemp - nMinTemp;
	int nMiddleTemp = (nMaxTemp + nMinTemp) / 2;

	int nGridHeight = height / nGridCount;
	int nReminder = height % nGridCount;
	int nVMargin = MIN_MYIMAGE_VMARGIN;
	if (nVMargin * 2 > nReminder) {
		int nSpared = (nVMargin * 2 - nReminder - 1) / nGridCount + 1;
		nGridHeight -= nSpared;
	}

	time_t  tFirstTime = GetFirstTime();
	time_t  tLastTime = GetLastTime();

	int middle = height / 2;
	int nRadius = 6;
	vector<TempData *>::iterator it;
	DWORD  dwCollectInterval = GetCollectInterval(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwCollectInterval);

	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];

		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;

			int nDiff = (int)(pItem->tTime - tFirstTime);
			int nX = (int)(((double)nDiff / dwCollectInterval ) *  g_data.m_dwCollectIntervalWidth);
			int nY = (int)((nMiddleTemp * 100.0 - (double)pItem->dwTemperature) / 100.0 * nGridHeight);

			int nX1 = nX + MYIMAGE_LEFT_BLANK + rect.left;
			int nY1 = nY + middle + rect.top;

			if (pPoint->x >= nX1 - 6 && pPoint->x <= nX1 + 6
				&& pPoint->y >= nY1 - 6 && pPoint->y <= nY1 + 6) {
				//JTelSvrPrint("you clicked the point!");

				// 如果之前在编辑状态
				if (g_edRemark->IsVisible()) {
					OnEdtRemarkKillFocus_g(this);
				}

				m_dwCurTempIndex = pItem->dwIndex;
				RECT rectRemark;
				rectRemark.left = nX1 - EDT_REMARK_WIDTH / 2 + 1;
				rectRemark.top = nY1 + EDT_REMARK_Y_OFFSET + 3;
				rectRemark.right = rectRemark.left + EDT_REMARK_WIDTH - 2;
				rectRemark.bottom = rectRemark.top + EDT_REMARK_HEIGHT - 6;
				g_edRemark->SetPos(rectRemark);
				g_edRemark->SetText(pItem->szRemark);
				g_edRemark->SetVisible(true);
				g_edRemark->SetFocus();

				g_data.m_bAutoScroll = FALSE;
				break;
			}
		}
	}
}

void   CMyImageUI::SetRemark(DuiLib::CDuiString & strRemark) {
	// assert(m_dwCurTempIndex != -1);
	if (m_dwCurTempIndex == -1) {
		return;
	}

	vector<TempData *>::iterator it;
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];
		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;
			if (pItem->dwIndex == m_dwCurTempIndex) {
				STRNCPY(pItem->szRemark, strRemark, sizeof(pItem->szRemark));
				return;
			}
		}
	}
}

void  CMyImageUI::ExportExcel(const char * szOrgPatientName) {
	CDuiString strText;

	char szPatientName[256] = {0};
	if ( 0 != szOrgPatientName && 0 != strcmp(szOrgPatientName, "--") ) {
		STRNCPY(szPatientName, szOrgPatientName, sizeof(szPatientName));
	}

	if (!CExcel::IfExcelInstalled()) {
		::MessageBox(g_data.m_hWnd, "没有检测到系统安装了excel", "保存excel", 0);
		return;
	}

	DWORD  dwDataCnt = 0;
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		dwDataCnt += m_vTempData[i].size();
	}
	if (dwDataCnt == 0) {
		strText.Format("没有温度数据，放弃保存excel");
		::MessageBox(g_data.m_hWnd, strText, "保存excel", 0);
		return;
	}

	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名  

	char szTime[256];
	time_t now = time(0);
	Date2String_1(szTime, sizeof(szTime), &now);
	SNPRINTF(strFilename, sizeof(strFilename), "%s_%s", szPatientName, szTime);

	ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
	ofn.hwndOwner = g_data.m_hWnd;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
	ofn.lpstrFilter = TEXT("Excel Flie(*.xls)\0*.xls\0Excel Flie(*.xlsx)\0*.xlsx\0\0");//设置过滤  
	ofn.nFilterIndex = 1;//过滤器索引  
	ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL  
	ofn.nMaxFile = sizeof(strFilename);//缓冲区长度  
	ofn.lpstrInitialDir = CPaintManagerUI::GetInstancePath();
	ofn.lpstrTitle = TEXT("请选择一个文件");//使用系统默认标题留空即可  
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项  
	if (!GetSaveFileName(&ofn))
	{
		return;
	}

	CExcel  excel;
	excel.WriteGrid(0, 0, "姓名");
	excel.WriteGrid(0, 1, szPatientName);

	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		char tmp[64];
		SNPRINTF(tmp, sizeof(tmp), "时间%lu", i + 1);
		excel.WriteGrid(2, 0 + i * 4, tmp);

		SNPRINTF(tmp, sizeof(tmp), "体温%lu", i + 1);
		excel.WriteGrid(2, 1 + i * 4, tmp);

		SNPRINTF(tmp, sizeof(tmp), "注释%lu", i + 1);
		excel.WriteGrid(2, 2 + i * 4, tmp);
	}

	std::vector<const char *> vValues;
	vector<TempData *>::iterator it;
	std::vector<const char *>::iterator  ix;

	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];
		for (it = vTempData.begin(); it != vTempData.end(); it++) {
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

		excel.WriteRange(3, 0 + i * 4, vTempData.size() + 3 - 1, 2 + i * 4, vValues);

		for (ix = vValues.begin(); ix != vValues.end(); ++ix) {
			if (*ix != 0) {
				delete[] * ix;
			}
		}
		vValues.clear();
	}

	char buf[8192];
	if ( 1 == ofn.nFilterIndex ) {
		SNPRINTF(buf, size(buf), "%s.xls", strFilename);
	}
	else if (2 == ofn.nFilterIndex) {
		SNPRINTF(buf, size(buf), "%s.xlsx", strFilename);
	}
	else {
		SNPRINTF(buf, size(buf), "%s", strFilename);
	}

	int ret = excel.SaveAs(buf);
	excel.Quit();

	if (ret != 0) {
		strText.Format("保存excel数据失败。请确保excel文件没有打开");
		::MessageBox( g_data.m_hWnd, strText, "保存excel", 0);
	}
}

void  CMyImageUI::PrintExcel(char szReaderName[MAX_READERS_PER_GRID][64], const char * szOrgPatientName) {
	CDuiString strText;

	if (!CExcelEx::IfExcelInstalled()) {
		::MessageBox( g_data.m_hWnd, "没有检测到系统安装了excel", "保存excel", 0);
		return;
	}

	char szPatientName[256] = { 0 };
	if (0 != szOrgPatientName && 0 != strcmp(szOrgPatientName, "--")) {
		STRNCPY(szPatientName, szOrgPatientName, sizeof(szPatientName));
	}

	DWORD  dwDataCnt = 0;
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		dwDataCnt += m_vTempData[i].size();
	}
	if (dwDataCnt == 0) {
		strText.Format("没有温度数据，放弃打印excel图表");
		::MessageBox(g_data.m_hWnd, strText, "打印excel图表", 0);
		return;
	}

	TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名  
	char szTime[256];
	time_t now = time(0);
	Date2String(szTime, sizeof(szTime), &now);
	SNPRINTF(strFilename, sizeof(strFilename), "%s %s", szPatientName, szTime);

	CExcelEx  excel;
	vector<TempData *>::iterator it;
	CExcelEx::Series s[MAX_READERS_PER_GRID] = { 0 };
	DWORD  dwMin = 3500;

	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];
		if (vTempData.size() == 0) {
			s[i].bEmpty = TRUE;
			continue;
		}

		s[i].bEmpty = FALSE;
		s[i].dwStartRowIndex = 0;
		s[i].dwStartColIndex = i * 2;
		s[i].dwEndRowIndex = vTempData.size() - 1;
		STRNCPY(s[i].szName, szReaderName[i], sizeof(s[i].szName));

		if (vTempData.size() > g_dwPrintExcelMaxPointsCnt) {
			s[i].dwStartRowIndex = vTempData.size() - g_dwPrintExcelMaxPointsCnt;
		}

		DWORD j = 0;
		for (it = vTempData.begin(), j = 0; it != vTempData.end(); it++, j++) {
			TempData * pItem = *it;

			Time2String(szTime, sizeof(szTime), &pItem->tTime);
			excel.WriteGrid(j, i * 2, szTime);

			char szTemperature[8];
			SNPRINTF(szTemperature, 8, "%.2f", pItem->dwTemperature / 100.0);
			excel.WriteGrid(j, i * 2 + 1, szTemperature);

			if (pItem->dwTemperature < dwMin) {
				dwMin = pItem->dwTemperature;
			}
		}
	}

	double dMin = 0.0;
	if (dwMin >= 3500) {
		dMin = 34.0;
	}
	else if (dwMin > 3000) {
		dMin = 30.0;
	}
	else if (dwMin > 2600) {
		dMin = 26.0;
	}
	else if (dwMin > 2200) {
		dMin = 22.0;
	}
	excel.PrintChartWithMultiSeries(s, MAX_READERS_PER_GRID, strFilename, 0, 0, TRUE, &dMin);
	excel.Quit();
}

void  CMyImageUI::EmptyData() {
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		ClearVector(m_vTempData[i]);
	}
}

void  CMyImageUI::EmptyData(DWORD  dwSubIndex) {
	assert(dwSubIndex < MAX_READERS_PER_GRID);
	ClearVector(m_vTempData[dwSubIndex]);
}

void  CMyImageUI::OnTempSqliteRet(std::vector<TempData*> & vRet, DWORD  dwIndex) {
	std::vector<TempData*>::iterator it;
	for ( it = vRet.begin(); it != vRet.end(); it++ ) {
		TempData*  pData = *it;

		pData->dwIndex = m_dwNextTempIndex;
		m_dwNextTempIndex++;
		// skip -1 index
		if (m_dwNextTempIndex == -1) {
			m_dwNextTempIndex = 0;
		}
	}
	m_vTempData[dwIndex].insert(m_vTempData[dwIndex].begin(), vRet.begin(), vRet.end());
}