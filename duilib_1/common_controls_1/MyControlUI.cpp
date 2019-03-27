#include "MyControlUI.h"


CMyControlUI::CMyControlUI() {
	m_nSplitX = -1;
	m_nSplitY = -1;
	m_bPainted = FALSE;

	m_brushs[0] = CreateSolidBrush(RGB(255, 0, 0));
	m_brushs[1] = CreateSolidBrush(RGB(0, 255, 0));
	m_brushs[2] = CreateSolidBrush(RGB(0, 0, 255));
	m_brushs[3] = CreateSolidBrush(RGB(255, 255, 0));

	this->OnSize += MakeDelegate(this, &CMyControlUI::OnMySize);
}

CMyControlUI::~CMyControlUI() {
	for (int i = 0; i < 4; i++) {
		DeleteObject(m_brushs[i]);
	}
}

bool  CMyControlUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);
	RECT  rect = this->GetPos();

	if (!m_bPainted) {
		m_nSplitX = (rect.left + rect.right) / 2;
		m_nSplitY = (rect.top + rect.bottom) / 2;

		m_pManager->SendNotify(this, "init_splits");
		m_bPainted = TRUE;
	}

	RECT r;
	r = rect;
	r.right = m_nSplitX;
	r.bottom = m_nSplitY;
	FillRect(hDC, &r, m_brushs[0]);

	r = rect;
	r.left = m_nSplitX;
	r.bottom = m_nSplitY;
	FillRect(hDC, &r, m_brushs[1]);

	r = rect;
	r.right = m_nSplitX;
	r.top   = m_nSplitY;
	FillRect(hDC, &r, m_brushs[2]);

	r = rect;
	r.left = m_nSplitX;
	r.top  = m_nSplitY;
	FillRect(hDC, &r, m_brushs[3]);

	return true;
}

void  CMyControlUI::DoEvent(DuiLib::TEventUI& event) {
	CControlUI::DoEvent(event);
}

bool CMyControlUI::OnMySize(void * pParam) {
	m_pManager->SendNotify(this, "init_splits");
	return true;
}
