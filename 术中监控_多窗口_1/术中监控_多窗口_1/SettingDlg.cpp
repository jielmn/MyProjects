#include "SettingDlg.h"

#define EIDT_TEXT_COLOR    0xFF386382
#define EDIT_FONT_INDEX    2

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
	strText.Format(GRIDS_SETTING_TEXT);
	pTitleNode = m_tree->AddNode(strText);

	for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
		InitGridCfg(pTitleNode, i);
	}

	m_tree->SetMinHeight(7960);
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
	strText.Format(COMMON_SETTING_TEXT);
	pTitleNode = m_tree->AddNode(strText);

	CEditUI * pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%lu", g_dwLayoutColumns);
	pEdit->SetText(strText);
	m_tree->AddNode(GRIDS_COLUMNS_TEXT, pTitleNode, 0, pEdit);

	pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%lu", g_dwLayoutRows);
	pEdit->SetText(strText);
	m_tree->AddNode(GRIDS_ROWS_TEXT, pTitleNode, 0, pEdit);

	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;

	// 皮肤
	pCombo = new CComboUI;
	AddComboItem(pCombo, SKIN_BLACK_TEXT);
	AddComboItem(pCombo, SKIN_WHITE_TEXT);
	if ( SKIN_BLACK == g_dwSkinIndex ) {
		pCombo->SelectItem(0);
	}
	else if(SKIN_WHITE == g_dwSkinIndex) {
		pCombo->SelectItem(1);
	}
	else {
		pCombo->SelectItem(0);
	}

	SetComboStyle(pCombo);
	m_tree->AddNode(ALARM_VOICE_SWITCH_TEXT, pTitleNode, 0, pCombo);

	// 报警声音开关
	pCombo = new CComboUI;
	AddComboItem(pCombo, SWITCH_ON_TEXT);
	AddComboItem(pCombo, SWITCH_OFF_TEXT);
	if ( g_bAlarmVoiceOff ) {
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
	strText.Format("窗格%lu参数设置", dwIndex + 1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode);

	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;

	// 间隔时间
	pCombo = new CComboUI;

	AddComboItem(pCombo, "10秒");
	AddComboItem(pCombo, "1分钟");
	AddComboItem(pCombo, "5分钟");
	AddComboItem(pCombo, "15分钟");
	AddComboItem(pCombo, "30分钟");
	AddComboItem(pCombo, "1小时");

	// comob 选定index
	int nSelIndex = 0;
	if (g_dwCollectInterval[dwIndex] <= 10) {
		nSelIndex = 0;
	}
	else if (g_dwCollectInterval[dwIndex] <= 60) {
		nSelIndex = 1;
	}
	else if (g_dwCollectInterval[dwIndex] <= 300) {
		nSelIndex = 2;
	}
	else if (g_dwCollectInterval[dwIndex] <= 900) {
		nSelIndex = 3;
	}
	else if (g_dwCollectInterval[dwIndex] <= 1800) {
		nSelIndex = 4;
	}
	else {
		nSelIndex = 5;
	}
	pCombo->SelectItem(nSelIndex);

	SetComboStyle(pCombo);
	m_tree->AddNode(COLLECT_INTERVAL_TEXT, pSubTitleNode, 0, pCombo);


	// 显示最低温度
	pCombo = new CComboUI;
	AddComboItem(pCombo, "20℃");
	AddComboItem(pCombo, "24℃");
	AddComboItem(pCombo, "28℃");
	AddComboItem(pCombo, "32℃");

	// 选定index
	nSelIndex = 0;
	if (g_dwMyImageMinTemp[dwIndex] <= 20) {
		nSelIndex = 0;
	}
	else if (g_dwMyImageMinTemp[dwIndex] <= 24) {
		nSelIndex = 1;
	}
	else if (g_dwMyImageMinTemp[dwIndex] <= 28) {
		nSelIndex = 2;
	}
	else {
		nSelIndex = 3;
	}
	pCombo->SelectItem(nSelIndex);

	SetComboStyle(pCombo);
	m_tree->AddNode(MIN_TEMPERATURE_TEXT, pSubTitleNode, 0, pCombo);

	// 低温报警
	CEditUI * pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%.2f", g_dwLowTempAlarm[dwIndex] / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode(LOW_ALARM_TEXT, pSubTitleNode, 0, pEdit);

	// 高温报警
	pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%.2f", g_dwHighTempAlarm[dwIndex] / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode(HIGH_ALARM_TEXT, pSubTitleNode, 0, pEdit);

	// 串口
	std::vector<std::string>  vCom;
	GetAllSerialPortName(vCom);

	std::vector<std::string>::iterator it;
	if (g_szComPort[dwIndex][0] != 0) {
		for (it = vCom.begin(); it != vCom.end(); it++) {
			std::string & s = *it;
			if (0 == StrICmp(s.c_str(), g_szComPort[dwIndex])) {
				break;
			}
		}
		if (it == vCom.end()) {
			vCom.push_back(g_szComPort[dwIndex]);
		}
	}

	pCombo = new CComboUI;
	AddComboItem( pCombo, ANY_COM_PORT_TEXT);

	for (it = vCom.begin(); it != vCom.end(); it++) {
		std::string & s = *it;
		AddComboItem(pCombo, s.c_str());
	}

	if (g_szComPort[dwIndex][0] == 0) {
		pCombo->SelectItem(0);
	}
	else {
		for (it = vCom.begin(); it != vCom.end(); it++) {
			std::string & s = *it;
			if (0 == StrICmp(s.c_str(), g_szComPort[dwIndex])) {
				pCombo->SelectItem((it - vCom.begin()) + 1);
			}
		}
	}

	SetComboStyle(pCombo);
	m_tree->AddNode(RW_COM_PORT_TEXT, pSubTitleNode, 0, pCombo);
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

void  CSettingDlg::SetComboStyle(CComboUI * pCombo) {
	pCombo->SetItemTextColor(0xFF386382);
	pCombo->SetHotItemTextColor(0xFF386382);
	pCombo->SetSelectedItemTextColor(0xFF386382);
	pCombo->SetItemFont(2);
	pCombo->SetAttributeList(" normalimage=\"file = 'Combo_nor.bmp' corner = '2,2,24,2'\" hotimage=\"file = 'Combo_over.bmp' "
		" corner = '2,2,24,2'\" pushedimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" textpadding=\"5, 1, 5, 1\" ");
}

void  CSettingDlg::OnBtnOk(DuiLib::TNotifyUI& msg) {

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
				}
			}
		}
		pFindControl = pFindControl->GetParent();
		nDepth++;
	}
}