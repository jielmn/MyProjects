#include "DuiMenu.h"

LRESULT CDuiMenu::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Close();
	bHandled = FALSE;
	return 0;
}

void CDuiMenu::Init(HWND hWndParent, POINT ptPos, const char * szCheckImg /*= 0*/)
{
	Create(hWndParent, _T("MenuWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	::ClientToScreen(hWndParent, &ptPos);
	::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	if (szCheckImg)
		m_strCheckedImg = szCheckImg;
}

void CDuiMenu::SetCheckedItem(const char * szItem) {
	if (0 == szItem)
		return;

	std::vector<std::string>::iterator it;
	for (it = m_CheckedItems.begin(); it != m_CheckedItems.end(); ++it) {
		if ( 0 == strcmp(szItem, (*it).c_str()) ) {
			return;
		}
	}
	m_CheckedItems.push_back(szItem);
}

void  CDuiMenu::Notify(DuiLib::TNotifyUI& msg) {
	if (msg.sType == "itemclick") {
		if (m_pOwner) {
			m_pOwner->GetManager()->SendNotify(m_pOwner, msg.pSender->GetName(), m_dwParam0, m_dwParam1, true);
			PostMessage(WM_CLOSE);
		}
		return;
	}
	else if (msg.sType == "windowinit") {
		std::vector<std::string>::iterator it;
		for (it = m_CheckedItems.begin(); it != m_CheckedItems.end(); ++it) {
			std::string & s = *it;
			CControlUI * pControl = m_PaintManager.FindControl(s.c_str());
			if ( pControl ) {
				pControl->SetBkImage(m_strCheckedImg);
			}
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiMenu::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}