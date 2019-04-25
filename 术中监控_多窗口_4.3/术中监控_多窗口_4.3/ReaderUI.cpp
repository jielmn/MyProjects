#include "ReaderUI.h"

CReaderUI::CReaderUI() : m_callback( m_pManager ) {
	m_bInited = FALSE;
	m_lblIndicator = 0;
	m_dwIndicator = 0;
	m_indicator = 0;
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

	m_lblIndicator = static_cast<DuiLib::CLabelUI  *>(m_pManager->FindControl(LBL_READER_INDICATOR));
	m_indicator = m_pManager->FindControl(READER_INDICATOR);
	SetIndicator(m_dwIndicator);

	m_bInited = TRUE;
}

void CReaderUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void  CReaderUI::SetIndicator(DWORD dwIndex) {
	m_dwIndicator = dwIndex;

	if (m_lblIndicator) {
		CDuiString  strText;
		strText.Format("%c", 'A' + dwIndex);
		m_lblIndicator->SetText(strText);
	}

	if (m_indicator) {
		assert(m_dwIndicator < MAX_READERS_PER_GRID);
		m_indicator->SetBkColor(g_ReaderIndicator[m_dwIndicator]);
	}
}