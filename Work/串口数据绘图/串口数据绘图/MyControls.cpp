#include "MyControls.h"
#include "LmnTemplates.h"

#define  BUTT_WIDTH             10
#define  VERTICAL_SCALE_COUNT   10
#define  MAX_POINTS_INTERVAL    40
#define  POINT_RADIUS           3
#define  SCALE_HEIGHT           10
#define  HORIZONTAL_SCALE_INTERVAL   80
#define  SCALE_FONT_HEIGHT      20
#define  SCALE_WIDTH            10
#define  VERTICAL_SCALE_INTERVAL   80

CMyChartUI::CMyChartUI() : m_scale_pen( Gdiplus::Color(0xFF1b9375) ), 
                           m_brush(Gdiplus::Color(0xFFC9E2F2))  {

	m_rcMargin.left   = 80;
	m_rcMargin.bottom = 50;
	m_rcMargin.right  = 30;
	m_rcMargin.top    = 30;

	m_fPos = 0.0f;
	m_fLength = 1.0f;
	m_fVPos = 0.0f;
	m_fVLength = 1.0f;
}

CMyChartUI::~CMyChartUI() {
	Clear();
}

void CMyChartUI::Clear() {
	std::map<int, CChannel *>::iterator it;
	for (it = m_data.begin(); it != m_data.end(); ++it) {
		delete it->second;
	}
	m_data.clear();
}

void CMyChartUI::GetParams(int & nMaxPoints, int & nMaxValue, int & nMinValue) {
	std::map<int, CChannel *>::iterator it;
	BOOL  bFirst = TRUE;
	nMaxPoints = 0;
	nMinValue = nMaxValue = 0;

	for (it = m_data.begin(); it != m_data.end(); ++it) {
		int nChannel = it->first;
		if ( !IfHasBit(g_data.m_dwChannels, nChannel) ) {
			continue;
		}
		CChannel * pChannel = it->second;
		std::vector<int>::iterator ix;		
		std::vector<int> & v = pChannel->m_vValues;
		for (ix = v.begin(); ix != v.end(); ++ix) {
			int nValue = *ix;
			if (bFirst) {
				nMaxValue = nMinValue = nValue;
				bFirst = FALSE;
			}
			else {
				if (nValue > nMaxValue) {
					nMaxValue = nValue;
				}
				else if (nValue < nMinValue) {
					nMinValue = nValue;
				}
			}
		}
		if ( (int)v.size() > nMaxPoints ) {
			nMaxPoints = v.size();
		}
	}
}
 
