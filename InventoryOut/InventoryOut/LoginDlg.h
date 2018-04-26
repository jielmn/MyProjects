#pragma once
#include "UIlib.h"
#include "sigslot.h"
#include "InvOutDatabase.h"

class CLoginWnd : public DuiLib::WindowImplBase, public sigslot::has_slots<>
{
public:
	CLoginWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUILogin"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("login.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void    Notify(DuiLib::TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void   InitWindow();
	virtual LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/);

private:
	void  OnDbStatusChange(CLmnOdbc::DATABASE_STATUS eNewStatus);
	void  OnDbStatusChangeMsg(CLmnOdbc::DATABASE_STATUS eNewStatus);

	void  OnLoginRet(int ret);
	void  OnLoginRetMsg(int ret);

	void  OnBtnOk();

private:
	DuiLib::CLabelUI *            m_lblDbStatus;
	DuiLib::CEditUI *             m_edUserName;
	DuiLib::CEditUI *             m_edUserPassword;
};
