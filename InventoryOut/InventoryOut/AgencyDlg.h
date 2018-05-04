#pragma once
#include "UIlib.h"
#include "sigslot.h"
#include "InvOutCommon.h"

class CAgencyWnd : public DuiLib::WindowImplBase, public sigslot::has_slots<>
{
public:
	CAgencyWnd( BOOL  bAdd = TRUE );

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIAgency"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("Agency_info.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void    Notify(DuiLib::TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void   InitWindow();
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/);

	AgencyItem    m_tAgency;

private:
	DuiLib::CEditUI *            m_edAgencyId;
	DuiLib::CEditUI *            m_edAgencyName;
	DuiLib::CComboUI *           m_cmbProvince;

	BOOL                         m_bAdd;	
private:
	void  OnAgencyOk();

private:
	void  OnAddAgencyRet(int ret, DWORD dwNewId);
	void  OnAddAgencyRetMsg(int ret, DWORD dwNewId);

	void  OnModifyAgencyRet(int ret);
	void  OnModifyAgencyRetMsg(int ret);
};
