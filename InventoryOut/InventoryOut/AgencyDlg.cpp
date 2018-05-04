#include "AgencyDlg.h"
#include "Business.h"

CAgencyWnd::CAgencyWnd(BOOL  bAdd /*= TRUE*/) {
	m_bAdd = bAdd;
	memset( &m_tAgency, 0, sizeof(AgencyItem));
	CBusiness::GetInstance()->m_sigAddAgency.connect(this, &CAgencyWnd::OnAddAgencyRet);
	CBusiness::GetInstance()->m_sigModifyAgency.connect(this, &CAgencyWnd::OnModifyAgencyRet);
}

void    CAgencyWnd::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if (msg.sType == "click") {
		if (name == "btnAgencyOk") {
			OnAgencyOk();
		}
	}
	DuiLib::WindowImplBase::Notify(msg);
}

LRESULT CAgencyWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_ADD_AGENCY_RET ) {
		OnAddAgencyRetMsg(wParam, lParam);
	}
	else if (uMsg == UM_MODIFY_AGENCY_RET) {
		OnModifyAgencyRetMsg(wParam);
	}
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void   CAgencyWnd::InitWindow() {
	m_edAgencyId = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edId"));
	m_edAgencyName = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edName"));
	m_cmbProvince = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbProvinces"));

	DuiLib::CDuiString strText;

	if (!m_bAdd) {
		m_edAgencyId->SetText(m_tAgency.szId);
		m_edAgencyName->SetText(m_tAgency.szName);

		int nCnt = m_cmbProvince->GetCount();
		for (int i = 0; i < nCnt; i++) {
			strText = m_cmbProvince->GetItemAt(i)->GetText();
			if ( strText == m_tAgency.szProvince ) {
				m_cmbProvince->SelectItem(i);
				break;
			}
		}

		m_edAgencyId->SetEnabled(false);
	}
	DuiLib::WindowImplBase::InitWindow();
}

LRESULT CAgencyWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) {
	return DuiLib::WindowImplBase::MessageHandler(uMsg, wParam, lParam, bHandled);
}

void  CAgencyWnd::OnAgencyOk() {
	DuiLib::CDuiString  strId, strName ;
	DuiLib::CDuiString  strTitle;
	DuiLib::CDuiString  strProvince;

	if (m_bAdd) {
		strTitle = "添加经销商";
	}
	else {
		strTitle = "修改经销商";
	}

	strId = m_edAgencyId->GetText();
	strName = m_edAgencyName->GetText();
	int nProvinceIndex = m_cmbProvince->GetCurSel();

	if (strId.GetLength() == 0) {
		::MessageBox(GetHWND(), "编号不能为空", strTitle, 0);
		return;
	}

	if (strName.GetLength() == 0) {
		::MessageBox(GetHWND(), "名称不能为空", strTitle, 0);
		return;
	}

	strProvince = m_cmbProvince->GetText();

	STRNCPY(m_tAgency.szId,           strId,          sizeof(m_tAgency.szId));
	STRNCPY(m_tAgency.szName,         strName,        sizeof(m_tAgency.szName));
	STRNCPY(m_tAgency.szProvince,     strProvince,    sizeof(m_tAgency.szProvince));

	if (m_bAdd)
	{
		CBusiness::GetInstance()->AddAgencyAsyn(&m_tAgency);
	}
	else {
		CBusiness::GetInstance()->ModifyAgencyAsyn(&m_tAgency);
	}
	
}


void  CAgencyWnd::OnAddAgencyRet(int ret, DWORD dwNewId) {
	::PostMessage(GetHWND(), UM_ADD_AGENCY_RET, ret, dwNewId);
}

void  CAgencyWnd::OnAddAgencyRetMsg(int ret, DWORD dwNewId) {
	if (0 == ret) {
		//::MessageBox(GetHWND(), "添加经销商成功", "添加经销商", 0);
		m_tAgency.dwId = dwNewId;
		PostMessage(WM_CLOSE);
	}
	else {
		::MessageBox(GetHWND(), GetErrorDescription(ret), "添加经销商", 0);
	}
}

void  CAgencyWnd::OnModifyAgencyRet(int ret) {
	::PostMessage(GetHWND(), UM_MODIFY_AGENCY_RET, ret, 0);
}

void  CAgencyWnd::OnModifyAgencyRetMsg(int ret) {
	if (0 == ret) {
		//::MessageBox(GetHWND(), "添加经销商成功", "添加经销商", 0);
		PostMessage(WM_CLOSE);
	}
	else {
		::MessageBox(GetHWND(), GetErrorDescription(ret), "修改经销商", 0);
	}
}