#include <assert.h>
#include "LoginDlg.h"
#include "Business.h"

CLoginWnd::CLoginWnd() {
	CBusiness::GetInstance()->m_sigStatusChange.connect(this, &CLoginWnd::OnDbStatusChange);
	CBusiness::GetInstance()->m_sigLoginRet.connect(this, &CLoginWnd::OnLoginRet);
}

void  CLoginWnd::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if ( msg.sType == "click" ) {
		if ( name == "btnOk" ) {
			OnBtnOk();
		}
	}
	DuiLib::WindowImplBase::Notify(msg);
}

LRESULT CLoginWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int ret = 0;

	if (UM_DB_STATUS == uMsg) {
		CLmnOdbc::DATABASE_STATUS eNewStatus = (CLmnOdbc::DATABASE_STATUS)wParam;
		OnDbStatusChangeMsg(eNewStatus);
		return 0;
	}
	else if (UM_LOGIN_RET == uMsg) {
		ret = (int)wParam;
		OnLoginRetMsg(ret);
		return 0;
	}
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CLoginWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	return DuiLib::WindowImplBase::OnKeyDown(uMsg, wParam, lParam, bHandled);
}

LRESULT CLoginWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) {
	if (uMsg == WM_KEYDOWN) {
		// 如果是ENTER
		if (wParam == VK_RETURN) {
			OnBtnOk();
			bHandled = true;
			return 0; 
		} 
	}   
	return DuiLib::WindowImplBase::MessageHandler(uMsg, wParam, lParam, bHandled);
}

void  CLoginWnd::InitWindow() {
	m_lblDbStatus = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblDatabaseStatus"));
	m_edUserName = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edUserName"));
	m_edUserPassword = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edUserPassword"));

	CInvoutDatabase::DATABASE_STATUS eStatus = CBusiness::GetInstance()->GetDbStatus();
	if (eStatus == CLmnOdbc::STATUS_OPEN) {
		m_lblDbStatus->SetText("数据库连接OK");
	}
	else {
		m_lblDbStatus->SetText("数据库连接失败");
	}

	char buf[8192];
	g_cfg->GetConfig("last login name", buf, sizeof(buf), "");
	m_edUserName->SetText(buf);

	DuiLib::WindowImplBase::InitWindow();
}

void  CLoginWnd::OnDbStatusChange(CLmnOdbc::DATABASE_STATUS eNewStatus) {
	::PostMessage(this->GetHWND(), UM_DB_STATUS, eNewStatus, 0);
}

void  CLoginWnd::OnDbStatusChangeMsg(CLmnOdbc::DATABASE_STATUS eNewStatus) {
	if (eNewStatus == CLmnOdbc::STATUS_OPEN) {
		m_lblDbStatus->SetText("数据库连接OK");
	}
	else {
		m_lblDbStatus->SetText("数据库连接失败");
	}
}

void  CLoginWnd::OnBtnOk() {
	DuiLib::CDuiString  strName;
	DuiLib::CDuiString  strPassword;

	strName = m_edUserName->GetText();
	strPassword = m_edUserPassword->GetText();

	CBusiness::GetInstance()->LoginAsyn(strName, strPassword);
}

void  CLoginWnd::OnLoginRet(int ret) {
	::PostMessage(this->GetHWND(), UM_LOGIN_RET, ret, 0);
}

void  CLoginWnd::OnLoginRetMsg(int ret) {
	// 登录成功
	if (0 == ret) {
		PostMessage(WM_CLOSE);
	}
	else {
		::MessageBox(GetHWND(), "登录失败", "登录", 0);
	}
}