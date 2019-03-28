#include "DragDropUI.h"


CDragDropUI::CDragDropUI() {
	m_cursor = 0;
	m_eDragDropType = DragDropType_ANY;
	m_bBounding = FALSE;
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
		else if (m_eDragDropType == DragDropType_NS) {
			m_rcNewPos.left = rect.left;
			m_rcNewPos.top = event.ptMouse.y - height / 2;
		}

		// 如果有范围限制
		if (m_bBounding) {
			if (m_rcNewPos.left < m_rcBounding.left) {
				m_rcNewPos.left = m_rcBounding.left;
			} else if (m_rcNewPos.left + width > m_rcBounding.right) {
				m_rcNewPos.left = m_rcBounding.right - width;
			}

			if (m_rcNewPos.top < m_rcBounding.top) {
				m_rcNewPos.top = m_rcBounding.top;
			}
			else if (m_rcNewPos.top + height > m_rcBounding.bottom) {
				m_rcNewPos.top = m_rcBounding.bottom - height;
			}
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
		m_pManager->Invalidate();
	}
	else if (event.Type == UIEVENT_BUTTONUP) {
		m_pManager->RemovePostPaint(this);
		this->SetPos(m_rcNewPos);
		m_pManager->SendNotify(this, "dragdrop_complete");
		m_pManager->Invalidate();
	}
	CControlUI::DoEvent(event);
}

void  CDragDropUI::SetCursor(HCURSOR h) {
	m_cursor = h;
}

void  CDragDropUI::SetDragDropType(DragDropType e) {
	m_eDragDropType = e;
}

void  CDragDropUI::SetBoundingRect(const RECT & r) {
	m_rcBounding = r;
	m_bBounding = TRUE;
}

void CDragDropUI::DoPostPaint(HDC hDC, const RECT& rcPaint) {
	CRenderEngine::DrawColor(hDC, m_rcNewPos, 0xAA000000);
}