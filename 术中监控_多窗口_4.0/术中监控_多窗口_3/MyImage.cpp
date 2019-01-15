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
	m_hDaySplitThreadPen = ::CreatePen(PS_DASHDOTDOT, 1, g_data.m_skin.GetRgb(CMySkin::BRIGHT_PEN));
	
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

	// 初始状态为显示7日曲线
	m_state = STATE_7_DAYS;
	// m_state = STATE_SINGLE_DAY;
	m_nSingleDayIndex = 0; // -6, -5, -4, ......, 0
	m_SingleDayZeroTime = 0;
	m_fSecondsPerPixel = 0.0f;
	m_bSetSecondsPerPixel = FALSE;
}

CMyImageUI::~CMyImageUI() {
	DeleteObject(m_hCommonThreadPen );
	DeleteObject(m_hBrighterThreadPen);
	DeleteObject(m_hCommonBrush);
	DeleteObject(m_hLowTempAlarmPen);
	DeleteObject(m_hHighTempAlarmPen);
	DeleteObject(m_hDaySplitThreadPen);
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		delete m_temperature_pen[i];
		delete m_temperature_brush[i];
		ClearVector(m_vTempData[i]);
	}
}

void   CMyImageUI::SubPaint_1(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	DuiLib::CDuiString strText;

	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	RECT rect = this->GetPos();
	int  width = pParent->GetWidth();
	int  height = rect.bottom - rect.top;
	DWORD  dwIndex = this->GetTag();


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
	int nMiddleTemp = (nMaxTemp + nMinTemp) / 2;

	::SetTextColor(hDC, g_data.m_skin.GetRgb(CMySkin::COMMON_TEXT_COLOR));
	::SelectObject(hDC, m_hCommonThreadPen);

	RECT rectLeft;
	rectLeft.left   = rect.left;
	rectLeft.top    = rect.top;
	rectLeft.right  = rectLeft.left + MYIMAGE_LEFT_BLANK;
	rectLeft.bottom = rect.bottom;

	int nFirstTop = middle - nGridHeight * (nGridCount / 2);
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
	int nY = (int)((nMiddleTemp * 100.0 - (double)g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[m_dwSelectedReaderIndex].m_dwLowTempAlarm) / 100.0 * nGridHeight);
	::MoveToEx(hDC, rectLeft.right, middle + nY + rect.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, middle + nY + rect.top);
	if (g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[m_dwSelectedReaderIndex].m_szName[0] == '\0'
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

	// 计算温度曲线跨越几个日子
	int  nDayCounts = GetDayCounts();
	assert(nDayCounts > 0);
	int  nDaySpare = (width - MYIMAGE_LEFT_BLANK) % nDayCounts;
	int  nDayWidth = (width - MYIMAGE_LEFT_BLANK) / nDayCounts;

	// 画日子的分割线
	::SelectObject(hDC, m_hDaySplitThreadPen);
	for ( int i = 0; i < nDayCounts - 1; i++ ) {
		::MoveToEx(hDC, rectLeft.right + nDayWidth * (i+1), rect.top, 0);
		::LineTo(hDC, rectLeft.right + nDayWidth * (i + 1), rect.bottom);
	}

	time_t   tTodayZeroTime = GetTodayZeroTime();
	time_t   tFirstDayZeroTime = tTodayZeroTime - 3600 * 24 * (nDayCounts - 1);
	int nWeekDayIndex = GetWeekDay(tFirstDayZeroTime);

	RECT  tmpRect;
	char  szDate[256];
	for ( int i = 0; i < nDayCounts; i++ ) {
		tmpRect.left   = rectLeft.right + nDayWidth * i;
		tmpRect.right  = tmpRect.left + nDayWidth;
		tmpRect.top    = rectLeft.top + nFirstTop + nGridHeight * nGridCount;
		tmpRect.bottom = rect.bottom;

		time_t  t = tFirstDayZeroTime + 3600 * 24 * i;
		Date2String(szDate, sizeof(szDate), &t);
		strText.Format("%s %s", szDate, GetWeekDayName((nWeekDayIndex + i) % 7) );
		::DrawText(hDC, strText, strText.GetLength(), &tmpRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	float  fSecondsPerPixel = 0.0f;
	// 画折线图
	if ( nDayWidth > 0 ) {
		fSecondsPerPixel = (3600 * 24.0f) / (float)nDayWidth;
		POINT  top_left;
		top_left.x = rectLeft.right;
		top_left.y = nFirstTop + rectLeft.top;
		DrawPolyline( tFirstDayZeroTime, -1, fSecondsPerPixel, nMaxTemp, nGridHeight, top_left, graphics );
	}

	// 画出类似注释的框
	::SetBkMode(hDC, TRANSPARENT);

	time_t tFirstTime = tFirstDayZeroTime;
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];
		vector<TempData *>::iterator it;
		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;

			if (pItem->szRemark[0] != '\0') {
				int nDiff = (int)(pItem->tTime - tFirstTime);
				int nX = (int)((float)nDiff / fSecondsPerPixel);
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
	
}

void  CMyImageUI::SubPaint_0(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	DuiLib::CDuiString strText;

	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	SIZE tParentScrollPos = pParent->GetScrollPos();
	SIZE tParentScrollRange = pParent->GetScrollRange();
	RECT rect = this->GetPos();
	int  width = pParent->GetWidth();
	int  height = rect.bottom - rect.top;
	DWORD  dwIndex = this->GetTag();

	int nRadius = RADIUS_SIZE_IN_MAXIUM;

	POINT cursor_point;
	GetCursorPos(&cursor_point);
	::ScreenToClient(g_data.m_hWnd, &cursor_point);

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
	//DWORD  dwCollectInterval = GetCollectInterval(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwCollectInterval);

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
	
	time_t  tFirstTime, tLastTime;
	BOOL bFindMin, bFindMax;
	CalcSingleDay(tFirstTime, bFindMin, tLastTime, bFindMax);

	// 如果有点
	if (bFindMin) {
		assert(bFindMax);
		// 如果有两个点以上，画温度曲线
		if (tLastTime > tFirstTime) {
			POINT  top_left;
			top_left.x = rect.left + MYIMAGE_LEFT_BLANK; //rectLeft.right;
			top_left.y = nFirstTop + rectLeft.top;
			DrawPolyline(tFirstTime, tLastTime, m_fSecondsPerPixel, nMaxTemp, nGridHeight, top_left, graphics, TRUE );

			// 画时间
			// 从第一个10秒整数，画时间
			if (tFirstTime >= 0) {
				int  remainder = tFirstTime % 10;
				time_t  tFirstTime_1 = tFirstTime;
				if (remainder > 0) {
					tFirstTime_1 += 10 - remainder;
				}

				int nLastX = -1;
				time_t t = 0;
				for (t = tFirstTime_1; t < tLastTime; t += 10) {
					int nDiff = (int)(t - tFirstTime);
					int nX = (int)( (float)nDiff / m_fSecondsPerPixel );

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
				int nX = (int)( (float)nDiff  / m_fSecondsPerPixel );
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
		}
		// 只有一个点
		else {

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
	int nMiddleTemp = (nMaxTemp + nMinTemp) / 2;
	::SelectObject(hDC, m_hLowTempAlarmPen);
	int nY = (int)((nMiddleTemp * 100.0 - (double)g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[m_dwSelectedReaderIndex].m_dwLowTempAlarm) / 100.0 * nGridHeight);
	::MoveToEx(hDC, rectLeft.right, middle + nY + rect.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, middle + nY + rect.top);
	if (g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[m_dwSelectedReaderIndex].m_szName[0] == '\0'
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
		vector<TempData *>::iterator it;
		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;

			if (pItem->szRemark[0] != '\0') {
				int nDiff = (int)(pItem->tTime - tFirstTime);
				int nX = (int)( (float)nDiff / m_fSecondsPerPixel );
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
	if (g_data.m_CfgData.m_bCrossAnchor && bFindMin
		&& ::PtInRect(&rCross, cursor_point)) {
		::SelectObject(hDC, m_hCommonThreadPen);
		::MoveToEx(hDC, cursor_point.x, rectLeft.top, 0);
		::LineTo(hDC, cursor_point.x, rectLeft.bottom);
		::MoveToEx(hDC, rectLeft.right, cursor_point.y, 0);
		::LineTo(hDC, rectLeft.left + width - 1, cursor_point.y);

		float  fTempCursor = nFistTemperature - (float)(cursor_point.y - rect.top - nFirstTop) / nGridHeight;
		int n1 = cursor_point.x - rect.left - MYIMAGE_LEFT_BLANK;
		time_t tCursor = (time_t)( (float)n1 * m_fSecondsPerPixel ) + tFirstTime ;

		struct tm tTmTime;
		localtime_s(&tTmTime, &tCursor);

		strText.Format("%.2f℃,%02d:%02d:%02d", fTempCursor, tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);
		int nFirstTop = middle - nGridHeight * (nGridCount / 2);
		::TextOut(hDC, cursor_point.x + 5, cursor_point.y - 20, strText, strText.GetLength());
	}
}

bool CMyImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	DuiLib::CControlUI::DoPaint(hDC, rcPaint, pStopControl);	

	if (m_state == STATE_7_DAYS) {
		SubPaint_1(hDC, rcPaint, pStopControl);
	}
	else {
		SubPaint_0(hDC, rcPaint, pStopControl);
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
	m_hDaySplitThreadPen = ::CreatePen(PS_DASH, 1, g_data.m_skin.GetRgb(CMySkin::BRIGHT_PEN));
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
	else if (m_vTempData[dwIndex].size() > 0) {
		TempData * pFirst = m_vTempData[dwIndex].at(0);
		time_t  tTodayZeroTime = GetTodayZeroTime();
		// 如果时间超过一周
		if ( (tTodayZeroTime > pFirst->tTime) && (tTodayZeroTime - pFirst->tTime > 3600 * 24 * 6 ) ) {
			vector<TempData *>::iterator it = m_vTempData[dwIndex].begin();
			TempData * pData = *it;
			m_vTempData[dwIndex].erase(it);
			delete pData;
		}
	}
	m_vTempData[dwIndex].push_back(pTemp);

	// 重绘
	MyInvalidate();
}

void  CMyImageUI::MyInvalidate() {
	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	int  width = pParent->GetWidth();

	// 重新计算宽度
	if ( m_state == STATE_SINGLE_DAY ) {
		time_t  tMin, tMax;
		BOOL bFindMin, bFindMax;
		CalcSingleDay(tMin, bFindMin, tMax, bFindMax);

		if ( bFindMin ) {
			assert(bFindMax);
			// 如果没有设置每秒像素率，计算该率
			if ( !m_bSetSecondsPerPixel ) {
				if ( width > MYIMAGE_LEFT_BLANK ) {
					if (tMax > tMin) {
						m_fSecondsPerPixel = (float)(tMax - tMin) / (float)(width - MYIMAGE_LEFT_BLANK);
						if ( m_fSecondsPerPixel < 1.0f ) {
							m_fSecondsPerPixel = 1.0f;
						}
						m_bSetSecondsPerPixel = TRUE;
					}
				}
			}
			else {
				width = (int)((tMax - tMin) / m_fSecondsPerPixel) + MYIMAGE_LEFT_BLANK;
			}			
		}
		this->SetMinWidth(width);
	}
	else {
		DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
		int  width = pParent->GetWidth();
		this->SetMinWidth(width);
	}
	Invalidate();	
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
				m_temperature_pen[i]->SetWidth(1.5);
			else 
				m_temperature_pen[i]->SetWidth(1.0);
		}
		else {
			if (m_type == TYPE_MAX)
				m_temperature_pen[i]->SetWidth(1.0);
			else
				m_temperature_pen[i]->SetWidth(1.0);
		}
	}
}

void   CMyImageUI::OnMyClick(const POINT * pPoint) {
	if ( m_state == STATE_7_DAYS ) {
		return;
	}

	if ( !m_bSetSecondsPerPixel ) {
		return;
	}

	if ( m_fSecondsPerPixel > 6.0f ) {
		return;
	}

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

	time_t  tFirstTime, tLastTime;
	BOOL bFindMin, bFindMax;
	CalcSingleDay(tFirstTime, bFindMin, tLastTime, bFindMax);

	int middle = height / 2;
	int nRadius = 6;
	vector<TempData *>::iterator it;
	// DWORD  dwCollectInterval = GetCollectInterval(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwCollectInterval);

	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];

		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;
			if ( !(pItem->tTime >= tFirstTime && pItem->tTime < tLastTime) ) {
				continue;
			}

			int nDiff = (int)(pItem->tTime - tFirstTime);
			int nX = (int)( nDiff / m_fSecondsPerPixel );
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

				// g_data.m_bAutoScroll = FALSE;
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
				CBusiness::GetInstance()->SetRemarkAsyn(m_sTagId[i].c_str(), pItem->tTime, strRemark);
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

	EmptyData(dwIndex);
	m_vTempData[dwIndex].insert(m_vTempData[dwIndex].begin(), vRet.begin(), vRet.end());
}

// 7日视图有几天数据
int  CMyImageUI::GetDayCounts( ) {	
	time_t  first_time      = GetFirstTime();
	if ( -1 == first_time ) {
		return 1;
	}

	time_t  today_zero_time = GetTodayZeroTime();
	if ( first_time >= today_zero_time ) {
		return 1;
	}

	// 一周前的开始位置
	time_t  tWeekBegin = today_zero_time - 3600 * 24 * 6;

	// 如果开始时间比一周前还早
	if ( first_time <= tWeekBegin ) {
		return 7;
	}

	return (int)(today_zero_time - first_time - 1) / (3600 * 24) + 1 + 1;
}

void  CMyImageUI::DrawPolyline(
	time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel,
	int  nHighestTemp, int nPixelPerCelsius, POINT  tTopLeft, Graphics & graphics,
	BOOL  bDrawPoints /*= FALSE*/ ) {

	Gdiplus::Point * points = new Gdiplus::Point[MAX_POINTS_COUNT];
	vector<TempData *>::iterator it;
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];
		
		// 找到第一个点
		for ( it = vTempData.begin(); it != vTempData.end(); it++ ) {
			TempData * pItem = *it;
			if ( pItem->tTime >= tFirstTime) {
				break;
			}
		}

		// 找到第一个点后，其他的点不在和起始时间点比较
		// 临时存储相同x坐标的vector
		vector<TempData *>  vTmp;
		// 临时vector的item有共同的点，它们的x坐标相同
		int nTmpX = 0;
		// points数组的大小
		int cnt = 0;

		for ( ; it != vTempData.end(); it++) {
			TempData * pItem = *it;
			// 如果最后时间有值
			if ( tLastTime > 0 ) {
				// 如果超出范围
				if ( pItem->tTime >= tLastTime ) {
					break;
				}
			}
			
			int  nX = (int)((pItem->tTime - tFirstTime) / fSecondsPerPixel);

			if ( vTmp.size() == 0 ) {
				vTmp.push_back(pItem);
				nTmpX = nX;
			}
			else {
				// 如果偏移量和上次的相同，放置在临时vector中
				if (nX <= nTmpX + 1) {
					vTmp.push_back(pItem);
				}
				else {
					vector<TempData *>::iterator  it_tmp;
					int  sum = 0;
					for (it_tmp = vTmp.begin(); it_tmp != vTmp.end(); ++it_tmp) {
						TempData * pTmpItem = *it_tmp;
						sum += pTmpItem->dwTemperature;
					}
					// 求平均值
					int  ave = sum / vTmp.size();

					points[cnt].X = nTmpX + tTopLeft.x;
					points[cnt].Y = tTopLeft.y + (int)((nHighestTemp - ave / 100.0) * nPixelPerCelsius);
					cnt++;

					vTmp.clear();
					vTmp.push_back(pItem);
					nTmpX = nX;
				}
			}
		}

		if (vTmp.size() > 0) {
			vector<TempData *>::iterator  it_tmp;
			int  sum = 0;
			for (it_tmp = vTmp.begin(); it_tmp != vTmp.end(); ++it_tmp) {
				TempData * pTmpItem = *it_tmp;
				sum += pTmpItem->dwTemperature;
			}
			// 求平均值
			int  ave = sum / vTmp.size();

			points[cnt].X = nTmpX + tTopLeft.x;
			points[cnt].Y = tTopLeft.y + (int)((nHighestTemp - ave / 100.0) * nPixelPerCelsius);
			cnt++;

			vTmp.clear();
		}

		if ( bDrawPoints && fSecondsPerPixel > 0.0f && fSecondsPerPixel < 6.0f ) {
			for (int m = 0; m < cnt; ++m) {
				DrawTempPoint(i, graphics, points[m].X, points[m].Y, 0, 3);
			}
		}	

		graphics.DrawLines(m_temperature_pen[i], points, cnt);
	}
	delete[] points;
}

void  CMyImageUI::OnDbClick() {
	int nPointsCnt = 0;
	for (int i = 0; i < MAX_READERS_PER_GRID; ++i) {
		nPointsCnt += m_vTempData[i].size();
	}
	// 如果没有数据就不重绘了 
	if ( 0 == nPointsCnt ) {
		return;
	}

	if ( m_state == STATE_7_DAYS ) {
		POINT cursor_point;
		GetCursorPos(&cursor_point);
		::ScreenToClient(g_data.m_hWnd, &cursor_point);

		DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
		RECT rect = this->GetPos();
		int  width = pParent->GetWidth();

		// 计算温度曲线跨越几个日子
		int  nDayCounts = GetDayCounts();
		assert(nDayCounts > 0);
		int  nDaySpare = (width - MYIMAGE_LEFT_BLANK) % nDayCounts;
		int  nDayWidth = (width - MYIMAGE_LEFT_BLANK) / nDayCounts;

		int  nOffsetX = cursor_point.x - rect.left;

		int i = 0;
		for ( i = 0; i < nDayCounts; i++ ) {
			if ( nOffsetX < MYIMAGE_LEFT_BLANK + nDayWidth * (i+1) ) {
				break;
			}
		}
		m_nSingleDayIndex = i - (nDayCounts - 1);
		assert(m_nSingleDayIndex >= -6 && m_nSingleDayIndex <= 0);
		time_t tTodayZeroTime = GetTodayZeroTime();
		m_SingleDayZeroTime = tTodayZeroTime - 3600 * 24 * ( 0 - m_nSingleDayIndex);
		assert( m_SingleDayZeroTime >= 0 );		
		m_state = STATE_SINGLE_DAY;
		m_bSetSecondsPerPixel = FALSE;
	}
	else {
		m_state = STATE_7_DAYS;
	}

	MyInvalidate();
}

void  CMyImageUI::OnMouseWheel(BOOL bPositive) {
	if ( m_bSetSecondsPerPixel ) {
		if (bPositive) {
			if (m_fSecondsPerPixel < 200.0f) {
				m_fSecondsPerPixel *= 1.2f;
			}			
		}
		else {
			if ( m_fSecondsPerPixel > 1.2f ) {
				m_fSecondsPerPixel /= 1.2f;
			}
		}
	}
	
	MyInvalidate();
}

void   CMyImageUI::CalcSingleDay(time_t & tMin, BOOL & bFindMin, time_t & tMax, BOOL & bFindMax) {
	tMin = time(0);
	bFindMin = FALSE;
	for (int m = 0; m < MAX_READERS_PER_GRID; m++) {
		vector<TempData *> & v = m_vTempData[m];
		vector<TempData *>::iterator it;
		for (it = v.begin(); it != v.end(); ++it) {
			TempData * pItem = *it;
			if (pItem->tTime >= m_SingleDayZeroTime) {
				if (pItem->tTime < m_SingleDayZeroTime + 3600 * 24) {
					if (pItem->tTime < tMin) {
						tMin = pItem->tTime;
					}
					bFindMin = TRUE;
					break;
				}
				else {
					break;
				}
			}
		}
	}

	tMax = 0;
	bFindMax = FALSE;
	for (int m = 0; m < MAX_READERS_PER_GRID; m++) {
		vector<TempData *> & v = m_vTempData[m];
		for (int n = v.size(); n > 0; n--) {
			TempData * pItem = v.at(n - 1);
			if (pItem->tTime >= m_SingleDayZeroTime) {
				if (pItem->tTime < m_SingleDayZeroTime + 3600 * 24) {
					if (pItem->tTime > tMax) {
						tMax = pItem->tTime;
					}
					bFindMax = TRUE;
					break;
				}
			}
			else {
				break;
			}
		}
	}
}

void  CMyImageUI::SetTagId(DWORD  dwSubIndex, const char * szTagId) {
	assert(dwSubIndex < MAX_READERS_PER_GRID);
	if (szTagId)
		m_sTagId[dwSubIndex] = szTagId;
	else
		m_sTagId[dwSubIndex] = "";
}