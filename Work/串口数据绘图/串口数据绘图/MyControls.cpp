#include "MyControls.h"

#define  BUTT_WIDTH    10

CMyChartUI::CMyChartUI() {

}

CMyChartUI::~CMyChartUI() {

}

bool CMyChartUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);
	::SetTextColor(hDC, RGB(255, 0, 0));
	::TextOut(hDC, m_rcItem.left, m_rcItem.top, "111", 3); 
	return true;
}

void CMyChartUI::DoEvent(DuiLib::TEventUI& event) {
	CControlUI::DoEvent(event);
}

LPCTSTR CMyChartUI::GetClass() const {
	return "MyChart";
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
	
	int nCtrlWidth_2 = (int)((1 - m_fPos - m_fLength) * nLogicWidth);
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

		if (::PtInRect(&r1, event.ptMouse)) {
			m_pManager->AddPostPaint(this);
			m_bStartMove = TRUE;
			m_nMoveSource = 1;    
			m_nMovePos = event.ptMouse.x;
		}
		else if (::PtInRect(&r2, event.ptMouse)) {
			m_pManager->AddPostPaint(this);
			m_bStartMove = TRUE;
			m_nMoveSource = 2;
			m_nMovePos = event.ptMouse.x;
		}
	}
	else if ( event.Type == UIEVENT_MOUSEMOVE ) {
		if ( m_bStartMove ) {
			m_nMovePos = event.ptMouse.x;
			m_pManager->Invalidate();
		}
	}
	else if (event.Type == UIEVENT_BUTTONUP) {
		if (m_bStartMove) {
			m_bStartMove = FALSE;
			m_pManager->RemovePostPaint(this);
			m_pManager->Invalidate();
		}
	}
	CContainerUI::DoEvent(event);
}
  
void CMySliderUI::DoPostPaint(HDC hDC, const RECT& rcPaint) {
	RECT rcPos = this->GetPos();
	RECT rc1   = m_ctl_1->GetPos();
	RECT rc2   = m_ctl_2->GetPos();

	RECT  rc;
	rc.left   = m_nMovePos - BUTT_WIDTH / 2;
	// rc.right  = rc.left + BUTT_WIDTH;
	rc.top    = rcPos.top + 1;
	rc.bottom = rcPos.bottom - 1;

	int nMinLeft = 0;
	int nMaxLeft = 0;

	if ( m_nMoveSource == 1 ) {
		nMinLeft = rcPos.left + 1 + 1;
		nMaxLeft = rc2.left - 1 - BUTT_WIDTH;
	}
	else if (m_nMoveSource == 2) {
		nMinLeft = rc1.right + 1;
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




CMyVSliderUI::CMyVSliderUI() {

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
}