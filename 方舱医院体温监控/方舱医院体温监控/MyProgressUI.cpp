#include "MyProgressUI.h"

void CMyProgressUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_BUTTONUP) {
		this->m_pManager->SendNotify(this, "progress_click", 
			event.ptMouse.x - m_rcItem.left, m_rcItem.right - m_rcItem.left );
		//CDuiString str;
		//str.Format("=====(%d,%d), (%d,%d,%d,%d) \n", event.ptMouse.x, event.ptMouse.y,
		//	m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
		//OutputDebugString(str);
	}
	CProgressUI::DoEvent(event);
}