#include "MyControls.h"


CDeskUI::CDeskUI() {

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
}

void CDeskUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}