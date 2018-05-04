#pragma once
#include "UIlib.h"
#include "sigslot.h"
#include "LmnOdbc.h"
#include "InvOutCommon.h"

class CDuiFrameWnd : public DuiLib::WindowImplBase, public sigslot::has_slots<>
{
public:
	CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("main.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void      InitWindow();
	virtual void      Notify(DuiLib::TNotifyUI& msg);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

	// 处理自定义信息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	
	DuiLib::CTabLayoutUI *        m_tabs;
	DuiLib::CLabelUI *            m_lblDbStatus;
	DuiLib::CListUI *             m_lstAgencies;

private:
	void  OnDbStatusChange(CLmnOdbc::DATABASE_STATUS eNewStatus);
	void  OnDbStatusChangeMsg(CLmnOdbc::DATABASE_STATUS eNewStatus);

	void  OnGetAllAgency(int, const std::vector<AgencyItem *> &);
	void  OnGetAllAgencyMsg(int, const std::vector<AgencyItem *> &);

	void  OnDeleteAgencyRet(int, DWORD);
	void  OnDeleteAgencyMsg(int, DWORD);

private:
	void  AddAgencyItem2List(DuiLib::CListTextElementUI* pListElement, AgencyItem * pItem, BOOL bSetTag = TRUE);
	void  OnAddAgency();
	void  OnModifyAgency();
	void  OnDeleteAgency();
};