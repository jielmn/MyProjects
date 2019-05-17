#include "TagUI.h"

CTagUI::CTagUI() : m_callback(m_pManager) {
	m_bInited = FALSE;
}

CTagUI::~CTagUI() {

}


LPCTSTR CTagUI::GetClass() const {
	return "TagUI";
}

void CTagUI::DoInit() {
	if (m_bInited) {
		return;
	}

	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(TAG_FILE_NAME, (UINT)0, &m_callback, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
		m_pManager->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		m_bInited = TRUE;
		return;
	}

	m_bInited = TRUE;
}

void CTagUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}