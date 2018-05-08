#pragma once
#include "UIlib.h"
#include "sigslot.h"
#include "InvOutCommon.h"

class CTargetDlg : public DuiLib::WindowImplBase, public sigslot::has_slots<>
{
public:
	CTargetDlg(int nTargetType = 0); 
	~CTargetDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUITarget"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("Target.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void    Notify(DuiLib::TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void   InitWindow();
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/);

private:
	int    m_nTargetType;

private:
	DuiLib::CVerticalLayoutUI *      m_layoutAgencies;
	DuiLib::CVerticalLayoutUI *      m_layoutSales;

	DuiLib::CListUI *                m_lstSales;
	DuiLib::CListUI *                m_lstAgencies;
	DuiLib::CComboUI *               m_cmbProvinces;

private:
	void   OnGetAllSalesRet(int, const std::vector<SaleStaff *> &);
	void   OnGetAllSalesMsg(int, const std::vector<SaleStaff *> &);

	void   OnGetAllAgentRet(int, const std::vector<AgencyItem *> &);
	void   OnGetAllAgentMsg(int, const std::vector<AgencyItem *> &);

public:
	DuiLib::CDuiString        m_strId;
	DuiLib::CDuiString        m_strName;

	std::vector<AgencyItem *>    m_vAgencies;

private:
	void   OnOk();
	void   OnProvinceChanged();
};
