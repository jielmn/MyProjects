#include "MyControls.h"


CDeskUI::CDeskUI() {
	m_Sex = 0;
	m_lblName = 0;
	m_lblTemp = 0;
	m_lblDate = 0;
	m_lblTime = 0;
	m_Warning = 0;
	m_btnDel = 0;
}

CDeskUI::~CDeskUI() {

}

LPCTSTR CDeskUI::GetClass() const {
	return "Desk";
}

void CDeskUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(_T("Desk.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}

	m_Sex = m_pManager->FindControl("ctlSex");
	m_lblName = static_cast<DuiLib::CLabelUI*>(m_pManager->FindControl("lblName"));
	m_lblTemp = static_cast<DuiLib::CLabelUI*>(m_pManager->FindControl("lblTemp"));
	m_lblDate = static_cast<DuiLib::CLabelUI*>(m_pManager->FindControl("lblDate"));
	m_lblTime = static_cast<DuiLib::CLabelUI*>(m_pManager->FindControl("lblTime"));
	m_Warning = m_pManager->FindControl("ctlWarning");
	m_btnDel = static_cast<DuiLib::CButtonUI*>(m_pManager->FindControl("btnDelDesk"));
}

void CDeskUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}