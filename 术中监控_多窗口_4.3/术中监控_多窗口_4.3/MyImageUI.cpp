#include "MyImageUI.h"
#include "GridUI.h"

CMyImageUI::CMyImageUI() {
	// 初始状态为显示7日曲线
	m_state = STATE_7_DAYS;
	m_nSingleDayIndex = 0;

	m_hCommonThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x66, 0x66, 0x66));
	m_hBrighterThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x99, 0x99, 0x99));
	m_hCommonBrush = ::CreateSolidBrush(RGB(0x43, 0x42, 0x48));
	m_hLowTempAlarmPen = ::CreatePen(PS_DASH, 1, RGB(0x02, 0xA5, 0xF1));
	m_hHighTempAlarmPen = ::CreatePen(PS_DASH, 1, RGB(0xFC, 0x23, 0x5C));
	m_hDaySplitThreadPen = ::CreatePen(PS_DASHDOTDOT, 1, RGB(0x99, 0x99, 0x99));
	m_fSecondsPerPixel = 0.0f;
	m_bSetSecondsPerPixel = FALSE;

	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		m_temperature_pen[i] = new Pen(Gdiplus::Color(g_ReaderIndicator[i]), 1.0);
		m_temperature_brush[i] = new SolidBrush(Gdiplus::Color(g_ReaderIndicator[i]));
	}
}

CMyImageUI::~CMyImageUI() {
	DeleteObject(m_hCommonThreadPen);
	DeleteObject(m_hBrighterThreadPen);
	DeleteObject(m_hCommonBrush);
	DeleteObject(m_hLowTempAlarmPen);
	DeleteObject(m_hHighTempAlarmPen);
	DeleteObject(m_hDaySplitThreadPen);

	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		delete m_temperature_pen[i];
		delete m_temperature_brush[i];
	}
}

bool CMyImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	if (m_state == STATE_7_DAYS) {
		DoPaint_7Days(hDC, rcPaint, pStopControl);
	}
	else {
		DoPaint_SingleDay(hDC, rcPaint, pStopControl);
	}

	return true;
}

void CMyImageUI::DoEvent(DuiLib::TEventUI& event) {
	if ( event.Type == UIEVENT_DBLCLICK) {
		OnDbClick();
	}
	else if (event.Type == UIEVENT_SCROLLWHEEL) {
		OnMyMouseWheel(event.wParam, event.lParam);
	}
	CControlUI::DoEvent(event);
}

LPCTSTR CMyImageUI::GetClass() const {
	return "MyImage";
}

int  CMyImageUI::GetMyWidth() {
	CControlUI * pParent = GetParent();
	assert(pParent);
	return pParent->GetWidth();
}

int  CMyImageUI::GetMyScrollX() {
	CContainerUI * pParent = (CContainerUI *)GetParent();
	assert(pParent);
	SIZE s = pParent->GetScrollPos();
	return s.cx;
}

CControlUI * CMyImageUI::GetGrid() {
	CControlUI * pParent = GetParent();
	DWORD  i = 0;

	while (pParent && i < 5) {
		pParent = pParent->GetParent();
		i++;
	}

	if (pParent)
		return pParent;
	else {
		assert(0);
		return 0;
	}
}

DWORD   CMyImageUI::GetGridIndex() {
	CControlUI * pParent = GetGrid();
	assert(pParent);
	return pParent->GetTag();
}

DWORD   CMyImageUI::GetReaderIndex() {
	CGridUI * pParent = (CGridUI *)GetGrid();
	return pParent->GetReaderIndex();
}

const   std::vector<TempItem * > & CMyImageUI::GetTempData(DWORD j) {
	CGridUI * pParent = (CGridUI *)GetGrid();
	return pParent->GetTempData(j);
}

CModeButton::Mode  CMyImageUI::GetMode() {
	CGridUI * pParent = (CGridUI *)GetGrid();
	return pParent->GetMode();
}

int   CMyImageUI::GetCelsiusHeight(int height, int nCelsiusCount) {
	int h        = height / nCelsiusCount;
	int r        = height % nCelsiusCount;
	int nVMargin = MIN_MYIMAGE_VMARGIN;

	if ( nVMargin * 2 > r ) {
		int nSpared = (nVMargin * 2 - r - 1) / nCelsiusCount + 1;
		h -= nSpared;
	}
	return h;
}

