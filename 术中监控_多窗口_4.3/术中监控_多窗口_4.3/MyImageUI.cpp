#include "MyImageUI.h"
#include "GridUI.h"
#include "business.h"

#define  MAX_SECONDS_PER_PIXEL               200.0f
#define  MIN_SECONDS_PER_PIXEL               1.0f

CMyImageUI::CMyImageUI() {
	// ��ʼ״̬Ϊ��ʾ7������
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

	m_bDragDrop = FALSE;
	m_DragDropObj = DragDrop_None;
	m_CursorObj = DragDrop_None;
	m_dwRemarkingIndex = 0;

	m_remark_pen = new Pen(Gdiplus::Color(0x803D5E49), 3.0);
	m_remark_brush = new SolidBrush( Gdiplus::Color(0x803D5E49) );
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

	delete m_remark_pen;
	delete m_remark_brush;
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
	else if (event.Type == UIEVENT_MOUSEMOVE) {
		// ���û�п�ʼ�ϷŲ���
		if ( !m_bDragDrop ) {
			// ������λ�ã��ڸ��¡����±�������ʱ���ı����״̬
			CheckCursor(event.ptMouse);
		}
		// �����ʼ������ϷŲ���
		else {
			DragDropIng(event.ptMouse);
		}

		if (g_data.m_CfgData.m_bCrossAnchor)
			this->Invalidate();
	}
	else if (event.Type == UIEVENT_BUTTONDOWN) {
		// �������Ϊ���϶�״̬
		if ( m_CursorObj != DragDrop_None ) {
			BeginDragDrop();
		}
		else {
			CheckRemark(event.ptMouse);
		}
	}
	else if (event.Type == UIEVENT_BUTTONUP) {
		// ��������ϷŲ���
		if ( m_bDragDrop ) {
			EndDragDrop(event.ptMouse);
		}
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

// ��ˮƽ�̶���
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

// ���߿�
void   CMyImageUI::DrawBorder(HDC hDC, const RECT & rectScale, int width) {

	::MoveToEx(hDC, rectScale.left, rectScale.top, 0);
	::LineTo(hDC, rectScale.left, rectScale.bottom);

	int nX = rectScale.left + width - 1;
	::MoveToEx(hDC, nX, rectScale.top, 0);
	::LineTo(hDC, nX, rectScale.bottom);
}

// ���̶�ֵ
void  CMyImageUI::DrawScale( HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY, int nMaxTemp,
	                         const RECT & rectScale, int width ) {
	CDuiString strText;
	int nVInterval = BRIGHT_DARK_INTERVAL;
	::SetTextColor(hDC, RGB(255, 255, 255));
	::SelectObject(hDC, m_hCommonThreadPen);
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
			strText.Format("%d��", nTemperature);
			::TextOut(hDC, rectScale.right + (-40), nTop + (-8),
				strText, strText.GetLength());
			nVInterval = nHeightPerCelsius;
		}
		else {
			nVInterval += nHeightPerCelsius;
		}
	}
}

// ��������
void   CMyImageUI::DrawWarning( HDC hDC,DWORD i, DWORD j, int nMaxTemp, int nHeightPerCelsius, int nMaxY,
	                            const RECT & rectScale, int width ) {
	CDuiString  strWarningLow;
	CDuiString  strWarningHigh;
	int nLowAlarm = 0;
	int nHighAlarm = 0;
	if (j == 0) {
		nLowAlarm = g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm;
		nHighAlarm = g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm;
		strWarningLow = "���±���";
		strWarningHigh = "���±���";
	}
	else {
		nLowAlarm = g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j-1].m_dwLowTempAlarm;
		nHighAlarm = g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j-1].m_dwHighTempAlarm;
		strWarningLow.Format("%c ���±���", (char)((j-1)+'A') );
		strWarningHigh.Format("%c ���±���", (char)((j - 1) + 'A'));
	}

	int nDiffY = 0;

	::SetTextColor(hDC, RGB(255, 255, 255));
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
		// �������ģʽ
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
	// j == 0���ֳֶ�����ģʽ
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

// 7����ͼ�м�������
int   CMyImageUI::GetDayCounts(DWORD i, DWORD j, CModeButton::Mode mode) {
	time_t  first_time = GetFirstTime(i,j,mode);
	if (-1 == first_time) {
		return 1;
	}

	time_t  today_zero_time = GetTodayZeroTime();
	if (first_time >= today_zero_time) {
		return 1;
	}

	// һ��ǰ�Ŀ�ʼλ��
	time_t  tWeekBegin = today_zero_time - 3600 * 24 * 6;

	// �����ʼʱ���һ��ǰ����
	if (first_time <= tWeekBegin) {
		return 7;
	}

	return (int)(today_zero_time - first_time - 1) / (3600 * 24) + 1 + 1;
}

// �����ӷָ���
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

// ������ͼ ��Χ[tFirstTime, tLastTime)
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

