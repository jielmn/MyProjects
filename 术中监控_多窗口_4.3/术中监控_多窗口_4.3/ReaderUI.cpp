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
	m_lblTemp = 0;
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
		m_pManager->AddNotifier(this);
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
	m_lblTemp = static_cast<DuiLib::CLabelUI  *>(m_pManager->FindControl(LBL_READER_TEMP));
	m_lblTemp->SetText("");

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

void CReaderUI::Notify(TNotifyUI& msg) {
	if (msg.sType == "selectchanged" ) {
		if (msg.pSender == m_optSelected) {
			OnReaderSwitch();
		}
	}
	else if ( msg.sType == "textchanged" ) {
		if (msg.pSender == m_cstBodyPart) {
			OnReaderNameChanged();
		}
	}
}

void CReaderUI::OnReaderSwitch() {
	bool bRet = m_optSelected->IsSelected();
	DWORD i = GetGridIndex();
	DWORD j = GetReaderIndex();

	assert(i >= 0 && i < MAX_READERS_PER_GRID);
	assert(j >= 0 && j < MAX_READERS_PER_GRID);

	g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch = bRet ? TRUE : FALSE;

	SaveReaderSwitch(i, j);
}

void CReaderUI::OnReaderNameChanged() {
	DWORD i = GetGridIndex();
	DWORD j = GetReaderIndex();
	assert(i >= 0 && i < MAX_READERS_PER_GRID);
	assert(j >= 0 && j < MAX_READERS_PER_GRID);

	CDuiString  strText = m_cstBodyPart->GetText();
	STRNCPY(g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_szReaderName, strText, MAX_READER_NAME_LENGTH);
	SaveReaderName(i, j);
}

DWORD CReaderUI::GetGridIndex() {
	CControlUI * pParent = GetParent();
	DWORD  i = 0;

	while (pParent && i < 5) {
		pParent = pParent->GetParent();
		i++;
	}

	if (pParent)
		return pParent->GetTag();
	else {
		assert(0);
		return 0;
	}
}

DWORD CReaderUI::GetReaderIndex() {
	return GetTag() - 1;
}

void  CReaderUI::SetReaderStatus(BOOL bConnected) {

	if ( bConnected ) {
		m_state->SetBkImage("");
	}
	else {
		m_state->SetBkImage("disconnected.png");
	}
}