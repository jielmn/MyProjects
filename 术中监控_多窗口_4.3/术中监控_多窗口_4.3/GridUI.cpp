#include "GridUI.h"

CGridUI::CGridUI() :m_callback(m_pManager) {
	m_bInited = FALSE;
	m_tabs = 0;

	m_btnBedNo = 0;
	m_btnBedNoM = 0;
	m_dwBedNo = 0;

	m_lblReaderNo = 0;

	m_layReaders = 0;
	memset(m_readers, 0, sizeof(m_readers));

	m_cstModeBtn = 0;
	m_hand_reader = 0;
	m_CurReaderState = 0;

	memset(m_aLastTemp, 0, sizeof(m_aLastTemp));
	m_dwSelSurReaderIndex = 0;

	m_cstImgLabel = 0;
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
		m_readers[i]->SetName(LAY_READER);
		m_layReaders->Add(m_readers[i]);
	}

	m_hand_reader = new CReaderUI;
	m_hand_reader->SetTag(0);
	m_hand_reader->SetFixedHeight(100);       
	m_layReaders->Add(m_hand_reader);	

	m_cstImgLabel = static_cast<CImageLabelUI *>(m_pManager->FindControl(CST_IMG_LBL_TEMP));

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
		m_dwSelSurReaderIndex = 0;
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
		
		OnSurReaderSelected(0);
	}
	break;

	case CModeButton::Mode_Multiple:
	{
		m_hand_reader->SetVisible(false);
		int  nSel = -1;
		for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
			m_readers[i]->SetVisible(true);
			if (0==i)
				m_readers[i]->m_optSelected->SetVisible(true);

			if (g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_bSwitch) {
				m_readers[i]->m_optSelected->Selected(true);
				if (nSel < 0)
					nSel = i;
			}
			else {
				m_readers[i]->m_optSelected->Selected(false);
			}
			m_readers[i]->m_cstBodyPart->SetText(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_szReaderName);
		}
		m_CurReaderState->SetVisible(true);
		
		if (nSel < 0)
			nSel = 0;
		OnSurReaderSelected(nSel);
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

// 设置术中读卡器的连接状态
void  CGridUI::SetSurReaderStatus(DWORD j, BOOL bConnected) {
	// 如果是手持状态
	if (m_cstModeBtn->GetMode() == CModeButton::Mode_Hand) {
		return;
	}

	assert(m_dwSelSurReaderIndex >= 1);
	m_readers[j]->SetReaderStatus(bConnected);

	// 如果当前选中的Reader Index和数据的index一致
	if (m_dwSelSurReaderIndex == j + 1) {
		SetCurReaderStatus(bConnected);
	}
}

void CGridUI::SetCurReaderStatus(BOOL bConnected) {
	if (bConnected)
		m_CurReaderState->SetBkImage("");
	else
		m_CurReaderState->SetBkImage("disconnected.png");
}

// 术中读卡器温度
void  CGridUI::OnSurReaderTemp(DWORD j, const TempItem & item) {
	// 如果是手持状态
	if (m_cstModeBtn->GetMode() == CModeButton::Mode_Hand) {
		return;
	}

	assert(m_dwSelSurReaderIndex >= 1);
	memcpy( &m_aLastTemp[j], &item, sizeof(TempItem) );

	SetReaderTemp(j, item.m_dwTemp);

	// 如果当前选中的Reader Index和数据的index一致
	if (m_dwSelSurReaderIndex == j + 1) {
		SetCurReaderTemp(item.m_dwTemp);
	}
}

void CGridUI::SetCurReaderTemp(DWORD  dwTemp) {
	CDuiString  strText;
	strText.Format("%.2f", dwTemp / 100.0);
	m_cstImgLabel->SetText(strText);
}

void CGridUI::SetReaderTemp(DWORD j, DWORD  dwTemp) {
	CDuiString  strText;
	strText.Format("%.2f", dwTemp / 100.0);
	m_readers[j]->m_lblTemp->SetText(strText);
}

// 选中读卡器
void CGridUI::OnSurReaderSelected(DWORD  dwSelected) {
	assert(dwSelected < MAX_READERS_PER_GRID);
	for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
		if (j == dwSelected) {
			m_readers[j]->SetBkColor(SELECTED_READER_BG_COLOR);
		}
		else {
			m_readers[j]->SetBkColor(0);
		}
	}

	m_dwSelSurReaderIndex = dwSelected + 1;

	CDuiString strText;
	strText.Format("%c", ('A' + (char)dwSelected));
	m_lblReaderNo->SetText(strText);

	m_CurReaderState->SetBkImage(m_readers[dwSelected]->m_state->GetBkImage());

	m_cstImgLabel->SetText(m_readers[dwSelected]->m_lblTemp->GetText());
}