// 画水平刻度线
void   CMyImageUI::DrawScaleLine( HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY,
	                              const RECT & rectScale, const RECT & rect ) {
	int nVInterval = BRIGHT_DARK_INTERVAL;
	for (int i = 0; i < nCelsiusCnt + 1; i++) {
		if (nVInterval >= BRIGHT_DARK_INTERVAL) {
			::SelectObject(hDC, m_hBrighterThreadPen);
			nVInterval = nHeightPerCelsius;
		}
		else {
			::SelectObject(hDC, m_hCommonThreadPen);
			nVInterval += nHeightPerCelsius;
		}
		int  nTop = nMaxY + i * nHeightPerCelsius;
		::MoveToEx(hDC, rectScale.right, nTop, 0);
		::LineTo(hDC, rect.right, nTop);
	}
}

// 画边框
void   CMyImageUI::DrawBorder(HDC hDC, const RECT & rectScale, int width) {

	::MoveToEx(hDC, rectScale.left, rectScale.top, 0);
	::LineTo(hDC, rectScale.left, rectScale.bottom);

	int nX = rectScale.left + width - 1;
	::MoveToEx(hDC, nX, rectScale.top, 0);
	::LineTo(hDC, nX, rectScale.bottom);
}

// 画刻度值
void  CMyImageUI::DrawScale( HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY, int nMaxTemp,
	                         const RECT & rectScale, int width ) {
	CDuiString strText;
	int nVInterval = BRIGHT_DARK_INTERVAL;
	::SetTextColor(hDC, RGB(255, 255, 255));
	::Rectangle(hDC, rectScale.left, rectScale.top, rectScale.right, rectScale.bottom);

	RECT r = rectScale;
	r.left++;
	r.right--;
	r.top++;
	r.bottom--;
	::FillRect(hDC, &r, m_hCommonBrush);

	for (int i = 0; i < nCelsiusCnt + 1; i++) {
		if (nVInterval >= BRIGHT_DARK_INTERVAL) {
			int  nTop = nMaxY + i * nHeightPerCelsius;
			int  nTemperature = nMaxTemp - i;
			strText.Format("%d℃", nTemperature);
			::TextOut(hDC, rectScale.right + (-40), nTop + (-8),
				strText, strText.GetLength());
			nVInterval = nHeightPerCelsius;
		}
		else {
			nVInterval += nHeightPerCelsius;
		}
	}
}

// 画报警线
void   CMyImageUI::DrawWarning( HDC hDC,DWORD i, DWORD j, int nMaxTemp, int nHeightPerCelsius, int nMaxY,
	                            const RECT & rectScale, int width ) {
	CDuiString  strWarningLow;
	CDuiString  strWarningHigh;
	int nLowAlarm = 0;
	int nHighAlarm = 0;
	if (j == 0) {
		nLowAlarm = g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm;
		nHighAlarm = g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm;
		strWarningLow = "低温报警";
		strWarningHigh = "高温报警";
	}
	else {
		nLowAlarm = g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j-1].m_dwLowTempAlarm;
		nHighAlarm = g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j-1].m_dwHighTempAlarm;
		strWarningLow.Format("%c 低温报警", (char)((j-1)+'A') );
		strWarningHigh.Format("%c 高温报警", (char)((j - 1) + 'A'));
	}

	int nDiffY = 0;

	nDiffY = (int)((nMaxTemp * 100.0 - (double)nLowAlarm) / 100.0 * nHeightPerCelsius);
	::SelectObject(hDC, m_hLowTempAlarmPen);
	::MoveToEx(hDC, rectScale.right, nMaxY + nDiffY, 0);
	::LineTo(hDC, rectScale.left + width - 1, nMaxY + nDiffY );	
	::TextOut(hDC, rectScale.right + 5, nMaxY + nDiffY + 5, strWarningLow, strWarningLow.GetLength());

	nDiffY = (int)((nMaxTemp * 100.0 - (double)nHighAlarm) / 100.0 * nHeightPerCelsius);
	::SelectObject(hDC, m_hHighTempAlarmPen);
	::MoveToEx(hDC, rectScale.right, nMaxY + nDiffY, 0);
	::LineTo(hDC, rectScale.left + width - 1, nMaxY + nDiffY);	
	::TextOut(hDC, rectScale.right + 5, nMaxY + nDiffY + (-20), strWarningHigh, strWarningHigh.GetLength());
}

