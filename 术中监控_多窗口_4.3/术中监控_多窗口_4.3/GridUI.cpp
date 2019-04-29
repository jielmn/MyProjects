#include "GridUI.h"

CGridUI::CGridUI() :m_callback(m_pManager) {
	m_bInited = FALSE;
	m_tabs = 0;

	m_btnBedNo = 0;
	m_btnBedNoM = 0;
	m_dwBedNo = 0;

	m_lblReaderNo = 0;
	m_dwReaderNo = 0;

	m_layReaders = 0;
	memset(m_readers, 0, sizeof(m_readers));

	m_cstModeBtn = 0;
	m_hand_reader = 0;
	m_CurReaderState = 0;
}

CGridUI::~CGridUI() {

}

LPCTSTR CGridUI::GetClass() const {
	return "GridUI";
}

void CGridUI::DoInit() {
	if (m_bInited) {
		return;
	}

	CDialogBuilder builder;
	if ( m_callback.m_pManager == 0 )
		m_callback.m_pManager = m_pManager;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(GRID_XML_FILE, (UINT)0, &m_callback, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
		m_pManager->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		m_bInited = TRUE;
		return;
	}

	CDuiString  strText;

	m_tabs = static_cast<DuiLib::CTabLayoutUI  *>(m_pManager->FindControl(GRID_TABS));

	m_btnBedNo = static_cast<DuiLib::CButtonUI *>(m_pManager->FindControl(BTN_BED_NO));
	m_btnBedNoM = static_cast<DuiLib::CButtonUI *>(m_pManager->FindControl(BTN_BED_NO_M));

	strText.Format("%02u", m_dwBedNo);
	m_btnBedNo->SetText(strText);
	m_btnBedNoM->SetText(strText);
	
	m_lblReaderNo = static_cast<DuiLib::CLabelUI *>(m_pManager->FindControl(LBL_READER_NO));	
	m_lblReaderNo->SetText("");

	m_CurReaderState = m_pManager->FindControl(CTR_CUR_READER_STATE);
	m_CurReaderState->SetVisible(false);

	m_cstModeBtn = static_cast<CModeButton *>(m_pManager->FindControl(MODE_BUTTON));

	// 格子模式
	strText = this->GetCustomAttribute("GridMode");
	DWORD dwMode = 0;
	sscanf(strText, "%lu", &dwMode);
	m_cstModeBtn->SetMode((CModeButton::Mode)dwMode);

	// A~F 6个连续测温读卡器
	m_layReaders = static_cast<DuiLib::CVerticalLayoutUI *>(m_pManager->FindControl(LAY_READERS));
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		m_readers[i] = new CReaderUI(CReaderUI::Surgency);
		m_readers[i]->SetTag(i+1);
		m_readers[i]->SetIndicator(i);    
		m_readers[i]->SetFixedHeight(100);
		m_layReaders->Add(m_readers[i]);
	}

	m_hand_reader = new CReaderUI;
	m_hand_reader->SetTag(0);
	m_hand_reader->SetFixedHeight(100);       
	m_layReaders->Add(m_hand_reader);	

	OnModeChanged();

	m_bInited = TRUE;
}

void CGridUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void  CGridUI::SetBedNo(DWORD dwIndex) {
	m_dwBedNo = dwIndex;

	if (m_btnBedNo) {
		CDuiString  strText;
		strText.Format("%02u", m_dwBedNo);
		m_btnBedNo->SetText(strText);
	}
}

// 双击后的视图切换
void  CGridUI::SwitchView() {
	int nSel = m_tabs->GetCurSel();

	if (0 == nSel) {
		m_tabs->SelectItem(1);
	}
	else {
		m_tabs->SelectItem(0);
	}
}

void CGridUI::Notify(TNotifyUI& msg) {
	if (msg.sType == "ModeChanged") {
		if (msg.pSender == m_cstModeBtn) {
			OnModeChanged();
		}
	}
}

void CGridUI::OnModeChanged() {
	DWORD  dwIndex = GetTag();

	switch (m_cstModeBtn->GetMode())
	{
	case CModeButton::Mode_Hand:
	{
		m_hand_reader->SetVisible(true);
		m_hand_reader->m_cstBodyPart->SetText(g_data.m_CfgData.m_GridCfg[dwIndex].m_HandReaderCfg.m_szReaderName);
		for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
			m_readers[i]->SetVisible(false);
		}
		m_CurReaderState->SetVisible(false);

		m_lblReaderNo->SetText("");
	}
	break;

	case CModeButton::Mode_Single:
	{
		m_hand_reader->SetVisible(false);
		for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
			if ( 0 == i ) {
				m_readers[i]->SetVisible(true);
				m_readers[i]->m_optSelected->SetVisible(false);
				m_readers[i]->m_cstBodyPart->SetText(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_szReaderName);
			}				
			else
				m_readers[i]->SetVisible(false);
		}
		m_CurReaderState->SetVisible(true);

		CDuiString strText;
		strText.Format("%c", ('A' + (char)m_dwReaderNo));
		m_lblReaderNo->SetText(strText);
	}
	break;

	case CModeButton::Mode_Multiple:
	{
		m_hand_reader->SetVisible(false);
		for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
			m_readers[i]->SetVisible(true);
			if (0==i)
				m_readers[i]->m_optSelected->SetVisible(true);
			m_readers[i]->m_optSelected->Selected(
				g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_bSwitch ? true : false, false );
			m_readers[i]->m_cstBodyPart->SetText(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_szReaderName);
		}
		m_CurReaderState->SetVisible(true);

		CDuiString strText;
		strText.Format("%c", ('A' + (char)m_dwReaderNo));
		m_lblReaderNo->SetText(strText);
	}
	break;

	default:
		break;
	}

	CDuiString  strText;	
	strText.Format("%s %lu", CFG_GRID_MODE, dwIndex+1);

	g_data.m_CfgData.m_GridCfg[dwIndex].m_dwGridMode = (DWORD)m_cstModeBtn->GetMode();
	DWORD dwDefValue = CModeButton::Mode_Hand;
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[dwIndex].m_dwGridMode, &dwDefValue);
}

void  CGridUI::SetMode(CModeButton::Mode e) {
	CDuiString strText;
	strText.Format("%lu", (DWORD)e);
	this->SetAttribute("GridMode", strText);
}