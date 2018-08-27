#include "SettingDlg.h"

#define EIDT_TEXT_COLOR    0xFF386382
#define EDIT_FONT_INDEX    2

#define COMMON_ITEMS_COUNT   4
#define GRID_SETTING_ITEMS_COUNT  3

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
	strText.Format("读卡器");
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

	// 1. 病区号
	pCombo = new CComboUI;
	AddComboItem(pCombo, "未设置", 0);

	std::vector<TArea *>::iterator it;
	int i = 0;
	int nIndex = 0;
	for (it = g_vArea.begin(); it != g_vArea.end(); ++it, ++i) {
		TArea * pArea = *it;

		strText.Format("%s(编号：%lu)", pArea->szAreaName, pArea->dwAreaNo);
		AddComboItem( pCombo, strText, pArea->dwAreaNo );

		if (g_data.m_dwAreaNo == pArea->dwAreaNo) {
			nIndex = i + 1;
		}
	}
	pCombo->SelectItem(nIndex);
	SetComboStyle(pCombo);
	m_tree->AddNode(AREA_NO_TEXT, pTitleNode, 0, pCombo);

	// 2. 采集间隔
	pCombo = new CComboUI;
	AddComboItem(pCombo, "10秒", 10);
	AddComboItem(pCombo, "1分钟", 60);
	AddComboItem(pCombo, "5分钟", 300);
	AddComboItem(pCombo, "15分钟", 900);
	AddComboItem(pCombo, "30分钟", 1800);
	AddComboItem(pCombo, "1小时", 3600);
	if (g_data.m_dwCollectInterval <= 10) {
		pCombo->SelectItem(0);
	}
	else if(g_data.m_dwCollectInterval <= 60 ) {
		pCombo->SelectItem(1);
	}
	else if (g_data.m_dwCollectInterval <= 300 ) {
		pCombo->SelectItem(2);
	}
	else if (g_data.m_dwCollectInterval <= 900 ) {
		pCombo->SelectItem(3);
	}
	else if (g_data.m_dwCollectInterval <= 1800 ) {
		pCombo->SelectItem(4);
	}
	else {
		pCombo->SelectItem(5);
	}
	SetComboStyle(pCombo);
	m_tree->AddNode("采集体温间隔", pTitleNode, 0, pCombo);


	// 3. 显示最低温度
	pCombo = new CComboUI;
	AddComboItem(pCombo, "20℃", 20);
	AddComboItem(pCombo, "24℃", 24);
	AddComboItem(pCombo, "28℃", 28);
	AddComboItem(pCombo, "32℃", 32);

	if ( g_data.m_dwMyImageMinTemp <= 20) {
		pCombo->SelectItem(0);
	}
	else if (g_data.m_dwMyImageMinTemp <= 24) {
		pCombo->SelectItem(1);
	}
	else if (g_data.m_dwMyImageMinTemp <= 28) {
		pCombo->SelectItem(2);
	}
	else {
		pCombo->SelectItem(3);
	}
	SetComboStyle(pCombo);
	m_tree->AddNode("显示的最低温度", pTitleNode, 0, pCombo);

	// 4. 报警声音开关
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
	strText.Format("读卡器%lu参数设置", dwIndex + 1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode);

	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;

	// 低温报警
	CEditUI * pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%.2f", g_data.m_dwLowTempAlarm[dwIndex] / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode(LOW_ALARM_TEXT, pSubTitleNode, 0, pEdit);

	// 高温报警
	pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%.2f", g_data.m_dwHighTempAlarm[dwIndex] / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode(HIGH_ALARM_TEXT, pSubTitleNode, 0, pEdit);

	// 床号
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
	DWORD       dwLowAlarm = 0;
	DWORD       dwHighAlarm = 0;
	DWORD       dwBedNo = 0;

	if ( !GetCommonConfig() ) {
		return;
	}

	for (int i = 0; i < MAX_READERS_COUNT; i++) {
		if ( !GetConfig(i, dwLowAlarm, dwHighAlarm, dwBedNo ) ) {
			return;
		}
		else {
			m_dwLowTempAlarm[i] = dwLowAlarm;
			m_dwHighTempAlarm[i] = dwHighAlarm;
			m_dwBedNo[i] = dwBedNo;
		}
	}

	DuiLib::CDuiString  strText;
	DWORD dwCount = MAX_READERS_COUNT;
	for ( DWORD i = 0; i < dwCount; ++i ) {
		if (m_dwBedNo[i] > 0) {
			for (DWORD j = i + 1; j < dwCount; ++j) {
				// 如果有重复的bed no
				if (m_dwBedNo[i] == m_dwBedNo[j]) {
					strText.Format("读卡器%lu和%lu的Reader相关床号配置相同", i + 1, j + 1);
					::MessageBox(GetHWND(), strText, "配置", 0);
					return;
				}
			}
		}		
	}

	g_data.m_bAlarmVoiceOff = m_bAlarmVoiceOff;
	g_data.m_dwAreaNo = m_dwAreaNo;
	g_data.m_dwMyImageMinTemp = m_dwMyImageMinTemp;
	g_data.m_dwCollectInterval = m_dwCollectInterval;

	memcpy( g_data.m_dwLowTempAlarm,  m_dwLowTempAlarm,  sizeof(DWORD) * MAX_READERS_COUNT);
	memcpy( g_data.m_dwHighTempAlarm, m_dwHighTempAlarm, sizeof(DWORD) * MAX_READERS_COUNT);
	memcpy( g_data.m_dwBedNo,         m_dwBedNo,         sizeof(DWORD) * MAX_READERS_COUNT);

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
	int  nAreaNo = 0;
	int  nCollectInterval = 0;
	int  nMinTemp = 0;
	BOOL bAlarmVoiceOff = FALSE;

	bGetCfg = m_tree->GetConfigValue( 1, cfgValue );
	nAreaNo = cfgValue.m_tag;

	bGetCfg = m_tree->GetConfigValue(2, cfgValue);
	nCollectInterval = cfgValue.m_tag;

	bGetCfg = m_tree->GetConfigValue(3, cfgValue);
	nMinTemp = cfgValue.m_tag;

	bGetCfg = m_tree->GetConfigValue(4, cfgValue);
	if (0 == cfgValue.m_nComboSel) {
		bAlarmVoiceOff = FALSE;
	}
	else  {
		bAlarmVoiceOff = TRUE;
	}

	m_bAlarmVoiceOff = bAlarmVoiceOff; 
	m_dwAreaNo = nAreaNo;
	m_dwCollectInterval = nCollectInterval;
	m_dwMyImageMinTemp = nMinTemp;

	return TRUE;
}

