#include "SettingDlg.h"

#define EIDT_TEXT_COLOR    0xFF386382
#define EDIT_FONT_INDEX    2
#define GRID_SETTING_ITEMS_COUNT  7

CSettingDlg::CSettingDlg() {

}


void   CSettingDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();

	if (msg.sType == "click") {
		if (name == BUTTON_OK_NAME) {
			OnBtnOk(msg);
		}
		else {
			OnMyClick(msg);
		}
	}
	WindowImplBase::Notify(msg); 
}

void   CSettingDlg::InitWindow() {
	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl(MYTREE_CONFIG_NAME);
	assert(m_tree);

	InitCommonCfg();

	DuiLib::CDuiString  strText;
	CMyTreeCfgUI::Node* pTitleNode = NULL;
	strText.Format("∂¡ø®∆˜");
	pTitleNode = m_tree->AddNode(strText);

	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		InitGridCfg(pTitleNode, i);
	}

	int nHeight = m_tree->CalculateMinHeight();
	m_tree->SetFixedHeight(nHeight);
	WindowImplBase::InitWindow();   
}

LRESULT  CSettingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CSettingDlg::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, MYTREE_CLASS_NAME)) {
		return new CMyTreeCfgUI();
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void  CSettingDlg::InitCommonCfg() {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pTitleNode = NULL;
	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;

	strText.Format(COMMON_SETTING_TEXT);
	pTitleNode = m_tree->AddNode(strText);

	// 1. ≤°«¯∫≈
	pCombo = new CComboUI;
	AddComboItem(pCombo, "Œ¥…Ë÷√", 0);

	std::vector<TArea *>::iterator it;
	int i = 0;
	int nIndex = 0;
	for (it = g_vArea.begin(); it != g_vArea.end(); ++it, ++i) {
		TArea * pArea = *it;

		strText.Format("%s(±‡∫≈£∫%lu)", pArea->szAreaName, pArea->dwAreaNo);
		AddComboItem( pCombo, strText, pArea->dwAreaNo );

		if (g_data.m_dwAreaNo == pArea->dwAreaNo) {
			nIndex = i + 1;
		}
	}
	pCombo->SelectItem(nIndex);
	SetComboStyle(pCombo);
	m_tree->AddNode(AREA_NO_TEXT, pTitleNode, 0, pCombo);

	// 2. ±®æØ…˘“Ùø™πÿ
	pCombo = new CComboUI;
	AddComboItem(pCombo, SWITCH_ON_TEXT);
	AddComboItem(pCombo, SWITCH_OFF_TEXT);
	if ( g_data.m_bAlarmVoiceOff ) {
		pCombo->SelectItem(1);
	}
	else {
		pCombo->SelectItem(0);
	}	

	SetComboStyle(pCombo);
	m_tree->AddNode(ALARM_VOICE_SWITCH_TEXT, pTitleNode, 0, pCombo);
}

void   CSettingDlg::InitGridCfg(CMyTreeCfgUI::Node* pTitleNode, DWORD dwIndex) {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pSubTitleNode = NULL;
	strText.Format("∂¡ø®∆˜%lu≤Œ ˝…Ë÷√", dwIndex + 1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode);

	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;

	// ∆Ù”√/Ω˚”√
	pCombo = new CComboUI;
	AddComboItem(pCombo, "∆Ù”√");
	AddComboItem(pCombo, "Ω˚”√");
	if ( g_data.m_bReaderSwitch[dwIndex] ) {
		pCombo->SelectItem(0);
	}
	else {
		pCombo->SelectItem(1);
	}
	SetComboStyle(pCombo);
	m_tree->AddNode(" «∑Ò∆Ù”√", pSubTitleNode, 0, pCombo);

	// µÕŒ¬±®æØ
	CEditUI * pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%.2f", g_data.m_dwLowTempAlarm[dwIndex] / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode(LOW_ALARM_TEXT, pSubTitleNode, 0, pEdit);

	// ∏ﬂŒ¬±®æØ
	pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%.2f", g_data.m_dwHighTempAlarm[dwIndex] / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode(HIGH_ALARM_TEXT, pSubTitleNode, 0, pEdit);

	// ¥≤∫≈
	pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%lu", g_data.m_dwBedNo[dwIndex]);
	pEdit->SetText(strText);
	m_tree->AddNode(READER_ID_TEXT, pSubTitleNode, 0, pEdit);
}

void  CSettingDlg::SetEditStyle(CEditUI * pEdit) {
	pEdit->SetTextColor(EIDT_TEXT_COLOR);
	pEdit->SetFont(EDIT_FONT_INDEX);
}

void  CSettingDlg::AddComboItem(CComboUI * pCombo, const char * szItem) {
	CListLabelElementUI * pElement = new CListLabelElementUI;
	pElement->SetText(szItem);
	pCombo->Add(pElement);
}

void  CSettingDlg::AddComboItem(CComboUI * pCombo, const char * szItem, UINT_PTR tag) {
	CListLabelElementUI * pElement = new CListLabelElementUI;
	pElement->SetText(szItem);
	pElement->SetTag(tag);
	pCombo->Add(pElement);
}

void  CSettingDlg::SetComboStyle(CComboUI * pCombo) {
	pCombo->SetItemTextColor(0xFF386382);
	pCombo->SetHotItemTextColor(0xFF386382);
	pCombo->SetSelectedItemTextColor(0xFF386382);
	pCombo->SetItemFont(2);
	pCombo->SetAttributeList(" normalimage=\"file = 'Combo_nor.bmp' corner = '2,2,24,2'\" hotimage=\"file = 'Combo_over.bmp' "
		" corner = '2,2,24,2'\" pushedimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" textpadding=\"5, 1, 5, 1\" ");
}

void  CSettingDlg::OnBtnOk(DuiLib::TNotifyUI& msg) {
	DWORD       dwInterval = 0;
	DWORD       dwLowAlarm = 0;
	DWORD       dwHighAlarm = 0;
	DWORD       dwMinTemp = 0;
	DWORD       dwBedNo = 0;
	BOOL        bSwitch = TRUE;

	return;

	if ( !GetCommonConfig() ) {
		return;
	}

	for (int i = 0; i < MAX_READERS_COUNT; i++) {
		if ( !GetConfig(i, dwLowAlarm, dwHighAlarm, dwBedNo, bSwitch) ) {
			return;
		}
		else {
			m_dwLowTempAlarm[i] = dwLowAlarm;
			m_dwHighTempAlarm[i] = dwHighAlarm;
			m_dwBedNo[i] = dwBedNo;
			m_bReaderSwitch[i] = bSwitch;
		}
	}

	DuiLib::CDuiString  strText;
	DWORD dwCount = MAX_READERS_COUNT;
	for ( DWORD i = 0; i < dwCount; ++i ) {
		if (m_dwBedNo[i] > 0) {
			for (DWORD j = i + 1; j < dwCount; ++j) {
				// »Áπ˚”–÷ÿ∏¥µƒbed no
				if (m_dwBedNo[i] == m_dwBedNo[j]) {
					strText.Format("∂¡ø®∆˜%lu∫Õ%luµƒReaderœ‡πÿ¥≤∫≈≈‰÷√œ‡Õ¨", i + 1, j + 1);
					::MessageBox(GetHWND(), strText, "≈‰÷√", 0);
					return;
				}
			}
		}		
	}

	PostMessage(WM_CLOSE);
}

void  CSettingDlg::OnMyClick(DuiLib::TNotifyUI& msg) {
	CControlUI* pFindControl = msg.pSender;
	DuiLib::CDuiString strName;
	int nDepth = 0;

	while (pFindControl) {
		strName = pFindControl->GetName();
		if (0 == StrICmp((const char *)strName, MYTREE_CONFIG_NAME)) {
			assert(nDepth == 4);
			int index = m_tree->GetItemIndex(msg.pSender);
			if (index != -1) {
				if (_tcscmp(msg.pSender->GetClass(), DUI_CTR_BUTTON) == 0) {
					CMyTreeCfgUI::Node* node = (CMyTreeCfgUI::Node*)msg.pSender->GetParent()->GetParent()->GetTag();
					m_tree->ExpandNode(node, !node->data()._expand);
					int nHeight = m_tree->CalculateMinHeight();
					m_tree->SetFixedHeight(nHeight);
				}
			}
		}
		pFindControl = pFindControl->GetParent();
		nDepth++;
	}
}

BOOL  CSettingDlg::GetCommonConfig() {
	CDuiString  strText;
	CMyTreeCfgUI::ConfigValue  cfgValue;
	bool bGetCfg = false;
	int  nCols = 0;
	int  nRows = 0;
	int  nSkin = 0;
	int  nAreaNo = 0;
	BOOL bAlarmVoiceOff = FALSE;

	bGetCfg = m_tree->GetConfigValue( 1, cfgValue );
	nAreaNo = cfgValue.m_tag;

	bGetCfg = m_tree->GetConfigValue(2, cfgValue);
	if (0 == cfgValue.m_nComboSel) {
		bAlarmVoiceOff = FALSE;
	}
	else  {
		bAlarmVoiceOff = TRUE;
	}

	m_bAlarmVoiceOff = bAlarmVoiceOff; 
	m_dwAreaNo = nAreaNo;

	return TRUE;
}

BOOL  CSettingDlg::GetConfig(int nIndex, DWORD & dwLowAlarm,
	DWORD & dwHighAlarm, DWORD & dwBedNo, BOOL & bSwitch )
{
	CMyTreeCfgUI::ConfigValue  cfgValue;
	CDuiString  strText;
	double dbTemp = 0.0;
	int    nBedNo = 0;
	bool   bGetCfg = false;

	bGetCfg = m_tree->GetConfigValue(7 + nIndex * GRID_SETTING_ITEMS_COUNT + 1, cfgValue);
	switch (cfgValue.m_nComboSel)
	{
	case 0:
		bSwitch = TRUE;
		break;

	case 1:
		bSwitch = FALSE;
		break;

	default:
		break;
	}

	bGetCfg = m_tree->GetConfigValue( 7 + nIndex * GRID_SETTING_ITEMS_COUNT + 4, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%lf", &dbTemp)) {
		strText.Format("∂¡ø®∆˜%d≈‰÷√£¨µÕŒ¬±®æØ«Î ‰»Î ˝◊÷", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "…Ë÷√", 0);
		return FALSE;
	}

	if (dbTemp > 42 || dbTemp < 20) {
		strText.Format("∂¡ø®∆˜%d≈‰÷√£¨µÕŒ¬±®æØ«Î ‰»Î∑∂Œß(20~42)", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "…Ë÷√", 0);
		return FALSE;
	}
	dwLowAlarm = (DWORD)(dbTemp * 100);

	bGetCfg = m_tree->GetConfigValue( 7 + nIndex * GRID_SETTING_ITEMS_COUNT + 5, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%lf", &dbTemp)) {
		strText.Format("∂¡ø®∆˜%d≈‰÷√£¨∏ﬂŒ¬±®æØ«Î ‰»Î ˝◊÷", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "…Ë÷√", 0);
		return FALSE;
	}

	if (dbTemp > 42 || dbTemp < 20) {
		strText.Format("∂¡ø®∆˜%d≈‰÷√£¨∏ﬂŒ¬±®æØ«Î ‰»Î∑∂Œß(20~42)", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "…Ë÷√", 0);
		return FALSE;
	}
	if (dbTemp < dwLowAlarm / 100.0) {
		strText.Format("∂¡ø®∆˜%d≈‰÷√£¨∏ﬂŒ¬±®æØ«Î¥Û”⁄µÕŒ¬±®æØ", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "…Ë÷√", 0);
		return FALSE;
	}
	dwHighAlarm = (DWORD)(dbTemp * 100);

	bGetCfg = m_tree->GetConfigValue(7 + nIndex * GRID_SETTING_ITEMS_COUNT + 6, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%d", &nBedNo)) {
		strText.Format("¥∞ø⁄%d≈‰÷√£¨Readerœ‡πÿ¥≤Œª∫≈«Î ‰»Î ˝◊÷", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "…Ë÷√", 0);
		return FALSE;
	}

	if ( nBedNo < 0 || nBedNo > MAX_BED_ID ) {
		strText.Format("∂¡ø®∆˜%d≈‰÷√£¨Readerœ‡πÿ¥≤Œª∫≈µƒ∑∂Œß «0µΩ200(∆‰÷–0±Ì æ√ª”–πÿ¡™µƒReader)", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "…Ë÷√", 0);
		return FALSE;
	}
	dwBedNo = nBedNo; 

	return TRUE;
}