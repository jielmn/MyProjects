#include <time.h>
#include "MyImage.h"
#include "skin.h"
#include "business.h"

CMyImageUI::CMyImageUI(E_TYPE e) :	m_remark_pen(Gdiplus::Color(0x803D5E49), 3.0),
							m_remark_brush(Gdiplus::Color(0x803D5E49)) {
	m_hCommonThreadPen   = ::CreatePen(PS_SOLID, 1, g_data.m_skin.GetRgb(CMySkin::COMMON_PEN) );
	m_hBrighterThreadPen = ::CreatePen(PS_SOLID, 1, g_data.m_skin.GetRgb(CMySkin::BRIGHT_PEN));
	m_hCommonBrush       = ::CreateSolidBrush(g_data.m_skin.GetRgb(CMySkin::COMMON_BRUSH));
	m_hLowTempAlarmPen   = ::CreatePen(PS_DASH, 1, g_data.m_skin.GetRgb(CMySkin::LOW_ALARM_PEN));
	m_hHighTempAlarmPen  = ::CreatePen(PS_DASH, 1, g_data.m_skin.GetRgb(CMySkin::HIGH_ALARM_PEN));
	
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		m_temperature_pen[i] = new Pen(Gdiplus::Color(g_data.m_argb[i]), 3.0);
		m_temperature_brush[i] = new SolidBrush(Gdiplus::Color(g_data.m_argb[i]));
	}

	m_dwSelectedReaderIndex = 0;
	m_dwNextTempIndex = 0;
	m_type = e;

	CBusiness * pBusiness = CBusiness::GetInstance();	
	m_sigUpdateScroll.connect(pBusiness, &CBusiness::OnUpdateScroll);
	m_sigAlarm.connect(pBusiness, &CBusiness::OnAlarm);
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

	/* 开始作图 */
	int nMinTemp = GetMinTemp(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwMinTemp);
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

	::SetTextColor(hDC, g_data.m_skin.GetRgb(CMySkin::COMMON_TEXT_COLOR));
	::SelectObject(hDC, m_hCommonThreadPen);

	RECT rectLeft;
	rectLeft.left = rect.left + tParentScrollPos.cx;
	rectLeft.top = rect.top;
	rectLeft.right = rectLeft.left + MYIMAGE_LEFT_BLANK;
	rectLeft.bottom = rect.bottom;

	int nFirstTop = middle - nGridHeight * (nGridCount / 2);
	int nFistTemperature = MAX_TEMPERATURE;
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
	int nMiddleTemp = (MAX_TEMPERATURE + nMinTemp) / 2;

	vector<TempData *>::iterator it;
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];

		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;

			int nDiff = (int)(pItem->tTime - tFirstTime);
			int nX    = (int)(((double)nDiff / dwCollectInterval )
				        * g_data.m_dwCollectIntervalWidth );
			int nY    = (int)((nMiddleTemp * 100.0 - (double)pItem->dwTemperature) / 100.0 * nGridHeight);

			DrawTempPoint(i, graphics, nX + MYIMAGE_LEFT_BLANK + rect.left, nY + middle + rect.top, hDC, nRadius );

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
					strText.Format("%02d:%02d:%02d", tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);

					::TextOut(hDC, nTextX,
						middle + (nGridCount / 2) * nGridHeight + rect.top + 5,
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
				strText.Format("%02d:%02d:%02d", tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);

				::TextOut(hDC, nTextX,
					middle + (nGridCount / 2) * nGridHeight + rect.top + 5,
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

	return true;
}

void CMyImageUI::DoEvent(DuiLib::TEventUI& event) {
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

	if (m_vTempData[dwIndex].size() >= 1500) {
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