#include "TargetDlg.h"
#include "Business.h"

CTargetDlg::CTargetDlg(int nTargetType /*= 0*/) : m_nTargetType( nTargetType ) {
	CBusiness::GetInstance()->m_sigGetAllSaleStaff.connect(this, &CTargetDlg::OnGetAllSalesRet);
	CBusiness::GetInstance()->m_sigGetAllAgencyForTarget.connect(this, &CTargetDlg::OnGetAllAgentRet);
}

CTargetDlg::~CTargetDlg() {
	ClearVector(m_vAgencies);
}

void  CTargetDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if (msg.sType == _T("click")) {
		if (name == "btnOk") {
			OnOk();
		}
	}
	else if (msg.sType == "itemselect") {
		if (name == "cmbProvinces") {
			OnProvinceChanged();
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
	else if (uMsg == UM_GET_ALL_AGENCY_FOR_TARGET_RET) {
		ret = wParam;
		std::vector<AgencyItem *> * pvRet = (std::vector<AgencyItem *> *)lParam;
		assert(pvRet);

		OnGetAllAgentMsg(ret, *pvRet);

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
	m_lstAgencies = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("query_agencies_list"));
	m_cmbProvinces = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbProvinces"));

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
		CBusiness::GetInstance()->GetAllAgencyForTargetAsyn();
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
	std::vector<AgencyItem *> * pvRet = new std::vector<AgencyItem *>;
	std::vector<AgencyItem *>::const_iterator it;
	for (it = vRet.begin(); it != vRet.end(); it++) {
		AgencyItem * pItem = *it;
		AgencyItem * pNewItem = new AgencyItem;
		memcpy(pNewItem, pItem, sizeof(AgencyItem));
		pvRet->push_back(pNewItem);
	}

	::PostMessage(GetHWND(), UM_GET_ALL_AGENCY_FOR_TARGET_RET, ret, (LPARAM)pvRet);
}

void   CTargetDlg::OnGetAllAgentMsg(int ret, const std::vector<AgencyItem *> & vRet) {
	std::vector<AgencyItem *>::const_iterator it;
	for (it = vRet.begin(); it != vRet.end(); it++) {
		AgencyItem * pItem = *it;
		AgencyItem * pNewItem = new AgencyItem;
		memcpy(pNewItem, pItem, sizeof(AgencyItem));

		m_vAgencies.push_back(pNewItem);
	}

	OnProvinceChanged();
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
		nSelIndex = m_lstAgencies->GetCurSel();
		if (nSelIndex < 0) {
			MessageBox(GetHWND(), "请选择一个经销商", "选择经销商", 0);
			return;
		}

		DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstAgencies->GetItemAt(nSelIndex);
		m_strId = pListElement->GetText(0);
		m_strName = pListElement->GetText(1);

		PostMessage(WM_CLOSE); 
	}
}

void   CTargetDlg::OnProvinceChanged() {
	m_lstAgencies->RemoveAll();

	int nSelIndex = m_cmbProvinces->GetCurSel();

	DuiLib::CDuiString  strProvince;
	if (nSelIndex >= 0) {
		strProvince = m_cmbProvinces->GetItemAt(nSelIndex)->GetText();
	}
	

	std::vector<AgencyItem *>::const_iterator it;
	for (it = m_vAgencies.begin(); it != m_vAgencies.end(); ++it) {
		AgencyItem * pItem = *it;
		
		if ( nSelIndex == 0 || nSelIndex < 0 ) {
			DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
			m_lstAgencies->Add(pListElement);

			pListElement->SetText(0, pItem->szId);
			pListElement->SetText(1, pItem->szName);
		}
		else {
			if (strProvince == pItem->szProvince) {
				DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
				m_lstAgencies->Add(pListElement);

				pListElement->SetText(0, pItem->szId);
				pListElement->SetText(1, pItem->szName);
			}
		}
		
	}
}