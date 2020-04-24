#include "MyControls.h"

#define  BUTT_WIDTH    10

CMyChartUI::CMyChartUI() : m_scale_pen( Gdiplus::Color(0xFF1b9375) ) {
	
}

CMyChartUI::~CMyChartUI() {
	std::map<int, CChannel *>::iterator it;
	for (it = m_data.begin(); it != m_data.end(); ++it) {
		delete it->second;
	}
	m_data.clear();
}

bool CMyChartUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	std::map<int, CChannel *>::iterator it;
	for (it = m_data.begin(); it != m_data.end(); ++it) {
		CChannel * pChannel = it->second;
		DWORD  dwPointsCnt = pChannel->m_vValues.size();
		Gdiplus::Point * points = new Gdiplus::Point[dwPointsCnt];
		for (DWORD i = 0; i < dwPointsCnt; i++) {
			points[i].X = 100 * i + 200;
			points[i].Y = pChannel->m_vValues[i];
		}
		graphics.DrawLines(&pChannel->m_pen, points, dwPointsCnt);
		delete[] points;
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
	CChannel * pChannel = m_data[nChartNo];
	if ( 0 == pChannel) {
		pChannel = new CChannel;
		m_data[nChartNo] = pChannel;
	}
	pChannel->m_vValues.push_back(nValue);
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
			OnMouseMove();
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
			OnMouseMove();
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

	m_fPos    = (float)(m_ctl_1->GetFixedHeight() - 1) / nLogicHeight;
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