time_t  CMyImageUI::GetFirstTime(DWORD i, DWORD j, CModeButton::Mode mode) {
	time_t  tTime = 0x7FFFFFFF;
	BOOL    bFind = FALSE;

	if (j > 0) {
		// 多接收器模式
		if (mode == CModeButton::Mode_Multiple) {
			for (DWORD k = 0; k < MAX_READERS_PER_GRID; k++) {
				const  std::vector<TempItem * > & vTemp = GetTempData(k+1);
				if (vTemp.size() > 0) {
					if (!bFind) {
						tTime = vTemp[0]->m_time;
						bFind = TRUE;
					}
					else if (vTemp[0]->m_time < tTime) {
						tTime = vTemp[0]->m_time;
					}
				}
			}
		}
		else if (mode == CModeButton::Mode_Single ) {
			assert(j == 1);
			const  std::vector<TempItem * > & vTemp = GetTempData(1);
			if (vTemp.size() > 0) {
				tTime = vTemp[0]->m_time;
				bFind = TRUE;
			}
		}
		else {
			assert(0);
		}		
	}
	// j == 0，手持读卡器模式
	else {
		assert(mode == CModeButton::Mode_Hand);
		const  std::vector<TempItem * > & vTemp = GetTempData(0);
		if (vTemp.size() > 0) {
			tTime = vTemp[0]->m_time;
			bFind = TRUE;
		}
	}
	
	if (bFind) {
		return tTime;
	}
	else {
		return -1;
	}
}

// 7日视图有几天数据
int   CMyImageUI::GetDayCounts(DWORD i, DWORD j, CModeButton::Mode mode) {
	time_t  first_time = GetFirstTime(i,j,mode);
	if (-1 == first_time) {
		return 1;
	}

	time_t  today_zero_time = GetTodayZeroTime();
	if (first_time >= today_zero_time) {
		return 1;
	}

	// 一周前的开始位置
	time_t  tWeekBegin = today_zero_time - 3600 * 24 * 6;

	// 如果开始时间比一周前还早
	if (first_time <= tWeekBegin) {
		return 7;
	}

	return (int)(today_zero_time - first_time - 1) / (3600 * 24) + 1 + 1;
}

