#include "MenuUI.h"

LRESULT CDuiMenu::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Close();
	bHandled = FALSE;
	return 0;
}

void CDuiMenu::Init(HWND hWndParent, POINT ptPos)
{
	Create(hWndParent, _T("MenuWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	::ClientToScreen(hWndParent, &ptPos);
	::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void  CDuiMenu::Notify(TNotifyUI& msg) {
	if (msg.sType == "itemclick") {
		if (m_pOwner) {
			m_pOwner->GetManager()->SendNotify(m_pOwner, msg.pSender->GetName(), 0, 0, true);
			this->PostMessage(WM_CLOSE);
		}
		return;
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiMenu::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}