// ������ͼ ��Χ[tFirstTime, tLastTime]��ֻ������vector
void    CMyImageUI::DrawPolyline(time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel,
	int    nMaxTemp, int nHeightPerCelsius, POINT  tTopLeft, Graphics & graphics,
	BOOL  bDrawPoints, const  std::vector<TempItem * > & vTempData, Pen * pen, SolidBrush * brush) {

	DWORD dwMaxTempCnt = vTempData.size();
	Gdiplus::Point * points = new Gdiplus::Point[dwMaxTempCnt];
	vector<TempItem *>::const_iterator it;

	// �ҵ���һ����
	for (it = vTempData.begin(); it != vTempData.end(); it++) {
		TempItem * pItem = *it;
		if ( pItem->m_time >= tFirstTime ) {
			break;
		}
	}

	// �ҵ���һ����������ĵ㲻�ں���ʼʱ���Ƚ�
	// ��ʱ�洢��ͬx�����vector
	vector<TempItem *>  vTmp;
	// ��ʱvector��item�й�ͬ�ĵ㣬���ǵ�x������ͬ
	int nTmpX = 0;
	// points����Ĵ�С
	int cnt = 0;

	for (; it != vTempData.end(); it++) {
		TempItem * pItem = *it;
		// ������ʱ����ֵ
		if (tLastTime > 0) {
			// ���������Χ
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
			// ���ƫ�������ϴε���ͬ����������ʱvector��
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
				// ��ƽ��ֵ
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
		// ��ƽ��ֵ
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
	// ˮƽ������λ��
	int  nScrollX = GetMyScrollX();

	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();
	  
	// ��ʾ������¶Ⱥ�����¶�
	int  nMinTemp, nMaxTemp;
	GetMaxMinShowTemp(nMinTemp, nMaxTemp, i, j, mode);
	// ���϶ȸ���
	int  nCelsiusCount     = nMaxTemp - nMinTemp;
	// ÿ�����϶ȵĸ߶�
	int  nHeightPerCelsius = GetCelsiusHeight( height, nCelsiusCount );
	// ��ֱ����
	int  nVMargin = (height - nHeightPerCelsius * nCelsiusCount) / 2;
	// ����¶ȵ�Y����ϵֵ
	int  nMaxY = rect.top + nVMargin;

	// ȫͼ��Ϊ��߿̶�������ұ�����ͼ
	RECT rectScale;
	rectScale.left   = rect.left+nScrollX;
	rectScale.top    = rect.top;
	rectScale.right  = rectScale.left + SCALE_RECT_WIDTH;
	rectScale.bottom = rect.bottom;

	// ��ˮƽ�̶���
	DrawScaleLine(hDC,nCelsiusCount, nHeightPerCelsius, nMaxY, rectScale, rect);

	// ���߿�
	DrawBorder(hDC, rectScale, width);

	// ���̶�ֵ
	DrawScale(hDC, nCelsiusCount, nHeightPerCelsius, nMaxY, nMaxTemp, rectScale, width);

	// ��������
	DrawWarning( hDC,i,j,nMaxTemp, nHeightPerCelsius, nMaxY, rectScale, width );

	// �����¶����߿�Խ��������
	int  nDayCounts = GetDayCounts(i,j,mode);
	assert(nDayCounts > 0);
	int  nDayWidth = (width - SCALE_RECT_WIDTH) / nDayCounts;
	time_t   tTodayZeroTime = GetTodayZeroTime();
	time_t   tFirstDayZeroTime = tTodayZeroTime - 3600 * 24 * (nDayCounts - 1);

	// �����ӵķָ���
	DrawDaySplit(hDC, nDayCounts, rectScale, nDayWidth, nMaxY, nCelsiusCount, nHeightPerCelsius, tFirstDayZeroTime);
	
	// ��Ч����
	RECT rValid;
	rValid.left = rectScale.right;
	rValid.right = rectScale.left + width - 1;
	rValid.top = rectScale.top;
	rValid.bottom = rectScale.bottom;

	float fSecondsPerPixel = 0.0f;
	POINT  top_left;

	// ������
	if (nDayWidth > 0) {
		fSecondsPerPixel = (3600 * 24.0f) / (float)nDayWidth;
		top_left.x = rectScale.right;
		top_left.y = nMaxY;
		DrawPolyline(tFirstDayZeroTime, -1, fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, 
			top_left, graphics, TRUE, i,j,mode);

		// ��ע��
		DrawRemark(hDC, graphics, tFirstDayZeroTime, fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, top_left, i, j, mode, rValid);
	}
	
	// ���λ��
	POINT cursor_point;
	GetCursorPos(&cursor_point);
	::ScreenToClient(g_data.m_hWnd, &cursor_point);

	// ��ʮ����
	if (nDayWidth > 0) {
		top_left.x = rect.left + SCALE_RECT_WIDTH;
		top_left.y = nMaxY;
		DrawCrossLine(hDC, rValid, cursor_point, tFirstDayZeroTime, fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, top_left);
	}		
}

// ���single day����ʼʱ��ͽ���ʱ��
BOOL  CMyImageUI::GetSingleDayTimeRange(time_t & start, time_t & end, DWORD i, DWORD j, CModeButton::Mode mode) {
	time_t   tTodayZeroTime = GetTodayZeroTime();
	start = tTodayZeroTime + 3600 * 24 * m_nSingleDayIndex;
	end   = start + 3600 * 24 - 1;
	BOOL  bRet = FALSE;

	if (mode == CModeButton::Mode_Hand) {
		const std::vector<TempItem * > & v = GetTempData(0);
		bRet = GetTimeRange(v, start, end);
		return bRet;
	}
	else if (mode == CModeButton::Mode_Single) {
		const std::vector<TempItem * > & v = GetTempData(1);
		bRet = GetTimeRange(v, start, end);
		return bRet;
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
			BOOL b = GetTimeRange(v, t1, t2);
			if ( !b ) {
				continue;
			}

			bRet = TRUE;
			if ( bFirst ) {
				tmp_s = t1;
				tmp_e = t2;
				bFirst = FALSE;
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

		if (bRet) {
			start = tmp_s;
			end = tmp_e;
		}

		return bRet;
	}
}

BOOL  CMyImageUI::GetTimeRange(const std::vector<TempItem * > & v, time_t & start, time_t & end) {
	std::vector<TempItem * >::const_iterator it;
	std::vector<TempItem * >::const_reverse_iterator itr;

	for (it = v.begin(); it != v.end(); ++it) {
		TempItem * pItem = *it;
		// ���������time����
		if ( pItem->m_time > end ) {
			return FALSE;
		}
		if (pItem->m_time >= start) {
			start = pItem->m_time;
			break;
		}
	}

	// û���ҵ�һ������[start,end]��Χ�ڵĵ�
	if (it == v.end()) {
		return FALSE;
	}

	for (itr = v.rbegin(); itr != v.rend(); ++itr) {
		TempItem * pItem = *itr;
		if (pItem->m_time < start) {
			return FALSE;
		}
		if (pItem->m_time <= end) {
			end = pItem->m_time;
			break;
		}
	}

	if ( itr == v.rend() ) {
		return FALSE;
	}

	return TRUE;
}

// ��ʱ���ı�
void    CMyImageUI::DrawTimeText( HDC hDC, time_t  tFirstTime , time_t tLastTime, 
	                              float fSecondsPerPixel, POINT  top_left, const RECT & rValid ) {
	char szTime[256];
	::SetTextColor(hDC, RGB(255, 255, 255));
	const int OFFSET_Y = 5;
	const int TIME_TEXT_INTERVAL = 80;
	const int TIME_TEXT_WIDTH = 60;
	BOOL  bFirst = TRUE;

	int  remainder = tFirstTime % 60;
	time_t  time_point = tFirstTime + (60 - remainder);
	int  time_width = (int)( (time_point - tFirstTime) / fSecondsPerPixel );
	// �������㹻�󣬿�ʼ��text
	if (time_width >= TIME_TEXT_INTERVAL / 2) {
		Time2String(szTime, sizeof(szTime), &time_point);
		//::TextOut(hDC, top_left.x + time_width - TIME_TEXT_WIDTH / 2, top_left.y + OFFSET_Y, szTime, strlen(szTime));
		::ExtTextOut(hDC, top_left.x + time_width - TIME_TEXT_WIDTH / 2, top_left.y + OFFSET_Y, ETO_CLIPPED, &rValid, szTime, strlen(szTime), 0);
		bFirst = FALSE;
	}
	
	
	int last_time_width = time_width;
	time_point += 60;	
	while (time_point <= tLastTime) {
		time_width = (int)((time_point - tFirstTime) / fSecondsPerPixel);
		if (bFirst) {
			// �������㹻�󣬿�ʼ��text
			if ((time_width - last_time_width) >= TIME_TEXT_INTERVAL / 2) {
				Time2String(szTime, sizeof(szTime), &time_point);
				//::TextOut(hDC, top_left.x + time_width - TIME_TEXT_WIDTH / 2, top_left.y + OFFSET_Y, szTime, strlen(szTime));
				::ExtTextOut(hDC, top_left.x + time_width - TIME_TEXT_WIDTH / 2, top_left.y + OFFSET_Y, ETO_CLIPPED, &rValid, szTime, strlen(szTime), 0);
				last_time_width = time_width;
				bFirst = FALSE;
			}
		}
		else {
			// �������㹻�󣬿�ʼ��text
			if ((time_width - last_time_width) >= TIME_TEXT_INTERVAL ) {
				Time2String(szTime, sizeof(szTime), &time_point);
				//::TextOut(hDC, top_left.x + time_width - TIME_TEXT_WIDTH / 2, top_left.y + OFFSET_Y, szTime, strlen(szTime));
				::ExtTextOut(hDC, top_left.x + time_width - TIME_TEXT_WIDTH / 2, top_left.y + OFFSET_Y, ETO_CLIPPED, &rValid, szTime, strlen(szTime), 0);
				last_time_width = time_width;
			}
		}
		

		time_point += 60;
	}
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
	// ˮƽ������λ��
	int  nScrollX = GetMyScrollX();

	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();	

	// ��ʾ������¶Ⱥ�����¶�
	int  nMinTemp, nMaxTemp;
	GetMaxMinShowTemp(nMinTemp, nMaxTemp, i, j, mode);
	// ���϶ȸ���
	int  nCelsiusCount = nMaxTemp - nMinTemp;
	// ÿ�����϶ȵĸ߶�
	int  nHeightPerCelsius = GetCelsiusHeight(height, nCelsiusCount);
	// ��ֱ����
	int  nVMargin = (height - nHeightPerCelsius * nCelsiusCount) / 2;
	// ����¶ȵ�Y����ϵֵ
	int  nMaxY = rect.top + nVMargin;

	// ȫͼ��Ϊ��߿̶�������ұ�����ͼ
	RECT rectScale;
	rectScale.left = rect.left + nScrollX;
	rectScale.top = rect.top;
	rectScale.right = rectScale.left + SCALE_RECT_WIDTH;
	rectScale.bottom = rect.bottom;


	// ��ˮƽ�̶���
	DrawScaleLine(hDC, nCelsiusCount, nHeightPerCelsius, nMaxY, rectScale, rect);

	// ���߿�
	DrawBorder(hDC, rectScale, width);	

	// ��������
	DrawWarning(hDC, i, j, nMaxTemp, nHeightPerCelsius, nMaxY, rectScale, width);

	// �鿴��������
	int nPointsCnt = GetTempCount(i, j, mode);
	// ���û�����ݾͲ��ػ��� 
	if ( nPointsCnt > 0 ) {
		// ���¶�����
		POINT  top_left;
		top_left.x = rect.left + SCALE_RECT_WIDTH;
		top_left.y = nMaxY;

		time_t  tFirstTime = 0, tLastTime = 0;
		BOOL bRet = GetSingleDayTimeRange(tFirstTime, tLastTime, i, j, mode);

		// ����е���Ի�
		if (bRet) {
			DrawPolyline(tFirstTime, tLastTime, m_fSecondsPerPixel, nMaxTemp, nHeightPerCelsius,
				top_left, graphics, TRUE, i, j, mode);

			// ��Ч����
			RECT rValid;
			rValid.left = rectScale.right;
			rValid.right = rectScale.left + width - 1;
			rValid.top = rectScale.top;
			rValid.bottom = rectScale.bottom;

			top_left.y += nHeightPerCelsius * nCelsiusCount;
			// ��ʱ���ı�
			DrawTimeText(hDC, tFirstTime, tLastTime, m_fSecondsPerPixel, top_left, rValid);			

			// ��ע��
			top_left.y = nMaxY;
			DrawRemark(hDC, graphics, tFirstTime, m_fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, top_left, i,j, mode, rValid);

			// ���λ��
			POINT cursor_point;
			GetCursorPos(&cursor_point);
			::ScreenToClient(g_data.m_hWnd, &cursor_point);			

			// ��ʮ����
			DrawCrossLine(hDC, rValid, cursor_point, tFirstTime, m_fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, top_left);
		}		
	}	

	// ���̶�ֵ
	DrawScale(hDC, nCelsiusCount, nHeightPerCelsius, nMaxY, nMaxTemp, rectScale, width);
}

void CMyImageUI::MyInvalidate() {
	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();

	if ( m_state == STATE_SINGLE_DAY ) {
		time_t  tFirstTime = 0, tLastTime = 0;
		BOOL bRet = GetSingleDayTimeRange(tFirstTime, tLastTime, i, j, mode);
		int  width = GetMyWidth();

		if ( !m_bSetSecondsPerPixel ) {
			if (bRet) {
				m_fSecondsPerPixel = (float)(tLastTime - tFirstTime) / (width - SCALE_RECT_WIDTH);
				if (m_fSecondsPerPixel < MIN_SECONDS_PER_PIXEL) {
					m_fSecondsPerPixel = MIN_SECONDS_PER_PIXEL;
				}
				else if (m_fSecondsPerPixel > MAX_SECONDS_PER_PIXEL) {
					m_fSecondsPerPixel = MAX_SECONDS_PER_PIXEL;
				}
				m_bSetSecondsPerPixel = TRUE;
			}			
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

// ˫���¼�
void  CMyImageUI::OnDbClick() {
	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();

	// �鿴��������
	int nPointsCnt = GetTempCount(i,j,mode);
	
	// ���û�����ݾͲ��ػ��� 
	if (0 == nPointsCnt) {
		return;
	}


	// �����7����ͼ
	if (m_state == STATE_7_DAYS) {
		// ���������һ��
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

// �¶����ݸ���
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

// ����˫���˵ڼ���
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

	// ƫ����
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

// ���ֻ���
void   CMyImageUI::OnMyMouseWheel(WPARAM wParam, LPARAM lParam) {
	if (!m_bSetSecondsPerPixel)
		return;

	int nDirectory = (int)wParam;
	if (nDirectory > 0) {
		m_fSecondsPerPixel *= 1.2f;
	}
	else {
		m_fSecondsPerPixel /= 1.2f;
	}

	if ( m_fSecondsPerPixel < MIN_SECONDS_PER_PIXEL) {
		m_fSecondsPerPixel = MIN_SECONDS_PER_PIXEL;
	}
	else if (m_fSecondsPerPixel > MAX_SECONDS_PER_PIXEL) {
		m_fSecondsPerPixel = MAX_SECONDS_PER_PIXEL;
	}
	MyInvalidate();
}

// �õ�������С��ʾ�¶�
void   CMyImageUI::GetMaxMinShowTemp(int & nMinTemp, int & nMaxTemp, DWORD i, DWORD j, CModeButton::Mode mode) {
	nMinTemp = 3400;
	nMaxTemp = 3900;
	BOOL bFirst = TRUE;

	if (mode == CModeButton::Mode_Hand) {
		const std::vector<TempItem * > & v = GetTempData(0);
		GetMaxMinShowTemp(nMinTemp, nMaxTemp,bFirst, v);
	}
	else if (mode == CModeButton::Mode_Single) {
		const std::vector<TempItem * > & v = GetTempData(1);
		GetMaxMinShowTemp(nMinTemp, nMaxTemp, bFirst, v);
	}
	else {
		for (DWORD k = 0; k < MAX_READERS_PER_GRID; k++) {
			const std::vector<TempItem * > & v = GetTempData(k + 1);
			GetMaxMinShowTemp(nMinTemp, nMaxTemp, bFirst, v);
		}
	}

	// ������С��ʾ�¶ȱ���Ϊ����
	// min: 2100~2199 ==> 2100
	// max: 3800 ==> 3800, 3801~3899 ==> 3900
	nMinTemp = nMinTemp / 100;
	int nReminder = nMaxTemp % 100;
	if ( 0 != nReminder ) {
		nMaxTemp = (nMaxTemp / 100 + 1);
	}
	else {
		nMaxTemp /= 100;
	}

	if (nMinTemp == nMaxTemp) {
		nMaxTemp = nMinTemp + 1;
	}

}

void  CMyImageUI::GetMaxMinShowTemp(int & nMinTemp, int & nMaxTemp, BOOL & bFirst, const std::vector<TempItem * > & v) {
	std::vector<TempItem * >::const_iterator it;
	for (it = v.begin(); it != v.end(); ++it) {
		TempItem * pItem = *it;
		if ( bFirst ) {
			nMaxTemp = pItem->m_dwTemp;
			nMinTemp = pItem->m_dwTemp;
			bFirst = FALSE;
		}
		else {
			if ((int)pItem->m_dwTemp > nMaxTemp) {
				nMaxTemp = pItem->m_dwTemp;
			}
			else if ((int)pItem->m_dwTemp < nMinTemp) {
				nMinTemp = pItem->m_dwTemp;
			}
		}		
	}
}

// ��ʮ����
void   CMyImageUI::DrawCrossLine( HDC hDC, const RECT & rValid, const POINT & cursor_point, 
	                              time_t tFirstTime, float fSecondsPerPixel, int nMaxTemp, int nHeightPerCelsius, 
	                              POINT  top_left ) {
	CDuiString strText;

	// ������㻭ʮ���ߵ�Ҫ��
	if ( g_data.m_CfgData.m_bCrossAnchor && ::PtInRect(&rValid, cursor_point)) {

		::SelectObject(hDC, m_hCommonThreadPen);
		::MoveToEx(hDC, cursor_point.x, rValid.top, 0);
		::LineTo(hDC, cursor_point.x, rValid.bottom);
		::MoveToEx(hDC, rValid.left, cursor_point.y, 0);
		::LineTo(hDC, rValid.right, cursor_point.y);

		float  cursot_temp = nMaxTemp + (float)( top_left.y - cursor_point.y ) / nHeightPerCelsius;
		time_t cursor_time = (time_t)((float)(cursor_point.x - top_left.x) * fSecondsPerPixel) + tFirstTime;
		
		char szTime[256];
		if ( m_state == STATE_7_DAYS )
			Time2String_hm(szTime, sizeof(szTime), &cursor_time);
		else
			Time2String(szTime, sizeof(szTime), &cursor_time);

		strText.Format("%.2f��,%s", cursot_temp, szTime);
		
		// �ж�������ߣ������ұ�
		if (rValid.right - cursor_point.x >= 120)
			::TextOut(hDC, cursor_point.x + 5, cursor_point.y - 20, strText, strText.GetLength());
		else
			::TextOut(hDC, cursor_point.x - 120, cursor_point.y - 20, strText, strText.GetLength());
	}
}

// Ϊlabel����
void   CMyImageUI::PaintForLabelUI(HDC hDC, int width, int height, const RECT & rect) {
	DuiLib::CDuiString strText;

	// GDI+
	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	// ˮƽ������λ��
	int  nScrollX = 0;

	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();

	// ��ʾ������¶Ⱥ�����¶�
	int  nMinTemp, nMaxTemp;
	GetMaxMinShowTemp(nMinTemp, nMaxTemp, i, j, mode);
	// ���϶ȸ���
	int  nCelsiusCount = nMaxTemp - nMinTemp;
	// ÿ�����϶ȵĸ߶�
	int  nHeightPerCelsius = GetCelsiusHeight(height, nCelsiusCount);
	// ��ֱ����
	int  nVMargin = (height - nHeightPerCelsius * nCelsiusCount) / 2;
	// ����¶ȵ�Y����ϵֵ
	int  nMaxY = rect.top + nVMargin;

	// ȫͼ��Ϊ��߿̶�������ұ�����ͼ
	RECT rectScale;
	rectScale.left   = rect.left + nScrollX;
	rectScale.top    = rect.top;
	rectScale.right  = rectScale.left;
	rectScale.bottom = rect.bottom;

	// ��ˮƽ�̶���
	DrawScaleLine(hDC, nCelsiusCount, nHeightPerCelsius, nMaxY, rectScale, rect);

	// ���߿�
	//DrawBorder(hDC, rectScale, width);

	// ���̶�ֵ
	//DrawScale(hDC, nCelsiusCount, nHeightPerCelsius, nMaxY, nMaxTemp, rectScale, width);

	// ��������
	//DrawWarning(hDC, i, j, nMaxTemp, nHeightPerCelsius, nMaxY, rectScale, width);

	// �����¶����߿�Խ��������
	int  nDayCounts = GetDayCounts(i, j, mode);
	assert(nDayCounts > 0);
	int  nDayWidth = (width - SCALE_RECT_WIDTH) / nDayCounts;
	time_t   tTodayZeroTime = GetTodayZeroTime();
	time_t   tFirstDayZeroTime = tTodayZeroTime - 3600 * 24 * (nDayCounts - 1);

	// �����ӵķָ���
	//DrawDaySplit(hDC, nDayCounts, rectScale, nDayWidth, nMaxY, nCelsiusCount, nHeightPerCelsius, tFirstDayZeroTime);

	float fSecondsPerPixel = 0.0f;
	// ������
	if (nDayWidth > 0) {
		fSecondsPerPixel = (3600 * 24.0f) / (float)nDayWidth;
		POINT  top_left;
		top_left.x = rectScale.right;
		top_left.y = nMaxY;
		DrawPolyline(tFirstDayZeroTime, -1, fSecondsPerPixel, nMaxTemp, nHeightPerCelsius,
			top_left, graphics, TRUE, i, j, mode);
	}
}

// ��ʼ�ϷŲ���
void  CMyImageUI::BeginDragDrop() {
	m_bDragDrop = TRUE;
	m_DragDropObj = m_CursorObj;
	SetCursor(g_data.m_hCursor);
	m_CursorObj = DragDrop_None;
}

// �������Ƿ���Ҫ�ı�ָ��
void  CMyImageUI::CheckCursor(const POINT & pt) {

	// self rectangle and width, height
	RECT rect = GetPos();
	int  width = GetMyWidth();
	int  height = rect.bottom - rect.top;
	// ˮƽ������λ��
	int  nScrollX = GetMyScrollX();

	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();

	// ��ʾ������¶Ⱥ�����¶�
	int  nMinTemp, nMaxTemp;
	GetMaxMinShowTemp(nMinTemp, nMaxTemp, i, j, mode);
	// ���϶ȸ���
	int  nCelsiusCount = nMaxTemp - nMinTemp;
	// ÿ�����϶ȵĸ߶�
	int  nHeightPerCelsius = GetCelsiusHeight(height, nCelsiusCount);
	// ��ֱ����
	int  nVMargin = (height - nHeightPerCelsius * nCelsiusCount) / 2;
	// ����¶ȵ�Y����ϵֵ
	int  nMaxY = rect.top + nVMargin;

	// ȫͼ��Ϊ��߿̶�������ұ�����ͼ
	RECT rectScale;
	rectScale.left = rect.left + nScrollX;
	rectScale.top = rect.top;
	rectScale.right = rectScale.left + SCALE_RECT_WIDTH;
	rectScale.bottom = rect.bottom;

	// ���¡����±����ߵ�y����
	int  nY = 0;
	DWORD  dwLowAlarmTemp = 0;
	DWORD  dwHighAlarmTemp = 0;

	if ( mode == CModeButton::Mode_Hand ) {
		dwLowAlarmTemp  = g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm;
		dwHighAlarmTemp = g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm;
	}
	else {
		dwLowAlarmTemp = g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j - 1].m_dwLowTempAlarm;
		dwHighAlarmTemp = g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j - 1].m_dwHighTempAlarm;
	}

	// ���µ�yֵ
	nY = (int)((nMaxTemp - dwLowAlarmTemp / 100.0) * nHeightPerCelsius) + nMaxY;
	int nMargin = 3;

	RECT  r;
	r.left   = rectScale.right;
	r.right  = rectScale.left + width;
	r.top    = nY - nMargin;
	r.bottom = nY + nMargin;

	// �������ڵ��¾�����
	if ( PtInRect(&r, pt) ) {
		m_CursorObj = DragDrop_LowAlarm;
		SetCursor(g_data.m_hCursor);
		this->Invalidate();
	}
	else {
		nY = (int)((nMaxTemp - dwHighAlarmTemp / 100.0) * nHeightPerCelsius) + nMaxY;
		r.top = nY - nMargin;
		r.bottom = nY + nMargin;
		if (PtInRect(&r, pt)) {
			m_CursorObj = DragDrop_HighAlarm;
			SetCursor(g_data.m_hCursor);
			this->Invalidate();
		}
		else {
			m_CursorObj = DragDrop_None;
		}
	}
	
	//DrawWarning(hDC, i, j, nMaxTemp, nHeightPerCelsius, nMaxY, rectScale, width);
}

// �����ϷŲ���
void  CMyImageUI::DragDropIng(const POINT & pt) {
	if ( !g_data.m_DragDropCtl->IsVisible() ) {
		g_data.m_DragDropCtl->SetVisible(true);
	}

	// self rectangle and width, height
	RECT rect = GetPos();
	int  width = GetMyWidth();
	int  height = rect.bottom - rect.top;
	// ˮƽ������λ��
	int  nScrollX = GetMyScrollX();

	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();

	// ��ʾ������¶Ⱥ�����¶�
	int  nMinTemp, nMaxTemp;
	GetMaxMinShowTemp(nMinTemp, nMaxTemp, i, j, mode);
	// ���϶ȸ���
	int  nCelsiusCount = nMaxTemp - nMinTemp;
	// ÿ�����϶ȵĸ߶�
	int  nHeightPerCelsius = GetCelsiusHeight(height, nCelsiusCount);
	// ��ֱ����
	int  nVMargin = (height - nHeightPerCelsius * nCelsiusCount) / 2;
	// ����¶ȵ�Y����ϵֵ
	int  nMaxY = rect.top + nVMargin;

	// ȫͼ��Ϊ��߿̶�������ұ�����ͼ
	RECT rectScale;
	rectScale.left = rect.left + nScrollX;
	rectScale.top = rect.top;
	rectScale.right = rectScale.left + SCALE_RECT_WIDTH;
	rectScale.bottom = rect.bottom;

	RECT r;
	r.left   = rectScale.right;
	r.right  = rectScale.left + width;
	r.top    = pt.y - 4;
	r.bottom = r.top + 4 * 2;

	if (r.top < rectScale.top) {
		r.top = rectScale.top;
		r.bottom = r.top + 4 * 2;
	}
	else if (r.bottom > rectScale.bottom) {
		r.bottom = rectScale.bottom;
		r.top = r.bottom - 4 * 2;
	}

	g_data.m_DragDropCtl->SetPos(r);
	SetCursor(g_data.m_hCursor);
}

// �����ϷŲ���
void  CMyImageUI::EndDragDrop(const POINT & pt) {
	// self rectangle and width, height
	RECT rect = GetPos();
	int  width = GetMyWidth();
	int  height = rect.bottom - rect.top;
	// ˮƽ������λ��
	int  nScrollX = GetMyScrollX();

	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();

	// ��ʾ������¶Ⱥ�����¶�
	int  nMinTemp, nMaxTemp;
	GetMaxMinShowTemp(nMinTemp, nMaxTemp, i, j, mode);
	// ���϶ȸ���
	int  nCelsiusCount = nMaxTemp - nMinTemp;
	// ÿ�����϶ȵĸ߶�
	int  nHeightPerCelsius = GetCelsiusHeight(height, nCelsiusCount);
	// ��ֱ����
	int  nVMargin = (height - nHeightPerCelsius * nCelsiusCount) / 2;
	// ����¶ȵ�Y����ϵֵ
	int  nMaxY = rect.top + nVMargin;

	// ȫͼ��Ϊ��߿̶�������ұ�����ͼ
	RECT rectScale;
	rectScale.left = rect.left + nScrollX;
	rectScale.top = rect.top;
	rectScale.right = rectScale.left + SCALE_RECT_WIDTH;
	rectScale.bottom = rect.bottom;

	// �����µĸ��¡����±���
	DWORD  dwAlarm = (DWORD)( ( nMaxTemp + (double)(nMaxY - pt.y) / nHeightPerCelsius ) * 100.0 );
	if ( m_DragDropObj == DragDrop_LowAlarm ) {
		if ( mode == CModeButton::Mode_Hand ) {
			g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm = dwAlarm;
		}
		else {
			g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j - 1].m_dwLowTempAlarm = dwAlarm;
		}
		SaveAlarmTemp(i, j, mode);
	}
	else if ( m_DragDropObj == DragDrop_HighAlarm ) {
		if (mode == CModeButton::Mode_Hand) {
			g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm = dwAlarm;
		}
		else {
			g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j - 1].m_dwHighTempAlarm = dwAlarm;
		}
		SaveAlarmTemp(i, j - 1, mode);
	}	

	g_data.m_DragDropCtl->SetVisible(false);
	m_bDragDrop = FALSE;
	m_DragDropObj = DragDrop_None;
	m_CursorObj = DragDrop_None;

	this->Invalidate();
}

// ����Ƿ�����ע��
void  CMyImageUI::CheckRemark(const POINT & pt) {
	if (m_state == STATE_7_DAYS) {
		return;
	}

	if (!m_bSetSecondsPerPixel) {
		return;
	}

	if (m_fSecondsPerPixel > 6.0f) {
		return;
	}

	// self rectangle and width, height
	RECT rect   = GetPos();
	int  width  = GetMyWidth();
	int  height = rect.bottom - rect.top;
	// ˮƽ������λ��
	int  nScrollX = GetMyScrollX();

	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();

	// ��ʾ������¶Ⱥ�����¶�
	int  nMinTemp, nMaxTemp;
	GetMaxMinShowTemp(nMinTemp, nMaxTemp, i, j, mode);
	// ���϶ȸ���
	int  nCelsiusCount = nMaxTemp - nMinTemp;
	// ÿ�����϶ȵĸ߶�
	int  nHeightPerCelsius = GetCelsiusHeight(height, nCelsiusCount);
	// ��ֱ����
	int  nVMargin = (height - nHeightPerCelsius * nCelsiusCount) / 2;
	// ����¶ȵ�Y����ϵֵ
	int  nMaxY = rect.top + nVMargin;

	// ȫͼ��Ϊ��߿̶�������ұ�����ͼ
	RECT rectScale;
	rectScale.left = rect.left + nScrollX;
	rectScale.top = rect.top;
	rectScale.right = rectScale.left + SCALE_RECT_WIDTH;
	rectScale.bottom = rect.bottom;

	// �鿴��������
	int nPointsCnt = GetTempCount(i, j, mode);
	// ���û�����ݣ�����
	if (nPointsCnt <= 0) {
		return;
	}

	time_t  tFirstTime = 0, tLastTime = 0;
	BOOL bRet = GetSingleDayTimeRange(tFirstTime, tLastTime, i, j, mode);
	if (!bRet) {
		return;
	}

	// ��Ч����
	RECT rValid;
	rValid.left   = rectScale.right;
	rValid.right  = rectScale.left + width - 1;
	rValid.top    = rectScale.top;
	rValid.bottom = rectScale.bottom;

	// ������������߷�Χ
	if ( !PtInRect(&rValid, pt) ) {
		return;
	}

	POINT  top_left;
	top_left.x = rect.left + SCALE_RECT_WIDTH;
	top_left.y = nMaxY;

	if (mode == CModeButton::Mode_Hand) {
		const std::vector<TempItem * > & v = GetTempData(0);
		CheckRemark(pt, v, tFirstTime, m_fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, top_left);
	}
	else if (mode == CModeButton::Mode_Single) {
		const std::vector<TempItem * > & v = GetTempData(1);
		CheckRemark(pt, v, tFirstTime, m_fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, top_left);
	}
	else {
		for (DWORD k = 0; k < MAX_READERS_PER_GRID; k++) {
			const std::vector<TempItem * > & v = GetTempData(k + 1);
			bRet = CheckRemark(pt, v, tFirstTime, m_fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, top_left);
			if ( bRet ) {
				break;
			}
		}
	}
}

BOOL  CMyImageUI::CheckRemark( const POINT & pt, const std::vector<TempItem * > & v,
	                           time_t tFirstTime, float fSecondsPerPixel,
	                           int nMaxTemp, int nHeightPerCelsius, POINT  top_left ) {

	std::vector<TempItem * >::const_iterator it;
	for (it = v.begin(); it != v.end(); ++it) {
		TempItem * pItem = *it;

		int nX = (int)((pItem->m_time - tFirstTime) / fSecondsPerPixel + top_left.x);
		// ����Ĳ��ü����
		if ( nX > pt.x + 3 ) {
			break;
		}
		
		if (nX < pt.x - 3) {
			continue;
		}

		int nY = (int)((nMaxTemp - pItem->m_dwTemp / 100.0) * nHeightPerCelsius + top_left.y);
		if (nY >= pt.y - 3 && nY <= pt.y + 3) {

			// ���֮ǰ�ڱ༭״̬
			if ( g_data.m_edRemark->IsVisible()) {
				::OnEdtRemarkKillFocus();
			}

			m_dwRemarkingIndex = pItem->m_dwDbId;

			RECT rectRemark;
			rectRemark.left   = nX - EDT_REMARK_WIDTH / 2;
			rectRemark.top    = nY - EDT_REMARK_HEIGHT - EDT_REMARK_Y_OFFSET;
			rectRemark.right  = rectRemark.left + EDT_REMARK_WIDTH;
			rectRemark.bottom = rectRemark.top + EDT_REMARK_HEIGHT;
			g_data.m_edRemark->SetTag((UINT_PTR)this);
			g_data.m_edRemark->SetPos(rectRemark);
			g_data.m_edRemark->SetText(pItem->m_szRemark);
			g_data.m_edRemark->SetVisible(true);
			g_data.m_edRemark->SetFocus();

			return TRUE;
		}
	}

	return FALSE;
}

void  CMyImageUI::SetRemark(DuiLib::CDuiString & strRemark) {

	// (�����п������ݿ����㹻��ѭ����һ��)
	if (m_dwRemarkingIndex == 0) {
		return;
	}

	DWORD  i = GetGridIndex();
	DWORD  j = GetReaderIndex();
	CModeButton::Mode mode = GetMode();

	if (mode == CModeButton::Mode_Hand) {
		const std::vector<TempItem * > & v = GetTempData(0);
		SetRemark(v, m_dwRemarkingIndex, strRemark);
	}
	else if (mode == CModeButton::Mode_Single) {
		const std::vector<TempItem * > & v = GetTempData(1);
		SetRemark(v, m_dwRemarkingIndex, strRemark);
	}
	else {
		BOOL bRet = FALSE;
		for (DWORD k = 0; k < MAX_READERS_PER_GRID; k++) {
			const std::vector<TempItem * > & v = GetTempData(k + 1);
			bRet = SetRemark(v, m_dwRemarkingIndex, strRemark);
			if (bRet) {
				break;
			}
		}
	}
}

// ����ע��
BOOL  CMyImageUI::SetRemark(const std::vector<TempItem * > & v, DWORD  dwDbId, DuiLib::CDuiString & strRemark) {
	std::vector<TempItem * >::const_iterator it;
	for (it = v.begin(); it != v.end(); ++it) {
		TempItem * pItem = *it;
		if (pItem->m_dwDbId == dwDbId) {
			STRNCPY(pItem->m_szRemark, strRemark, MAX_REMARK_LENGTH);
			CBusiness::GetInstance()->SaveRemarkAsyn(pItem->m_dwDbId, pItem->m_szRemark);
			return TRUE;
		}
	}

	return FALSE;
}

// ��ע��
void  CMyImageUI::DrawRemark( HDC hDC, Graphics & g, time_t tFirstTime, float fSecondsPerPixel,
	                          int nMaxTemp, int nHeightPerCelsius, POINT  top_left, 
	                          DWORD i, DWORD j, CModeButton::Mode mode, const RECT & rValid) {

	::SetBkMode(hDC, TRANSPARENT);

	if (mode == CModeButton::Mode_Hand) {
		const std::vector<TempItem * > & v = GetTempData(0);
		DrawRemark(hDC, g, tFirstTime, fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, top_left, v, rValid);
	}
	else if (mode == CModeButton::Mode_Single) {
		const std::vector<TempItem * > & v = GetTempData(1);
		DrawRemark(hDC, g, tFirstTime, fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, top_left, v, rValid);
	}
	else {
		for (DWORD k = 0; k < MAX_READERS_PER_GRID; k++) {
			const std::vector<TempItem * > & v = GetTempData(k + 1);
			DrawRemark(hDC, g, tFirstTime, fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, top_left, v, rValid);
		}
	}
}

void  CMyImageUI::DrawRemark( HDC hDC, Graphics & g, time_t tFirstTime, float fSecondsPerPixel,
	                          int nMaxTemp, int nHeightPerCelsius, POINT  top_left,
	                          const std::vector<TempItem * > & v, const RECT & rValid) {
	int nX = 0;
	int nY = 0;
	CDuiString  strText;

	std::vector<TempItem * >::const_iterator it;
	for ( it = v.begin(); it != v.end(); ++it ) {
		TempItem * pItem = *it;
		if ( pItem->m_szRemark[0] == '\0') {
			continue;
		}

		nX = (int)( (pItem->m_time - tFirstTime) / fSecondsPerPixel ) + top_left.x;
		nY = (int)( (nMaxTemp - pItem->m_dwTemp / 100.0 ) * nHeightPerCelsius ) + top_left.y;

		CGraphicsRoundRectPath  RoundRectPath;
		RoundRectPath.AddRoundRect( nX - EDT_REMARK_WIDTH / 2, nY - EDT_REMARK_HEIGHT - EDT_REMARK_Y_OFFSET,
			                        EDT_REMARK_WIDTH, EDT_REMARK_HEIGHT, 5, 5 );
		g.DrawPath(m_remark_pen, &RoundRectPath);
		g.FillPath(m_remark_brush, &RoundRectPath);

		strText = pItem->m_szRemark;
		RECT rectRemark;
		rectRemark.left   = nX - EDT_REMARK_WIDTH / 2;
		rectRemark.top    = nY - EDT_REMARK_HEIGHT - EDT_REMARK_Y_OFFSET;
		rectRemark.right  = rectRemark.left + EDT_REMARK_WIDTH;
		rectRemark.bottom = rectRemark.top + EDT_REMARK_HEIGHT;

		RECT rCalc = rectRemark;
		int h = ::DrawText(hDC, strText, strText.GetLength(), &rCalc, DT_CALCRECT );
		int w = rCalc.right - rCalc.left; 
		 
		::ExtTextOut(hDC, rectRemark.left + EDT_REMARK_WIDTH / 2 - w / 2, rectRemark.top + EDT_REMARK_HEIGHT / 2 - h / 2, ETO_CLIPPED, &rValid, strText, strText.GetLength(), 0);
	}
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
	int width  = pParent->GetWidth();
	int height = pParent->GetHeight();
	RECT     r = this->GetPos();

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



CMyHandImage::CMyHandImage() {

}

CMyHandImage::~CMyHandImage() {
	Clear();
}

void CMyHandImage::Clear() {
	std::map<std::string, vector<TempItem *> *>::iterator it;
	for ( it = m_data.begin(); it != m_data.end(); ++it ) {
		assert(it->second);
		if (it->second == 0) {
			continue;
		}

		ClearVector(*it->second);
		delete it->second;
	}

	m_data.clear();
}

void CMyHandImage::DoPaint_7Days(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {

}

void CMyHandImage::DoPaint_SingleDay(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {

}

void  CMyHandImage::CheckCursor(const POINT & pt) {

}

// ˫���¼�
void   CMyHandImage::OnDbClick() {

}

DWORD  CMyHandImage::GetGridIndex() {
	assert(0);
	return -1;
}

DWORD  CMyHandImage::GetReaderIndex() {
	assert(0);
	return -1;
}

CModeButton::Mode   CMyHandImage::GetMode() {
	assert(0);
	return (CModeButton::Mode)-1;
}

void  CMyHandImage::OnHandTemp(TempItem * pTemp, BOOL & bNewTag) {
	bNewTag = FALSE;
	vector<TempItem *> * pVec = 0;

	pVec = m_data[pTemp->m_szTagId];
	// ���û�м��ϵ�
	if ( 0 == pVec ) {
		pVec = new vector<TempItem *>;
		m_data[pTemp->m_szTagId] = pVec;
		bNewTag = TRUE;
	}

	pVec->push_back(pTemp);
}

void  CMyHandImage::OnHandTempVec(vector<TempItem *> * pNew, const char * szTagId) {
	assert(szTagId);

	vector<TempItem *> * pVec = 0;
	pVec = m_data[szTagId];
	assert(pVec == 0);

	m_data[szTagId] = pNew;
}

// ɾ����ʱ������
void  CMyHandImage::PruneData() {

}