bool CMyChartUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	RECT pos     = GetPos();
	int  nWidth  = pos.right - pos.left;
	int  nHeight = pos.bottom - pos.top;
	int  nRealWidth  = nWidth - m_rcMargin.left - m_rcMargin.right;
	int  nRealHeight = nHeight - m_rcMargin.top - m_rcMargin.bottom;


	// 画背景
	graphics.FillRectangle(&m_brush, pos.left, pos.top, nWidth, nHeight);	

	int nMaxPointsCnt, nMaxValue, nMinValue;
	GetParams(nMaxPointsCnt, nMaxValue, nMinValue);

	float  fInterval = nRealWidth / (nMaxPointsCnt * m_fLength);
	if ( fInterval > MAX_POINTS_INTERVAL ) {
		fInterval = MAX_POINTS_INTERVAL;
	}

	int nDiffValue    = nMaxValue - nMinValue;
	float  fVInterval = 0.0f;
	if (nDiffValue > 0) {
		fVInterval = nRealHeight / (nDiffValue * 1.02f * m_fVLength);
	}		

	float fOriginX = nMaxPointsCnt * m_fPos + 0;
	float fOriginY = nDiffValue * 1.02f * m_fVPos + (nMinValue - nDiffValue * 0.01f);
	int nOriginXUi = pos.left + m_rcMargin.left;
	int nOriginYUi = pos.bottom - m_rcMargin.bottom;

	if ( nDiffValue == 0 ) {
		nOriginYUi -= nRealHeight / 2;
	}

	// 画图
	std::map<int, CChannel *>::iterator it;
	for (it = m_data.begin(); it != m_data.end(); ++it) {
		int nChannel = it->first;
		if (!IfHasBit(g_data.m_dwChannels, nChannel)) {
			continue;
		}

		CChannel * pChannel = it->second;
		DWORD  dwPointsCnt = pChannel->m_vValues.size();
		Gdiplus::Point * points = new Gdiplus::Point[dwPointsCnt];
		for (DWORD i = 0; i < dwPointsCnt; i++) {
			points[i].X = (int)((i - fOriginX) * fInterval + nOriginXUi);
			points[i].Y = (int)(nOriginYUi - (pChannel->m_vValues[i] - fOriginY) * fVInterval );
			if ( fInterval > POINT_RADIUS * 2 ) {
				graphics.FillEllipse(&pChannel->m_brush, points[i].X - POINT_RADIUS, 
					points[i].Y - POINT_RADIUS, 2 * POINT_RADIUS, 2 * POINT_RADIUS);
			}			
		}
		graphics.DrawLines(&pChannel->m_pen, points, dwPointsCnt);
		delete[] points;
	}

	// 填充空白区域
	Rect r;
	r.X = pos.left;
	r.Y = pos.bottom - m_rcMargin.bottom;
	r.Width = nWidth;
	r.Height = m_rcMargin.bottom;
	graphics.FillRectangle(&m_brush, r);

	r.X = pos.left;
	r.Y = pos.top;
	r.Width = m_rcMargin.left;
	r.Height = nHeight;
	graphics.FillRectangle(&m_brush, r);

	r.X = pos.right - m_rcMargin.right;
	r.Y = pos.top;
	r.Width = m_rcMargin.right;
	r.Height = nHeight;
	graphics.FillRectangle(&m_brush, r);

	r.X = pos.left;
	r.Y = pos.top;
	r.Width = nWidth;
	r.Height = m_rcMargin.top;
	graphics.FillRectangle(&m_brush, r);

	//画刻度线
	Gdiplus::Point  scale_points[2];
	scale_points[0].X = pos.left + m_rcMargin.left;
	scale_points[0].Y = pos.bottom - m_rcMargin.bottom;
	scale_points[1].X = pos.left + m_rcMargin.left;
	scale_points[1].Y = pos.top + m_rcMargin.top;
	graphics.DrawLines(&m_scale_pen, scale_points, 2);

	scale_points[0].X = pos.left + m_rcMargin.left;
	scale_points[0].Y = pos.bottom - m_rcMargin.bottom;
	scale_points[1].X = pos.right - m_rcMargin.right;
	scale_points[1].Y = pos.bottom - m_rcMargin.bottom;
	graphics.DrawLines(&m_scale_pen, scale_points, 2);

	int nX     = (int)ceil(fOriginX);
	int nLastX = 0;
	RECT rcScale;
	CDuiString  strText;	

	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, RGB(0x44,0x7A,0xA1));

	for ( int i = 0; i < nMaxPointsCnt; i++ ) {
		scale_points[0].X = (int)((i + nX - fOriginX) * fInterval + nOriginXUi);
		scale_points[0].Y = pos.bottom - m_rcMargin.bottom;
		scale_points[1].X = scale_points[0].X;
		scale_points[1].Y = scale_points[0].Y + SCALE_HEIGHT;

		if ( scale_points[0].X >= pos.left + m_rcMargin.left + nRealWidth ) {
			break;
		}

		if (i == 0) {			
			graphics.DrawLines(&m_scale_pen, scale_points, 2);
			rcScale.left = scale_points[0].X - HORIZONTAL_SCALE_INTERVAL / 2;
			rcScale.right = rcScale.left + HORIZONTAL_SCALE_INTERVAL;
			rcScale.top = scale_points[1].Y;
			rcScale.bottom = rcScale.top + SCALE_FONT_HEIGHT;
			strText.Format("%d", i);
			::DrawText(hDC, strText, -1, &rcScale, DT_CENTER);
			nLastX = scale_points[0].X;
		}
		else {
			if (scale_points[0].X - nLastX >= HORIZONTAL_SCALE_INTERVAL) {
				graphics.DrawLines(&m_scale_pen, scale_points, 2);
				rcScale.left   = scale_points[0].X - HORIZONTAL_SCALE_INTERVAL / 2;
				rcScale.right  = rcScale.left + HORIZONTAL_SCALE_INTERVAL;
				rcScale.top    = scale_points[1].Y;
				rcScale.bottom = rcScale.top + SCALE_FONT_HEIGHT;
				strText.Format("%d", i);
				::DrawText(hDC, strText, -1, &rcScale, DT_CENTER);
				nLastX = scale_points[0].X;
			}
		}		
	}

	if ( nDiffValue == 0 ) {
		return true;
	}

	// 得到 a, ab, abc, abcd,  abcde...形式
	int nVInterval = (nDiffValue - 1) / VERTICAL_SCALE_COUNT + 1;

	// 获得 a, a0, a00, a000, a0000，等形式
	int nResume = 0;
	if ( nVInterval <= 10 ) {
		// nVInterval = nVInterval;
	}
	else if (nVInterval <= 100) {
		nResume = nVInterval % 10;
		nVInterval = nVInterval - nResume;
	}
	else if (nVInterval <= 1000) {
		nResume = nVInterval % 100;
		nVInterval = nVInterval - nResume;
	}
	else if (nVInterval <= 10000) {
		nResume = nVInterval % 1000;
		nVInterval = nVInterval - nResume;
	}
	else {
		nResume = nVInterval % 10000;
		nVInterval = nVInterval - nResume;
	}

	int nY    = (int)ceil(fOriginY);
	int nPage = (nY - 1) / nVInterval + 1;
	nY = nPage * nVInterval;
	BOOL  bFirst = TRUE;
	int nLastY = 0;
	
	for ( int i = nY; i <= nMaxValue; i += nVInterval ) {
		scale_points[0].X = pos.left + m_rcMargin.left;
		scale_points[0].Y = (int)(nOriginYUi - (i - fOriginY) * fVInterval);
		scale_points[1].X = scale_points[0].X - SCALE_WIDTH;
		scale_points[1].Y = scale_points[0].Y;

		if ( bFirst ) {
			graphics.DrawLines(&m_scale_pen, scale_points, 2);

			rcScale.left = pos.left;
			rcScale.right = pos.left + m_rcMargin.left - SCALE_WIDTH - 5;
			rcScale.top = scale_points[1].Y - 8;
			rcScale.bottom = rcScale.top + SCALE_FONT_HEIGHT;
			strText.Format("%d", i);
			::DrawText(hDC, strText, -1, &rcScale, DT_RIGHT);

			nLastY = scale_points[0].Y;
			bFirst = FALSE;
		}
		else {
			if ( nLastY - scale_points[0].Y >= VERTICAL_SCALE_INTERVAL ) {
				graphics.DrawLines(&m_scale_pen, scale_points, 2);

				rcScale.left   = pos.left;
				rcScale.right  = pos.left + m_rcMargin.left - SCALE_WIDTH - 5;
				rcScale.top    = scale_points[1].Y - 8;
				rcScale.bottom = rcScale.top + SCALE_FONT_HEIGHT;
				strText.Format("%d", i);
				::DrawText(hDC, strText, -1, &rcScale, DT_RIGHT );

				nLastY = scale_points[0].Y;
			}
		}
	}



	return true; 
}

