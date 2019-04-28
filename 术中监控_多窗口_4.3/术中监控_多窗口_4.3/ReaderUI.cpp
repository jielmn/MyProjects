#include "ReaderUI.h"

CReaderUI::CReaderUI(ReaderType e /*= Hand*/) : m_callback( m_pManager ) {
	m_bInited = FALSE;
	m_lblIndicator = 0;
	m_dwIndicator = 0;
	m_indicator = 0;
	m_type = e;
	m_optSelected = 0;
	m_cstBodyPart = 0;
	m_state = 0;
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

	m_optSelected = static_cast<DuiLib::COptionUI  *>(m_pManager->FindControl(OPT_READER_SELECTED));
	m_cstBodyPart = static_cast<CEditableButtonUI  *>(m_pManager->FindControl(CST_EDT_BTN_BodyPart));
	m_state = m_pManager->FindControl(CTR_READER_STATE);

	if ( m_type == Hand ) {
		m_lblIndicator->SetVisible(false);
		m_optSelected->SetVisible(false);
		m_cstBodyPart->SetEnabled(false);
		m_state->SetVisible(false); 
	}

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