BOOL  CSettingDlg::GetConfig(int nIndex, DWORD & dwLowAlarm, DWORD & dwHighAlarm, DWORD & dwBedNo  )
{
	CMyTreeCfgUI::ConfigValue  cfgValue;
	CDuiString  strText;
	double dbTemp = 0.0;
	int    nBedNo = 0;
	bool   bGetCfg = false;

	bGetCfg = m_tree->GetConfigValue( (COMMON_ITEMS_COUNT + 2)  + nIndex * (GRID_SETTING_ITEMS_COUNT + 1) + 1, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%lf", &dbTemp)) {
		strText.Format("读卡器%d配置，低温报警请输入数字", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	if (dbTemp > 42 || dbTemp < 20) {
		strText.Format("读卡器%d配置，低温报警请输入范围(20~42)", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	dwLowAlarm = (DWORD)(dbTemp * 100);

	bGetCfg = m_tree->GetConfigValue((COMMON_ITEMS_COUNT + 2) + nIndex * (GRID_SETTING_ITEMS_COUNT + 1) + 2, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%lf", &dbTemp)) {
		strText.Format("读卡器%d配置，高温报警请输入数字", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	if (dbTemp > 42 || dbTemp < 20) {
		strText.Format("读卡器%d配置，高温报警请输入范围(20~42)", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	if (dbTemp < dwLowAlarm / 100.0) {
		strText.Format("读卡器%d配置，高温报警请大于低温报警", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	dwHighAlarm = (DWORD)(dbTemp * 100);

	bGetCfg = m_tree->GetConfigValue((COMMON_ITEMS_COUNT + 2) + nIndex * (GRID_SETTING_ITEMS_COUNT + 1) + 3, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%d", &nBedNo)) {
		strText.Format("读卡器%d配置，Reader相关床位号请输入数字", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	if ( nBedNo < 0 || nBedNo > MAX_BED_ID ) {
		strText.Format("读卡器%d配置，Reader相关床位号的范围是0到200(其中0表示没有关联的Reader)", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	dwBedNo = nBedNo; 

	return TRUE;
}