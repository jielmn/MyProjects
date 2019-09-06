#include "DuiMenu.h"

void  CDuiMenu::InitWindow() {
	BOOL  bPlay = (BOOL)m_dwParam0;
	BOOL  bFullScreen = (BOOL)m_dwParam1;
	CListContainerElementUI * pItem = (CListContainerElementUI *)m_PaintManager.FindControl("menu_play");
	CListContainerElementUI * pItem_1 = (CListContainerElementUI *)m_PaintManager.FindControl("menu_full_screen");

	if (bPlay) { 
		pItem->GetItemAt(0)->SetText("暂停");
	}
	else {
		pItem->GetItemAt(0)->SetText("继续播放");
	}

	if (bFullScreen) {
		pItem_1->GetItemAt(0)->SetText("退出全屏");
	}
	else {
		pItem_1->GetItemAt(0)->SetText("全屏");
	}
}

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

void  CDuiMenu::Notify(DuiLib::TNotifyUI& msg) {
	if (msg.sType == "itemclick") {
		if (m_pOwner) {
			m_pOwner->GetManager()->SendNotify(m_pOwner, msg.pSender->GetName(), m_dwParam0, m_dwParam1, true);
			PostMessage(WM_CLOSE);
		}
		return;
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiMenu::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::HandleMessage(uMsg, wParam, lParam);
}