void CMyChartUI::DoEvent(DuiLib::TEventUI& event) {
	CControlUI::DoEvent(event);
}

LPCTSTR CMyChartUI::GetClass() const {
	return "MyChart";
}

void CMyChartUI::AddData(int nChartNo, int nValue) {
	if (nChartNo > MAX_CHANNEL_COUNT || nChartNo <= 0)
		return;

	CChannel * pChannel = m_data[nChartNo];
	if ( 0 == pChannel) {
		pChannel = new CChannel;
		m_data[nChartNo] = pChannel;

		std::map<int, DWORD>::iterator it = g_color.find(nChartNo);
		if ( it == g_color.end() ) {
			DWORD dwColor = GetRand(0, 0x00FFFFFF);
			dwColor |= 0xFF000000;
			pChannel->m_pen.SetColor(Gdiplus::Color(dwColor));
			pChannel->m_brush.SetColor(Gdiplus::Color(dwColor));
		}
		else {
			pChannel->m_pen.SetColor(Gdiplus::Color(it->second));
			pChannel->m_brush.SetColor(Gdiplus::Color(it->second));
		}
	}
	pChannel->m_vValues.push_back(nValue);
}

void CMyChartUI::SetPosAndLen(float fPos, float fLength, BOOL bHorizontal /*= TRUE*/) {
	if (bHorizontal) {
		m_fPos = fPos;
		m_fLength = fLength;
	}
	else {
		m_fVPos = fPos;
		m_fVLength = fLength;
	}
	this->Invalidate();
}




