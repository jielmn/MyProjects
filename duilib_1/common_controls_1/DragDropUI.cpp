#include "DragDropUI.h"


CDragDropUI::CDragDropUI() {
	m_cursor = 0;
	m_eDragDropType = DragDropType_ANY;
}

CDragDropUI::~CDragDropUI() {

}

bool CDragDropUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);
	return true;
}

void CDragDropUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_MOUSEMOVE) {
		if (m_cursor!= 0)
			::SetCursor(m_cursor);

		RECT rect = this->GetPos();
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		if (m_eDragDropType == DragDropType_ANY) {
			m_rcNewPos.left = event.ptMouse.x - width / 2;
			m_rcNewPos.top = event.ptMouse.y - height / 2;			
		}
		else if (m_eDragDropType == DragDropType_WE) {
			m_rcNewPos.left = event.ptMouse.x - width / 2;
			m_rcNewPos.top = rect.top;
		}

		m_rcNewPos.right = m_rcNewPos.left + width;
		m_rcNewPos.bottom = m_rcNewPos.top + height;
		
		m_pManager->Invalidate();
	}
	else if (event.Type == UIEVENT_BUTTONDOWN) {
		if (m_cursor != 0)
			::SetCursor(m_cursor);
		m_rcNewPos = m_rcItem;
		m_pManager->AddPostPaint(this);
	}
	else if (event.Type == UIEVENT_BUTTONUP) {
		m_pManager->RemovePostPaint(this);
	}
	CControlUI::DoEvent(event);
}

void  CDragDropUI::SetCursor(HCURSOR h) {
	m_cursor = h;
}

void  CDragDropUI::SetDragDropType(DragDropType e) {
	m_eDragDropType = e;
}

void CDragDropUI::DoPostPaint(HDC hDC, const RECT& rcPaint) {
	CRenderEngine::DrawColor(hDC, m_rcNewPos, 0xAA000000);
}