// 画日子分割线
void   CMyImageUI::DrawDaySplit( HDC hDC, int nDayCounts, const RECT & rectScale, int nDayWidth, int nMaxY, 
	                             int nCelsiusCnt, int nHeightPerCelsius, time_t  tFirstDayZeroTime) {
	::SelectObject(hDC, m_hDaySplitThreadPen);
	for (int i = 0; i < nDayCounts - 1; i++) {
		::MoveToEx(hDC, rectScale.right + nDayWidth * (i + 1), rectScale.top, 0);
		::LineTo(hDC, rectScale.right + nDayWidth * (i + 1), rectScale.bottom );
	}

	int nWeekDayIndex = GetWeekDay(tFirstDayZeroTime);

	RECT  tmpRect;
	char  szDate[256];
	CDuiString strText;

	for (int i = 0; i < nDayCounts; i++) {
		tmpRect.left   = rectScale.right + nDayWidth * i;
		tmpRect.right  = tmpRect.left + nDayWidth;
		tmpRect.top    = nMaxY + nHeightPerCelsius * nCelsiusCnt;
		tmpRect.bottom = rectScale.bottom;

		time_t  t = tFirstDayZeroTime + 3600 * 24 * i;
		Date2String(szDate, sizeof(szDate), &t);
		strText.Format("%s %s", szDate, GetWeekDayName((nWeekDayIndex + i) % 7));
		::DrawText(hDC, strText, strText.GetLength(), &tmpRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}
}

// 画折线图 范围[tFirstTime, tLastTime)
void    CMyImageUI::DrawPolyline(time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel,
	int    nMaxTemp, int nHeightPerCelsius, POINT  tTopLeft, Graphics & graphics,
	BOOL  bDrawPoints, DWORD i, DWORD j, CModeButton::Mode mode ) {

	if ( mode == CModeButton::Mode_Hand ) {
		const std::vector<TempItem * > & v = GetTempData(0);
		DrawPolyline(tFirstTime, tLastTime, fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, 
			tTopLeft, graphics, bDrawPoints, v, m_temperature_pen[0], m_temperature_brush[0]);
	}
	else if ( mode == CModeButton::Mode_Single ) {
		const std::vector<TempItem * > & v = GetTempData(1);
		DrawPolyline(tFirstTime, tLastTime, fSecondsPerPixel, nMaxTemp, nHeightPerCelsius,
			tTopLeft, graphics, bDrawPoints, v, m_temperature_pen[0], m_temperature_brush[0]);
	}
	else {
		for ( DWORD k = 0; k < MAX_READERS_PER_GRID; k++ ) {
			const std::vector<TempItem * > & v = GetTempData(k+1);
			DrawPolyline(tFirstTime, tLastTime, fSecondsPerPixel, nMaxTemp, nHeightPerCelsius,
				tTopLeft, graphics, bDrawPoints, v, m_temperature_pen[k], m_temperature_brush[k]);
		}
	}
}

// 画折线图 范围[tFirstTime, tLastTime]，只画单个vector
void    CMyImageUI::DrawPolyline(time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel,
	int    nMaxTemp, int nHeightPerCelsius, POINT  tTopLeft, Graphics & graphics,
	BOOL  bDrawPoints, const  std::vector<TempItem * > & vTempData, Pen * pen, SolidBrush * brush) {

	DWORD dwMaxTempCnt = vTempData.size();
	Gdiplus::Point * points = new Gdiplus::Point[dwMaxTempCnt];
	vector<TempItem *>::const_iterator it;

	// 找到第一个点
	for (it = vTempData.begin(); it != vTempData.end(); it++) {
		TempItem * pItem = *it;
		if ( pItem->m_time >= tFirstTime ) {
			break;
		}
	}

	// 找到第一个点后，其他的点不在和起始时间点比较
	// 临时存储相同x坐标的vector
	vector<TempItem *>  vTmp;
	// 临时vector的item有共同的点，它们的x坐标相同
	int nTmpX = 0;
	// points数组的大小
	int cnt = 0;

	for (; it != vTempData.end(); it++) {
		TempItem * pItem = *it;
		// 如果最后时间有值
		if (tLastTime > 0) {
			// 如果超出范围
			if ( pItem->m_time > tLastTime ) {
				break;
			}
		}

		int  nX = (int)((pItem->m_time - tFirstTime) / fSecondsPerPixel);

		if (vTmp.size() == 0) {
			vTmp.push_back(pItem);
			nTmpX = nX;
		}
		else {
			// 如果偏移量和上次的相同，放置在临时vector中
			if (nX <= nTmpX + 1) {
				vTmp.push_back(pItem);
			}
			else {
				vector<TempItem *>::iterator  it_tmp;
				int  sum = 0;
				for (it_tmp = vTmp.begin(); it_tmp != vTmp.end(); ++it_tmp) {
					TempItem * pTmpItem = *it_tmp;
					sum += pTmpItem->m_dwTemp;
				}
				// 求平均值
				int  ave = sum / vTmp.size();

				points[cnt].X = nTmpX + tTopLeft.x;
				points[cnt].Y = tTopLeft.y + (int)((nMaxTemp - ave / 100.0) * nHeightPerCelsius);
				cnt++;

				vTmp.clear();
				vTmp.push_back(pItem);
				nTmpX = nX;
			}
		}
	}

	if (vTmp.size() > 0) {
		vector<TempItem *>::iterator  it_tmp;
		int  sum = 0;
		for (it_tmp = vTmp.begin(); it_tmp != vTmp.end(); ++it_tmp) {
			TempItem * pTmpItem = *it_tmp;
			sum += pTmpItem->m_dwTemp;
		}
		// 求平均值
		int  ave = sum / vTmp.size();

		points[cnt].X = nTmpX + tTopLeft.x;
		points[cnt].Y = tTopLeft.y + (int)((nMaxTemp - ave / 100.0) * nHeightPerCelsius);
		cnt++;

		vTmp.clear();
	}

	if (bDrawPoints && fSecondsPerPixel > 0.0f && fSecondsPerPixel < 6.0f) {
		for (int m = 0; m < cnt; ++m) {
			DrawPoint( brush, graphics, points[m].X, points[m].Y, 0, 3);
		}
	}

	if (cnt > 1)
		graphics.DrawLines(pen, points, cnt);
	else if (1 == cnt)
		DrawPoint(brush, graphics, points[0].X, points[0].Y, 0, 3);

	delete[] points;
}

void    CMyImageUI::DrawPoint(SolidBrush * brush, Graphics & g, int x, int y, HDC hDc, int radius) {
	g.FillEllipse(brush, x - radius, y - radius, 2 * radius, 2 * radius);
}

void   CMyImageUI::DoPaint_7Days(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	DuiLib::CDuiString strText;

	// GDI+
	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	// self rectangle and width, height
	RECT rect     = GetPos();
	int  width    = GetMyWidth();
	int  height   = rect.bottom - rect.top;
	// 水平滑动条位置
	int  nScrollX = GetMyScrollX();

	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();
	  
	// 显示的最低温度和最高温度
	int  nMinTemp          = g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp;
	int  nMaxTemp          = g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp;
	// 摄氏度个数
	int  nCelsiusCount     = nMaxTemp - nMinTemp;
	// 每个摄氏度的高度
	int  nHeightPerCelsius = GetCelsiusHeight( height, nCelsiusCount );
	// 垂直留白
	int  nVMargin = (height - nHeightPerCelsius * nCelsiusCount) / 2;
	// 最高温度的Y坐标系值
	int  nMaxY = rect.top + nVMargin;

	// 全图分为左边刻度区域和右边折线图
	RECT rectScale;
	rectScale.left   = rect.left+nScrollX;
	rectScale.top    = rect.top;
	rectScale.right  = rectScale.left + SCALE_RECT_WIDTH;
	rectScale.bottom = rect.bottom;

	// 采集间隔(单位：秒)
	int  nCollectInterval = GetCollectInterval( g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval );

	// 画水平刻度线
	DrawScaleLine(hDC,nCelsiusCount, nHeightPerCelsius, nMaxY, rectScale, rect);

	// 画边框
	DrawBorder(hDC, rectScale, width);

	// 画刻度值
	DrawScale(hDC, nCelsiusCount, nHeightPerCelsius, nMaxY, nMaxTemp, rectScale, width);

	// 画报警线
	DrawWarning( hDC,i,j,nMaxTemp, nHeightPerCelsius, nMaxY, rectScale, width );

	// 计算温度曲线跨越几个日子
	int  nDayCounts = GetDayCounts(i,j,mode);
	assert(nDayCounts > 0);
	int  nDayWidth = (width - SCALE_RECT_WIDTH) / nDayCounts;
	time_t   tTodayZeroTime = GetTodayZeroTime();
	time_t   tFirstDayZeroTime = tTodayZeroTime - 3600 * 24 * (nDayCounts - 1);

	// 画日子的分割线
	DrawDaySplit(hDC, nDayCounts, rectScale, nDayWidth, nMaxY, nCelsiusCount, nHeightPerCelsius, tFirstDayZeroTime);
	
	float fSecondsPerPixel = 0.0f;
	// 画折线
	if (nDayWidth > 0) {
		fSecondsPerPixel = (3600 * 24.0f) / (float)nDayWidth;
		POINT  top_left;
		top_left.x = rectScale.right;
		top_left.y = nMaxY;
		DrawPolyline(tFirstDayZeroTime, -1, fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, 
			top_left, graphics, TRUE, i,j,mode);
	}
	
	// 画十字线
}

// 获得single day的起始时间和结束时间
void  CMyImageUI::GetSingleDayTimeRange(time_t & start, time_t & end, DWORD i, DWORD j, CModeButton::Mode mode) {
	time_t   tTodayZeroTime = GetTodayZeroTime();
	start = tTodayZeroTime + 3600 * 24 * m_nSingleDayIndex;
	end   = start + 3600 * 24 - 1;

	if (mode == CModeButton::Mode_Hand) {
		const std::vector<TempItem * > & v = GetTempData(0);
		GetTimeRange(v, start, end);
	}
	else if (mode == CModeButton::Mode_Single) {
		const std::vector<TempItem * > & v = GetTempData(1);
		GetTimeRange(v, start, end);
	}
	else {
		time_t tmp_s = 0;
		time_t tmp_e = 0;
		BOOL bFirst = TRUE;

		for (DWORD k = 0; k < MAX_READERS_PER_GRID; k++) {
			const std::vector<TempItem * > & v = GetTempData(k + 1);
			if ( v.size() == 0 )
				continue;

			time_t t1 = start, t2 = end;
			GetTimeRange(v, t1, t2);
			if ( bFirst ) {
				tmp_s = t1;
				tmp_e = t2;
			}
			else {
				if (t1 < tmp_s) {
					tmp_s = t1;
				}
				if (t2 > tmp_e) {
					tmp_e = t2;
				}
			}			
		}

		start = tmp_s;
		end = tmp_e;
	}
}

void  CMyImageUI::GetTimeRange(const std::vector<TempItem * > & v, time_t & start, time_t & end) {
	std::vector<TempItem * >::const_iterator it;
	std::vector<TempItem * >::const_reverse_iterator itr;

	for (it = v.begin(); it != v.end(); ++it) {
		TempItem * pItem = *it;
		if (pItem->m_time >= start) {
			start = pItem->m_time;
			break;
		}
	}

	for (itr = v.rbegin(); itr != v.rend(); ++itr) {
		TempItem * pItem = *itr;
		if (pItem->m_time <= end) {
			end = pItem->m_time;
			break;
		}
	}
}

// 画时间文本
void    CMyImageUI::DrawTimeText( HDC hDC, time_t  tFirstTime , time_t tLastTime, 
	                              float fSecondsPerPixel, POINT  top_left ) {

}

void  CMyImageUI::DoPaint_SingleDay(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	DuiLib::CDuiString strText;

	// GDI+
	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	// self rectangle and width, height
	RECT rect = GetPos();
	int  width = GetMyWidth();
	int  height = rect.bottom - rect.top;
	// 水平滑动条位置
	int  nScrollX = GetMyScrollX();

	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();

	// 显示的最低温度和最高温度
	int  nMinTemp = g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp;
	int  nMaxTemp = g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp;
	// 摄氏度个数
	int  nCelsiusCount = nMaxTemp - nMinTemp;
	// 每个摄氏度的高度
	int  nHeightPerCelsius = GetCelsiusHeight(height, nCelsiusCount);
	// 垂直留白
	int  nVMargin = (height - nHeightPerCelsius * nCelsiusCount) / 2;
	// 最高温度的Y坐标系值
	int  nMaxY = rect.top + nVMargin;

	// 全图分为左边刻度区域和右边折线图
	RECT rectScale;
	rectScale.left = rect.left + nScrollX;
	rectScale.top = rect.top;
	rectScale.right = rectScale.left + SCALE_RECT_WIDTH;
	rectScale.bottom = rect.bottom;

	// 采集间隔(单位：秒)
	int  nCollectInterval = GetCollectInterval(g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval);

	// 画水平刻度线
	DrawScaleLine(hDC, nCelsiusCount, nHeightPerCelsius, nMaxY, rectScale, rect);

	// 画边框
	DrawBorder(hDC, rectScale, width);

	// 查看有无数据
	int nPointsCnt = GetTempCount(i, j, mode);
	// 如果没有数据就不重绘了 
	if ( nPointsCnt > 0 ) {
		// 画温度曲线
		POINT  top_left;
		top_left.x = rect.left + SCALE_RECT_WIDTH;
		top_left.y = nMaxY;

		time_t  tFirstTime = 0, tLastTime = 0;
		GetSingleDayTimeRange(tFirstTime, tLastTime, i, j, mode);

		DrawPolyline(tFirstTime, tLastTime, m_fSecondsPerPixel, nMaxTemp, nHeightPerCelsius,
			top_left, graphics, TRUE, i, j, mode);

		top_left.y += nHeightPerCelsius * nCelsiusCount;
		// 画时间文本
		DrawTimeText(hDC, tFirstTime, tLastTime, m_fSecondsPerPixel, top_left);
	}

	// 画刻度值
	DrawScale(hDC, nCelsiusCount, nHeightPerCelsius, nMaxY, nMaxTemp, rectScale, width);

	// 画报警线
	DrawWarning(hDC, i, j, nMaxTemp, nHeightPerCelsius, nMaxY, rectScale, width);


}

void CMyImageUI::MyInvalidate() {
	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();

	if ( m_state == STATE_SINGLE_DAY ) {
		time_t  tFirstTime = 0, tLastTime = 0;
		GetSingleDayTimeRange(tFirstTime, tLastTime, i, j, mode);
		int  width = GetMyWidth();

		if ( !m_bSetSecondsPerPixel ) {
			m_fSecondsPerPixel = (float)(tLastTime - tFirstTime) / (width - SCALE_RECT_WIDTH);
			m_bSetSecondsPerPixel = TRUE;
		}
		else {
			width = (int)((tLastTime - tFirstTime) / m_fSecondsPerPixel) + SCALE_RECT_WIDTH;
		}
		
		this->SetMinWidth(width);
	}
	else {
		int  width = GetMyWidth();
		this->SetMinWidth(width);
	}
	Invalidate();
}

// 双击事件
void  CMyImageUI::OnDbClick() {
	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();

	// 查看有无数据
	int nPointsCnt = GetTempCount(i,j,mode);
	
	// 如果没有数据就不重绘了 
	if (0 == nPointsCnt) {
		return;
	}


	// 如果是7日视图
	if (m_state == STATE_7_DAYS) {
		// 检查点击了那一天
		m_nSingleDayIndex = GetClickDayIndex(i, j, mode);
		assert(m_nSingleDayIndex >= -6 && m_nSingleDayIndex <= 0);
		m_state = STATE_SINGLE_DAY;
		m_bSetSecondsPerPixel = FALSE;
	}
	else {
		m_state = STATE_7_DAYS;
	}

	MyInvalidate();
}

// 温度数据个数
DWORD   CMyImageUI::GetTempCount(DWORD i, DWORD j, CModeButton::Mode mode) {
	DWORD dwPointsCnt = 0;
	if (mode == CModeButton::Mode_Hand) {
		const std::vector<TempItem * > & v = GetTempData(0);
		dwPointsCnt = v.size();
	}
	else if (mode == CModeButton::Mode_Single) {
		const std::vector<TempItem * > & v = GetTempData(1);
		dwPointsCnt = v.size();
	}
	else {
		for (DWORD k = 0; k < MAX_READERS_PER_GRID; k++) {
			const std::vector<TempItem * > & v = GetTempData(k + 1);
			dwPointsCnt += v.size();
		}
	}
	return dwPointsCnt;
}

// 计算双击了第几天
int  CMyImageUI::GetClickDayIndex(DWORD i, DWORD j, CModeButton::Mode mode) {
	POINT cursor_point;
	GetCursorPos(&cursor_point);
	::ScreenToClient(g_data.m_hWnd, &cursor_point);

	RECT rect       = GetPos();
	int  width      = GetMyWidth();
	int  height     = rect.bottom - rect.top;
	int  nScrollX   = GetMyScrollX();
	int  nDayCounts = GetDayCounts(i, j, mode);
	int  nDayWidth  = (width - SCALE_RECT_WIDTH) / nDayCounts;

	RECT rectScale;
	rectScale.left   = rect.left + nScrollX;
	rectScale.top    = rect.top;
	rectScale.right  = rectScale.left + SCALE_RECT_WIDTH;
	rectScale.bottom = rect.bottom;

	// 偏移量
	int  nOffsetX    = cursor_point.x - rectScale.right;

	if (nOffsetX < 0 || nOffsetX > nDayCounts * nDayWidth)
		return - (nDayCounts - 1);

	int k = 0;
	for (k = 0; k < nDayCounts; k++) {
		if ( nOffsetX < nDayWidth * (k + 1) ) {
			break;
		}
	}
	return k - (nDayCounts - 1);
}

// 鼠轮滑动
void   CMyImageUI::OnMyMouseWheel(WPARAM wParam, LPARAM lParam) {
	if (!m_bSetSecondsPerPixel)
		return;

	int nDirectory = (int)wParam;
	if (nDirectory > 0) {
		if (m_fSecondsPerPixel < 200.0f) {
			m_fSecondsPerPixel *= 1.2f;
		}
	}
	else {
		if (m_fSecondsPerPixel > 1.2f) {
			m_fSecondsPerPixel /= 1.2f;
		}
	}
	MyInvalidate();
}

// 为label作画
void   CMyImageUI::PaintForLabelUI(HDC hDC, int width, int height, const RECT & rect) {

}






CImageLabelUI::CImageLabelUI() {
	m_image = 0;
}

CImageLabelUI::~CImageLabelUI() {

}
 
bool CImageLabelUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CLabelUI::DoPaint(hDC, rcPaint, pStopControl);

	if (0 == m_image) {
		return true;
	}

	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	int width = pParent->GetWidth();
	int height = pParent->GetHeight();
	RECT r = this->GetPos();

	SetBkMode(hDC, TRANSPARENT);

	m_image->PaintForLabelUI(hDC, width, height, r);

	return true;
}

void CImageLabelUI::DoEvent(DuiLib::TEventUI& event) {
	CLabelUI::DoEvent(event);
}

LPCTSTR  CImageLabelUI::GetClass() const {
	return "ImageLabel";
}
