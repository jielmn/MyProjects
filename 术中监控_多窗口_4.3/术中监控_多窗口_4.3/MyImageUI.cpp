#include "MyImageUI.h"
#include "GridUI.h"

CMyImageUI::CMyImageUI() {
	// 初始状态为显示7日曲线
	m_state = STATE_7_DAYS;

	m_hCommonThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x66, 0x66, 0x66));
	m_hBrighterThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x99, 0x99, 0x99));
	m_hCommonBrush = ::CreateSolidBrush(RGB(0x43, 0x42, 0x48));
	m_hLowTempAlarmPen = ::CreatePen(PS_DASH, 1, RGB(0x02, 0xA5, 0xF1));
	m_hHighTempAlarmPen = ::CreatePen(PS_DASH, 1, RGB(0xFC, 0x23, 0x5C));
	m_hDaySplitThreadPen = ::CreatePen(PS_DASHDOTDOT, 1, RGB(0x99, 0x99, 0x99));
}

CMyImageUI::~CMyImageUI() {
	DeleteObject(m_hCommonThreadPen);
	DeleteObject(m_hBrighterThreadPen);
	DeleteObject(m_hCommonBrush);
	DeleteObject(m_hLowTempAlarmPen);
	DeleteObject(m_hHighTempAlarmPen);
	DeleteObject(m_hDaySplitThreadPen);
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
	
}

void  CMyImageUI::DoPaint_SingleDay(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {

}






CImageLabelUI::CImageLabelUI() {

}

CImageLabelUI::~CImageLabelUI() {

}
 
bool CImageLabelUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CLabelUI::DoPaint(hDC, rcPaint, pStopControl);
	return true;
}

void CImageLabelUI::DoEvent(DuiLib::TEventUI& event) {
	CLabelUI::DoEvent(event);
}

LPCTSTR  CImageLabelUI::GetClass() const {
	return "ImageLabel";
}
