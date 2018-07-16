#include "SettingDlg.h"


CSettingDlg::CSettingDlg() {

}

BOOL  CSettingDlg::GetConfig( int nIndex, DWORD & dwInterval, DWORD & dwLowAlarm, 
	                         DWORD & dwHighAlarm, DWORD & dwMinTemp, CDuiString & strComPort) 
{
	CMyTreeCfgUI::ConfigValue  cfgValue;
	CDuiString  strText;
	double dbTemp;

	dwInterval = 10;
	bool bGetCfg = m_tree->GetConfigValue(nIndex*6+1, cfgValue);
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
	bGetCfg = m_tree->GetConfigValue(nIndex * 6 + 2, cfgValue);
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


	bGetCfg = m_tree->GetConfigValue(nIndex * 6 + 3, cfgValue);
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

	bGetCfg = m_tree->GetConfigValue(nIndex * 6 + 4, cfgValue);
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
	bGetCfg = m_tree->GetConfigValue(nIndex * 6 + 5, cfgValue);
	if (0 == cfgValue.m_nComboSel) {
		strComPort = "";
	}
	else {
		strComPort = cfgValue.m_strEdit;
	}
	return TRUE;
}

void   CSettingDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();

	if (msg.sType == "click") {
		if (name == "btnOK") {
			DWORD   dwInterval = 0;
			DWORD   dwLowAlarm = 0;
			DWORD   dwHighAlarm = 0;
			DWORD   dwMinTemp = 0;
			CDuiString  strComPort;

			for (int i = 0; i < MYCHART_COUNT; i++) {
				if ( !GetConfig(i, dwInterval, dwLowAlarm, dwHighAlarm, dwMinTemp, strComPort) ) {
					return;
				}
				else {
					g_dwLowTempAlarm[i] = dwLowAlarm;
					g_dwHighTempAlarm[i] = dwHighAlarm;
					g_dwCollectInterval[i] = dwInterval;
					g_dwMinTemp[i] = dwMinTemp;
					STRNCPY(g_szComPort[i], strComPort, 32);
				}
			}
			PostMessage(WM_CLOSE);
		}
		else {
			CControlUI* pFindControl = msg.pSender;
			DuiLib::CDuiString strName;
			int nDepth = 0;

			while (pFindControl) {
				strName = pFindControl->GetName();
				if (0 == StrICmp((const char *)strName, "tree1")) {
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
	}
	DuiLib::WindowImplBase::Notify(msg);
}
void  CSettingDlg::SetComboStyle(CComboUI * pCombo) {
	pCombo->SetItemTextColor(0xFF386382);
	pCombo->SetHotItemTextColor(0xFF386382);
	pCombo->SetSelectedItemTextColor(0xFF386382);
	pCombo->SetItemFont(2);
	pCombo->SetAttributeList(" normalimage=\"file = 'Combo_nor.bmp' corner = '2,2,24,2'\" hotimage=\"file = 'Combo_over.bmp' "
		" corner = '2,2,24,2'\" pushedimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" textpadding=\"5, 1, 5, 1\" ");
}

void  CSettingDlg::SetEditStyle(CEditUI * pEdit) {
	pEdit->SetTextColor(0xFF386382);
	pEdit->SetFont(2);
}

void  CSettingDlg::AddComboItem(CComboUI * pCombo, const char * szItem) {
	CListLabelElementUI * pElement = new CListLabelElementUI;
	pElement->SetText(szItem);
	pCombo->Add(pElement);
}

void   CSettingDlg::InitConfig(int nIndex) {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pTitleNode = NULL;
	strText.Format("窗口%d参数设置", nIndex + 1);
	pTitleNode = m_tree->AddNode(strText);

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
	if (g_dwCollectInterval[nIndex] <= 10) {
		nSelIndex = 0;
	}
	else if (g_dwCollectInterval[nIndex] <= 60) {
		nSelIndex = 1;
	}
	else if (g_dwCollectInterval[nIndex] <= 300) {
		nSelIndex = 2;
	}
	else if (g_dwCollectInterval[nIndex] <= 900) {
		nSelIndex = 3;
	}
	else if (g_dwCollectInterval[nIndex] <= 1800) {
		nSelIndex = 4;
	}
	else {
		nSelIndex = 5;
	}
	pCombo->SelectItem(nSelIndex);

	SetComboStyle(pCombo);	
	m_tree->AddNode("采样间隔", pTitleNode, (void *)(1+MYCHART_COUNT*nIndex), pCombo);


	// 显示最低温度
	pCombo = new CComboUI;
	AddComboItem(pCombo, "20℃");
	AddComboItem(pCombo, "24℃");
	AddComboItem(pCombo, "28℃");
	AddComboItem(pCombo, "32℃");

	// 选定index
	nSelIndex = 0;
	if (g_dwMinTemp[nIndex] <= 20) {
		nSelIndex = 0;
	}
	else if (g_dwMinTemp[nIndex] <= 24) {
		nSelIndex = 1;
	}
	else if (g_dwMinTemp[nIndex] <= 28) {
		nSelIndex = 2;
	}
	else {
		nSelIndex = 3;
	}
	pCombo->SelectItem(nSelIndex);

	SetComboStyle(pCombo);
	m_tree->AddNode("显示的最低温度", pTitleNode, (void *)(2+MYCHART_COUNT*nIndex), pCombo);

	// 低温报警
	CEditUI * pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%.2f", g_dwLowTempAlarm[nIndex] / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode("低温报警", pTitleNode, (void *)(3 + MYCHART_COUNT*nIndex), pEdit);

	// 高温报警
	pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%.2f", g_dwHighTempAlarm[nIndex] / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode("高温报警", pTitleNode, (void *)(4 + MYCHART_COUNT*nIndex), pEdit);

	// 串口
	std::vector<std::string>  vCom;
	GetAllSerialPortName(vCom);

	std::vector<std::string>::iterator it;
	if (g_szComPort[nIndex][0] != 0) {
		for (it = vCom.begin(); it != vCom.end(); it++) {
			std::string & s = *it;
			if (0 == StrICmp(s.c_str(), g_szComPort[nIndex])) {
				break;
			}
		}
		if (it == vCom.end()) {
			vCom.push_back(g_szComPort[nIndex]);
		}
	}

	m_cmbComPort[nIndex] = new CComboUI;
	pCombo = m_cmbComPort[nIndex];
	AddComboItem(pCombo, "任意端口");

	for (it = vCom.begin(); it != vCom.end(); it++) {
		std::string & s = *it;
		AddComboItem(pCombo, s.c_str());
	}

	if (g_szComPort[nIndex][0] == 0) {
		pCombo->SelectItem(0);
	}
	else {
		for (it = vCom.begin(); it != vCom.end(); it++) {
			std::string & s = *it;
			if (0 == StrICmp(s.c_str(), g_szComPort[nIndex])) {
				pCombo->SelectItem((it - vCom.begin()) + 1);
			}
		}
	}

	SetComboStyle(pCombo);
	m_tree->AddNode("读写串口", pTitleNode, (void *)(5 + MYCHART_COUNT*nIndex), pCombo);
}

void   CSettingDlg::InitWindow() {	
	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl("tree1"); 
	assert(m_tree);

	for (int i = 0; i < MYCHART_COUNT; i++) {
		InitConfig(i);
	}

	m_tree->SetMinHeight(950);
	DuiLib::WindowImplBase::InitWindow();    
}

LRESULT  CSettingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CSettingDlg::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyTree")) {
		return new CMyTreeCfgUI();
	}
	return WindowImplBase::CreateControl(pstrClass);
}