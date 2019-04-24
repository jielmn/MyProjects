#include "ReaderUI.h"

CReaderUI::CReaderUI() : m_callback( m_pManager ) {
	m_bInited = FALSE;
}

CReaderUI::~CReaderUI() {

}

LPCTSTR  CReaderUI::GetClass() const {
	return "ReaderUI";
}

void CReaderUI::DoInit() {
	if (m_bInited) {
		return;
	}

	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(READER_FILE_NAME, (UINT)0, &m_callback, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		m_bInited = TRUE;
		return;
	}

	m_bInited = TRUE;
}

void CReaderUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}