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
	DWORD       dwInterval = 0;
	DWORD       dwLowAlarm = 0;
	DWORD       dwHighAlarm = 0;
	DWORD       dwMinTemp = 0;
	CDuiString  strComPort;

	if ( !GetCommonConfig() ) {
		return;
	}

	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		if ( !GetConfig(i, dwInterval, dwLowAlarm, dwHighAlarm, dwMinTemp, strComPort) ) {
			return;
		}
		else {
			g_dwLowTempAlarm[i] = dwLowAlarm;
			g_dwHighTempAlarm[i] = dwHighAlarm;
			g_dwCollectInterval[i] = dwInterval;
			g_dwMyImageMinTemp[i] = dwMinTemp;
			STRNCPY(g_szComPort[i], strComPort, MAX_COM_PORT_LENGTH);
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
	BOOL bAlarmVoiceOff = FALSE;

	bGetCfg = m_tree->GetConfigValue( 1, cfgValue );
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%d", &nCols)) {
		strText.Format("窗格列数请输入数字");
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	if (nCols <= 0) {
		strText.Format("窗格列数请输入正数");
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	bGetCfg = m_tree->GetConfigValue(2, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%d", &nRows)) {
		strText.Format("窗格行请输入数字");
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	if (nRows <= 0) {
		strText.Format("窗格行数请输入正数");
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	if ( nCols * nRows > MAX_GRID_COUNT ) {
		strText.Format("窗口列数(%d) * 窗口行数(%d) = (%d)。请保证小于(%d)", nCols, nRows, nCols * nRows, MAX_GRID_COUNT);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	
	bGetCfg = m_tree->GetConfigValue(3, cfgValue);
	if ( 0 == cfgValue.m_nComboSel ) {
		nSkin = SKIN_BLACK;
	}
	else if ( 1 == cfgValue.m_nComboSel ) {
		nSkin = SKIN_WHITE;
	}
	else {
		nSkin = SKIN_BLACK;
	}

	bGetCfg = m_tree->GetConfigValue(4, cfgValue);
	if (0 == cfgValue.m_nComboSel) {
		bAlarmVoiceOff = FALSE;
	}
	else  {
		bAlarmVoiceOff = TRUE;
	}

	g_dwLayoutColumns = nCols;
	g_dwLayoutRows = nRows;
	g_dwSkinIndex = nSkin;
	g_bAlarmVoiceOff = bAlarmVoiceOff;

	return TRUE;
}

BOOL  CSettingDlg::GetConfig(int nIndex, DWORD & dwInterval, DWORD & dwLowAlarm,
	DWORD & dwHighAlarm, DWORD & dwMinTemp, CDuiString & strComPort)
{
	CMyTreeCfgUI::ConfigValue  cfgValue;
	CDuiString  strText;
	double dbTemp;

	dwInterval = 10;
	bool bGetCfg = m_tree->GetConfigValue( 6 + nIndex * 6 + 1, cfgValue);
	switch (cfgValue.m_nComboSel)
	{
	case 0:
	{
		dwInterval = 10;
	}
	break;

	case 1:
	{
		dwInterval = 60;
	}
	break;

	case 2:
	{
		dwInterval = 300;
	}
	break;

	case 3:
	{
		dwInterval = 900;
	}
	break;

	case 4:
	{
		dwInterval = 1800;
	}
	break;

	case 5:
	{
		dwInterval = 3600;
	}
	break;

	default:
		break;
	}


	dwMinTemp = 20;
	bGetCfg = m_tree->GetConfigValue(6 + nIndex * 6 + 2, cfgValue);
	switch (cfgValue.m_nComboSel)
	{
	case 0:
	{
		dwMinTemp = 20;
	}
	break;

	case 1:
	{
		dwMinTemp = 24;
	}
	break;

	case 2:
	{
		dwMinTemp = 28;
	}
	break;

	case 3:
	{
		dwMinTemp = 32;
	}
	break;

	default:
		break;
	}


	bGetCfg = m_tree->GetConfigValue( 6 + nIndex * 6 + 3, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%lf", &dbTemp)) {
		strText.Format("窗口%d配置，低温报警请输入数字", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	if (dbTemp > 42 || dbTemp < 20) {
		strText.Format("窗口%d配置，低温报警请输入范围(20~42)", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	dwLowAlarm = (DWORD)(dbTemp * 100);

	bGetCfg = m_tree->GetConfigValue( 6 + nIndex * 6 + 4, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%lf", &dbTemp)) {
		strText.Format("窗口%d配置，高温报警请输入数字", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	if (dbTemp > 42 || dbTemp < 20) {
		strText.Format("窗口%d配置，高温报警请输入范围(20~42)", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	if (dbTemp < dwLowAlarm / 100.0) {
		strText.Format("窗口%d配置，高温报警请大于低温报警", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	dwHighAlarm = (DWORD)(dbTemp * 100);

	// 串口
	bGetCfg = m_tree->GetConfigValue( 6 + nIndex * 6 + 5, cfgValue);
	if (0 == cfgValue.m_nComboSel) {
		strComPort = "";
	}
	else {
		strComPort = cfgValue.m_strEdit;
	}
	return TRUE;
}