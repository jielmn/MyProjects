#include "TargetDlg.h"
#include "Business.h"

CTargetDlg::CTargetDlg(int nTargetType /*= 0*/) : m_nTargetType( nTargetType ) {
	CBusiness::GetInstance()->m_sigGetAllSaleStaff.connect(this, &CTargetDlg::OnGetAllSalesRet);
	CBusiness::GetInstance()->m_sigGetAllAgencyForTarget.connect(this, &CTargetDlg::OnGetAllAgentRet);
}

void  CTargetDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if (msg.sType == _T("click")) {
		if (name == "btnOk") {
			OnOk();
		}
	}
	DuiLib::WindowImplBase::Notify(msg);
}

LRESULT CTargetDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int ret = 0;
	if (uMsg == UM_GET_ALL_SALES) {
		ret = wParam;
		std::vector<SaleStaff *> * pvRet = (std::vector<SaleStaff *> *)lParam;

		assert(pvRet);
		OnGetAllSalesMsg(ret, *pvRet);

		if (pvRet) {
			ClearVector(*pvRet);
			delete pvRet;
		}
	}
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void    CTargetDlg::InitWindow() {
	m_layoutAgencies = static_cast<DuiLib::CVerticalLayoutUI*>(m_PaintManager.FindControl("layoutAgencies"));
	m_layoutSales = static_cast<DuiLib::CVerticalLayoutUI*>(m_PaintManager.FindControl("layoutSales"));

	m_lstSales = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("query_sales_list")); 

	if (m_nTargetType == 0) {
		m_layoutSales->SetVisible(true);
		m_layoutAgencies->SetVisible(false);

		// 获取所有的销售
		CBusiness::GetInstance()->GetAllSalesAsyn();
	}
	else {
		m_layoutSales->SetVisible(false);
		m_layoutAgencies->SetVisible(true);  

		// 获取所有的供应商
	}

	DuiLib::WindowImplBase::InitWindow();
}

LRESULT CTargetDlg::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled ) {
	return DuiLib::WindowImplBase::MessageHandler(uMsg, wParam, lParam, bHandled);
}



void   CTargetDlg::OnGetAllSalesRet(int ret, const std::vector<SaleStaff *> & vRet) {
	std::vector<SaleStaff *> * pvRet = new std::vector<SaleStaff *>;

	std::vector<SaleStaff *>::const_iterator it;
	for (it = vRet.begin(); it != vRet.end(); it++) {
		SaleStaff * pItem = *it;
		SaleStaff * pNewItem = new SaleStaff;
		memcpy(pNewItem, pItem, sizeof(SaleStaff));

		pvRet->push_back(pNewItem);
	}

	::PostMessage(GetHWND(), UM_GET_ALL_SALES, ret, (LPARAM)pvRet);
}

void   CTargetDlg::OnGetAllSalesMsg(int ret, const std::vector<SaleStaff *> & vRet) {
	if (0 != ret) {
		return;
	}

	std::vector<SaleStaff *>::const_iterator it;
	for (it = vRet.begin(); it != vRet.end(); it++) {
		SaleStaff * pItem = *it;

		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstSales->Add(pListElement);

		pListElement->SetText(0, pItem->szId);
		pListElement->SetText(1, pItem->szName);
	}
}

void   CTargetDlg::OnGetAllAgentRet(int ret, const std::vector<AgencyItem *> & vRet) {

}

void   CTargetDlg::OnGetAllAgentMsg(int ret, const std::vector<AgencyItem *> & vRet) {

}




void   CTargetDlg::OnOk() {
	int nSelIndex = -1;

	if (m_nTargetType == 0) {
		nSelIndex = m_lstSales->GetCurSel();
		if (nSelIndex < 0) {
			MessageBox(GetHWND(), "请选择一个销售", "选择销售", 0);
			return;
		}

		DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstSales->GetItemAt(nSelIndex);
		m_strId   = pListElement->GetText(0);
		m_strName = pListElement->GetText(1);

		PostMessage(WM_CLOSE);
	}
	else {

	}
}