CMySliderUI::CMySliderUI() {
	m_ctl_1  = 0;
	m_ctl_2  = 0;
	m_butt_1 = 0;
	m_butt_2 = 0;
	m_block  = 0;
	m_bStartMove = FALSE;
	m_nMoveSource = 0;
	m_nMovePos = 0;
	m_nStartPos = 0;

	this->OnSize += MakeDelegate(this, &CMySliderUI::OnMySize);

	m_fPos    = 0.0f;
	m_fLength = 1.0f;
}

CMySliderUI::~CMySliderUI() {

}

LPCTSTR CMySliderUI::GetClass() const {
	return "MySlider";
}

void CMySliderUI::DoInit() {
	CDialogBuilder builder; 
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(_T("MySlider.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}

	m_ctl_1  = m_pManager->FindControl("ctl_1");
	m_ctl_2  = m_pManager->FindControl("ctl_2");
	m_butt_1 = m_pManager->FindControl("butt_1");
	m_butt_2 = m_pManager->FindControl("butt_2");
	m_block  = m_pManager->FindControl("ctlBlock");
}

bool CMySliderUI::OnMySize(void * pParam) {
	int nWidth = this->GetWidth();
	int nLogicWidth = nWidth - 1 * 2 - BUTT_WIDTH * 2;

	int nCtrlWidth_1 = (int)(nLogicWidth * m_fPos);
	if ( nCtrlWidth_1 <= 0 ) {
		nCtrlWidth_1 = 1;
	}
	
	int nCtrlWidth_2 = (int)((1.0f - m_fPos - m_fLength) * nLogicWidth);
	if (nCtrlWidth_2 <= 0) {
		nCtrlWidth_2 = 1;
	}

	m_ctl_1->SetFixedWidth(nCtrlWidth_1);
	m_ctl_2->SetFixedWidth(nCtrlWidth_2);

	return true;
}

void CMySliderUI::DoEvent(DuiLib::TEventUI& event) {
	if ( event.Type == UIEVENT_BUTTONDOWN ) {
		RECT r1 = m_butt_1->GetPos();
		RECT r2 = m_butt_2->GetPos();
		RECT r3 = m_block->GetPos();

		if (::PtInRect(&r1, event.ptMouse)) {
			m_bStartMove = TRUE;
			m_nMoveSource = 1;    
			m_nMovePos = event.ptMouse.x;
			m_rcMove   = m_butt_1->GetPos();
		}
		else if (::PtInRect(&r2, event.ptMouse)) {
			m_bStartMove = TRUE;
			m_nMoveSource = 2;
			m_nMovePos = event.ptMouse.x;
			m_rcMove = m_butt_2->GetPos();
		}
		else if (::PtInRect(&r3, event.ptMouse)) {
			m_bStartMove = TRUE;
			m_nMoveSource = 3;
			m_nMovePos = event.ptMouse.x;
			m_rcMove = m_block->GetPos();
			m_nStartPos = m_nMovePos;
		}
	}
	else if ( event.Type == UIEVENT_MOUSEMOVE ) {
		if ( m_bStartMove ) {
			SetCursor();
			m_nMovePos = event.ptMouse.x;
			float  fPos = m_fPos;
			float  fLength = m_fLength;
			OnMouseMove();
			CalcData();
			SendNotify(fPos, fLength);
			m_nStartPos = m_nMovePos;
			m_pManager->Invalidate();
		}
		else {
			RECT r1 = m_butt_1->GetPos();
			RECT r2 = m_butt_2->GetPos();

			if (::PtInRect(&r1, event.ptMouse)) {
				SetCursor();
			}
			else if (::PtInRect(&r2, event.ptMouse)) {
				SetCursor();
			}
		}
	}
	else if (event.Type == UIEVENT_BUTTONUP) {
		if (m_bStartMove) {
			float  fPos = m_fPos;
			float  fLength = m_fLength;
			CalcData();
			SendNotify(fPos, fLength);
			m_bStartMove = FALSE;
			m_pManager->Invalidate();
		}
	}
	CContainerUI::DoEvent(event);
}
  
void CMySliderUI::DoPostPaint(HDC hDC, const RECT& rcPaint) {
	RECT rcPos = this->GetPos();
	RECT rc1   = m_ctl_1->GetPos();
	RECT rc2   = m_ctl_2->GetPos();

	RECT & rc = m_rcMove;
	rc.left   = m_nMovePos - BUTT_WIDTH / 2;
	// rc.right  = rc.left + BUTT_WIDTH;
	rc.top    = rcPos.top + 1;
	rc.bottom = rcPos.bottom - 1;

	int nMinLeft = 0;
	int nMaxLeft = 0;

	if ( m_nMoveSource == 1 ) {
		nMinLeft = rcPos.left + 1 + 1;
		nMaxLeft = rc2.left - BUTT_WIDTH -  BUTT_WIDTH;
	}
	else if (m_nMoveSource == 2) {
		nMinLeft = rc1.right + BUTT_WIDTH;
		nMaxLeft = rcPos.right - 1 - 1 - BUTT_WIDTH;
	}

	if (rc.left < nMinLeft) {
		rc.left = nMinLeft;
	}
	else if (rc.left > nMaxLeft) {
		rc.left = nMaxLeft;
	}
	rc.right = rc.left + BUTT_WIDTH;

	CRenderEngine::DrawColor(hDC, rc, 0xAA000000);
}

void  CMySliderUI::CalcData() {
	int nWidth      = this->GetWidth();
	int nLogicWidth = nWidth - 1 * 2 - BUTT_WIDTH * 2;

	m_fPos = (float)(m_ctl_1->GetFixedWidth() - 1) / nLogicWidth;
	m_fLength =  (float)( nWidth - m_ctl_1->GetFixedWidth() - m_ctl_2->GetFixedWidth() - BUTT_WIDTH * 2 ) / nLogicWidth;
}

void  CMySliderUI::OnMouseMove() {
	RECT rcPos = this->GetPos();
	RECT rc1   = m_ctl_1->GetPos();
	RECT rc2   = m_ctl_2->GetPos();

	RECT & rc = m_rcMove;
	rc.left   = m_nMovePos - BUTT_WIDTH / 2;
	rc.top    = rcPos.top + 1;
	rc.bottom = rcPos.bottom - 1;

	int nMinLeft = 0;
	int nMaxLeft = 0;

	if (m_nMoveSource == 1) {
		nMinLeft = rcPos.left + 1 + 1;
		nMaxLeft = rc2.left - BUTT_WIDTH - BUTT_WIDTH;
	}
	else if (m_nMoveSource == 2) {
		nMinLeft = rc1.right + BUTT_WIDTH;
		nMaxLeft = rcPos.right - 1 - 1 - BUTT_WIDTH;
	}

	if (rc.left < nMinLeft) {
		rc.left = nMinLeft;
	}
	else if (rc.left > nMaxLeft) {
		rc.left = nMaxLeft;
	}
	rc.right = rc.left + BUTT_WIDTH;

	int nWidth = 0;
	if (m_nMoveSource == 1) {
		nWidth = rc.left - (m_rcItem.left + 1);
		if (nWidth <= 0) {
			nWidth = 1;
		}
		m_ctl_1->SetFixedWidth(nWidth);
	}
	else if (m_nMoveSource == 2) {
		nWidth = m_rcItem.right - 1 - rc.right;
		if (nWidth <= 0) {
			nWidth = 1;
		}
		m_ctl_2->SetFixedWidth(nWidth);
	}
	else if ( m_nMoveSource == 3 ) {
		int nDiff = m_nMovePos - m_nStartPos;
		int nWidth_1 = m_ctl_1->GetFixedWidth();
		int nWidth_2 = m_ctl_2->GetFixedWidth();
		if (nWidth_1 <= 0) {
			nWidth_1 = 1;
		}
		if (nWidth_2 <= 0) {
			nWidth_2 = 1;
		}

		int nWidthBlock = m_block->GetWidth();
		if ( nWidth_2 - nDiff <= 0 ) {
			nDiff = nWidth_2 - 1;
			nWidth_2 = 1;
			nWidth_1 += nDiff;
		}
		else if (nWidth_1 + nDiff <= 0) {
			nDiff = 1 - nWidth_1;
			nWidth_1 = 1;
			nWidth_2 -= nDiff;
		}
		else {
			nWidth_1 += nDiff;
			nWidth_2 -= nDiff;
		}
		m_ctl_1->SetFixedWidth(nWidth_1);
		m_ctl_2->SetFixedWidth(nWidth_2);
	}
}

void  CMySliderUI::SetCursor() {
	if (g_data.m_cursor_we != 0)
		::SetCursor(g_data.m_cursor_we);
}

void  CMySliderUI::SendNotify(float fPos, float fLength) {
	float f1 = fPos - m_fPos;
	float f2 = fLength - m_fLength;
	BOOL bNotify = FALSE;

	if ( !(f1 >= -0.001f && f1 <= 0.001f) || !(f2 >= -0.001f && f2 <= 0.001f) ) {
		bNotify = TRUE ;
	}

	if (bNotify) {
		m_pManager->SendNotify(this, "hslider_changed", (int)(m_fPos * 1000.0f), (int)(m_fLength * 1000.0f));
	}
}




CMyVSliderUI::CMyVSliderUI() {
	m_ctl_1 = 0;
	m_ctl_2 = 0;
	m_butt_1 = 0;
	m_butt_2 = 0;
	m_block = 0;
	m_bStartMove = FALSE;
	m_nMoveSource = 0;
	m_nMovePos = 0;
	m_nStartPos = 0;

	this->OnSize += MakeDelegate(this, &CMyVSliderUI::OnMySize);

	m_fPos = 0.0f;
	m_fLength = 1.0f;
}

CMyVSliderUI::~CMyVSliderUI() {

}

LPCTSTR CMyVSliderUI::GetClass() const {
	return "MySlider";
}

void CMyVSliderUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(_T("MyVSlider.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}

	m_ctl_1 = m_pManager->FindControl("ctl_1");
	m_ctl_2 = m_pManager->FindControl("ctl_2");
	m_butt_1 = m_pManager->FindControl("butt_1");
	m_butt_2 = m_pManager->FindControl("butt_2");
	m_block = m_pManager->FindControl("ctlBlock");
}

bool CMyVSliderUI::OnMySize(void * pParam) {
	int nHeight = this->GetHeight();
	int nLogicHeight = nHeight - 1 * 2 - BUTT_WIDTH * 2;

	int nCtrlHeight_1 = (int)(nLogicHeight * m_fPos);
	if (nCtrlHeight_1 <= 0) {
		nCtrlHeight_1 = 1;
	}

	int nCtrlHeight_2 = (int)((1.0f - m_fPos - m_fLength) * nLogicHeight);
	if (nCtrlHeight_2 <= 0) {
		nCtrlHeight_2 = 1;
	}

	m_ctl_1->SetFixedHeight(nCtrlHeight_1);
	m_ctl_2->SetFixedHeight(nCtrlHeight_2);
	return true;
}

void CMyVSliderUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_BUTTONDOWN) {
		RECT r1 = m_butt_1->GetPos();
		RECT r2 = m_butt_2->GetPos();
		RECT r3 = m_block->GetPos();

		if (::PtInRect(&r1, event.ptMouse)) {
			m_bStartMove = TRUE;
			m_nMoveSource = 1;
			m_nMovePos = event.ptMouse.y;
			m_rcMove = m_butt_1->GetPos();
		}
		else if (::PtInRect(&r2, event.ptMouse)) {
			m_bStartMove = TRUE;
			m_nMoveSource = 2;
			m_nMovePos = event.ptMouse.y;
			m_rcMove = m_butt_2->GetPos();
		}
		else if (::PtInRect(&r3, event.ptMouse)) {
			m_bStartMove = TRUE;
			m_nMoveSource = 3;
			m_nMovePos = event.ptMouse.y;
			m_rcMove = m_block->GetPos();
			m_nStartPos = m_nMovePos;
		}
	}
	else if (event.Type == UIEVENT_MOUSEMOVE) {
		if (m_bStartMove) {
			SetCursor();
			m_nMovePos = event.ptMouse.y;
			float  fPos = m_fPos;
			float  fLength = m_fLength;			
			OnMouseMove();
			CalcData();
			SendNotify(fPos, fLength);
			m_nStartPos = m_nMovePos;
			m_pManager->Invalidate();
		}
		else {
			RECT r1 = m_butt_1->GetPos();
			RECT r2 = m_butt_2->GetPos();

			if (::PtInRect(&r1, event.ptMouse)) {
				SetCursor();
			}
			else if (::PtInRect(&r2, event.ptMouse)) {
				SetCursor();
			}
		}
	}
	else if (event.Type == UIEVENT_BUTTONUP) {
		if (m_bStartMove) {
			float  fPos = m_fPos;
			float  fLength = m_fLength;
			CalcData();
			SendNotify(fPos, fLength);
			m_bStartMove = FALSE;
			m_pManager->Invalidate();
		}
	}
	CContainerUI::DoEvent(event);
}

void CMyVSliderUI::DoPostPaint(HDC hDC, const RECT& rcPaint) {
	RECT rcPos = this->GetPos();
	RECT rc1 = m_ctl_1->GetPos();
	RECT rc2 = m_ctl_2->GetPos();

	RECT & rc = m_rcMove;
	rc.top   = m_nMovePos - BUTT_WIDTH / 2;
	rc.left  = rcPos.left + 1;
	rc.right = rcPos.right - 1;

	int nMinTop = 0;
	int nMaxTop = 0;

	if (m_nMoveSource == 1) {
		nMinTop = rcPos.top + 1 + 1;
		nMaxTop = rc2.top - BUTT_WIDTH - BUTT_WIDTH;
	}
	else if (m_nMoveSource == 2) {
		nMinTop = rc1.bottom + BUTT_WIDTH;
		nMaxTop = rcPos.bottom - 1 - 1 - BUTT_WIDTH;
	}

	if (rc.top < nMinTop) {
		rc.top = nMinTop;
	}
	else if (rc.top > nMaxTop) {
		rc.top = nMaxTop;
	}
	rc.bottom = rc.top + BUTT_WIDTH;

	CRenderEngine::DrawColor(hDC, rc, 0xAA000000);
}

void  CMyVSliderUI::CalcData() {
	int nHeight = this->GetHeight();
	int nLogicHeight = nHeight - 1 * 2 - BUTT_WIDTH * 2;

	m_fPos    = (float)(m_ctl_2->GetFixedHeight() - 1) / nLogicHeight;
	m_fLength = (float)(nHeight - m_ctl_1->GetFixedHeight() - m_ctl_2->GetFixedHeight() - BUTT_WIDTH * 2) / nLogicHeight;
}

void  CMyVSliderUI::OnMouseMove() {
	RECT rcPos = this->GetPos();
	RECT rc1 = m_ctl_1->GetPos();
	RECT rc2 = m_ctl_2->GetPos();

	RECT & rc = m_rcMove;
	rc.top = m_nMovePos - BUTT_WIDTH / 2;
	rc.left = rcPos.left + 1;
	rc.right = rcPos.right - 1;

	int nMinTop = 0;
	int nMaxTop = 0;

	if (m_nMoveSource == 1) {
		nMinTop = rcPos.top + 1 + 1;
		nMaxTop = rc2.top - BUTT_WIDTH - BUTT_WIDTH;
	}
	else if (m_nMoveSource == 2) {
		nMinTop = rc1.bottom + BUTT_WIDTH;
		nMaxTop = rcPos.bottom - 1 - 1 - BUTT_WIDTH;
	}

	if (rc.top < nMinTop) {
		rc.top = nMinTop;
	}
	else if (rc.top > nMaxTop) {
		rc.top = nMaxTop;
	}
	rc.bottom = rc.top + BUTT_WIDTH;

	int nHeight = 0;
	if (m_nMoveSource == 1) {
		nHeight = rc.top - (m_rcItem.top + 1);
		if (nHeight <= 0) {
			nHeight = 1;
		}
		m_ctl_1->SetFixedHeight(nHeight);
	}
	else if (m_nMoveSource == 2) {
		nHeight = m_rcItem.bottom - 1 - rc.bottom;
		if (nHeight <= 0) {
			nHeight = 1;
		}
		m_ctl_2->SetFixedHeight(nHeight);
	}
	else if (m_nMoveSource == 3) {
		int nDiff = m_nMovePos - m_nStartPos;
		int nHeight_1 = m_ctl_1->GetFixedHeight();
		int nHeight_2 = m_ctl_2->GetFixedHeight();
		if (nHeight_1 <= 0) {
			nHeight_1 = 1;
		}
		if (nHeight_2 <= 0) {
			nHeight_2 = 1;
		}

		if (nHeight_2 - nDiff <= 0) {
			nDiff = nHeight_2 - 1;
			nHeight_2 = 1;
			nHeight_1 += nDiff;
		}
		else if (nHeight_1 + nDiff <= 0) {
			nDiff = 1 - nHeight_1;
			nHeight_1 = 1;
			nHeight_2 -= nDiff;
		}
		else {
			nHeight_1 += nDiff;
			nHeight_2 -= nDiff;
		}
		m_ctl_1->SetFixedHeight(nHeight_1);
		m_ctl_2->SetFixedHeight(nHeight_2);
	}
}

void  CMyVSliderUI::SetCursor() {
	if (g_data.m_cursor_ns != 0)
		::SetCursor(g_data.m_cursor_ns);
}

void  CMyVSliderUI::SendNotify(float fPos, float fLength) {
	float f1 = fPos - m_fPos;
	float f2 = fLength - m_fLength;
	BOOL bNotify = FALSE;

	if (!(f1 >= -0.001f && f1 <= 0.001f) || !(f2 >= -0.001f && f2 <= 0.001f)) {
		bNotify = TRUE;
	}

	if (bNotify) {
		m_pManager->SendNotify(this, "vslider_changed", (int)(m_fPos * 1000.0f), (int)(m_fLength * 1000.0f));
	}
}
