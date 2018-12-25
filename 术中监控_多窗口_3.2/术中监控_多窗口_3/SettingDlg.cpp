#include "SettingDlg.h"

#define EIDT_TEXT_COLOR    0xFF386382
#define EDIT_FONT_INDEX    2

#define COMMON_ITEMS_COUNT   4
#define GRID_SETTING_ITEMS_COUNT  3

CSettingDlg::CSettingDlg() {

}


void   CSettingDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	if ( msg.sType == "click" && name == "btnOK" ) {
		OnBtnOk(msg);
	}
	WindowImplBase::Notify(msg); 
}

void   CSettingDlg::InitWindow() {
	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl(MYTREE_CONFIG_NAME);
	assert(m_tree);

	InitCommonCfg();

	DuiLib::CDuiString  strText;
	CMyTreeCfgUI::Node* pTitleNode = NULL;
	strText.Format("床位");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666 );

	for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
		InitGridCfg(pTitleNode, i);
	}
	WindowImplBase::InitWindow();   
}

LRESULT  CSettingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CSettingDlg::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, MYTREE_CLASS_NAME)) {
		return new CMyTreeCfgUI(180);
	}
	return WindowImplBase::CreateControl(pstrClass);
}


void  CSettingDlg::AddComboItem(CComboUI * pCombo, const char * szItem, UINT_PTR tag) {
	CListLabelElementUI * pElement = new CListLabelElementUI;
	pElement->SetText(szItem);
	pElement->SetTag(tag);
	pCombo->Add(pElement);
}

void  CSettingDlg::InitCommonCfg() {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pTitleNode = NULL;
	CComboUI * pCombo = 0;  
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;
	CCheckBoxUI * pCheckBox = 0;	
	std::vector<TArea *>::iterator it;
	int i = 0;
	int nIndex = 0;

	strText.Format("通用设置");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// 病区号
	pCombo = new CComboUI;
	AddComboItem(pCombo, "未设置", 0);  	
	for (it = g_vArea.begin(); it != g_vArea.end(); ++it, ++i) {
		TArea * pArea = *it;

		strText.Format("%s(编号：%lu)", pArea->szAreaName, pArea->dwAreaNo);
		AddComboItem(pCombo, strText, pArea->dwAreaNo);
		if ( g_data.m_CfgData.m_dwAreaNo == pArea->dwAreaNo) {
			nIndex = i + 1;
		}
	}
	pCombo->SelectItem(nIndex);
	m_tree->AddNode("病区号", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// 行、列
	pEdit = new CEditUI;
	strText.Format("%lu", g_data.m_CfgData.m_dwLayoutColumns);
	pEdit->SetText(strText);
	m_tree->AddNode("窗格列数", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	pEdit = new CEditUI;
	strText.Format("%lu", g_data.m_CfgData.m_dwLayoutRows);
	pEdit->SetText(strText);
	m_tree->AddNode("窗格行数", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	// 皮肤
	pCombo = new CComboUI;
	AddComboItem(pCombo, "黑曜石", 0);
	AddComboItem(pCombo, "白宣纸", 1);
	pCombo->SelectItem(g_data.m_CfgData.m_dwSkinIndex);
	m_tree->AddNode("选择皮肤", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// 报警声音开关 
	pCheckBox = new CCheckBoxUI;
	pCheckBox->Selected(g_data.m_CfgData.m_bAlarmVoiceOff ? false : true);
	m_tree->AddNode("报警声音开关", pTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382 );

	// 字体
	pCombo = new CComboUI;
	for ( DWORD i = 0; i < 8; i++ ) {
		strText.Format("%lu", i + 1);
		AddComboItem(pCombo, strText, i+10);
	}
	pCombo->SelectItem(g_data.m_CfgData.m_dwTempFont - 10);
	m_tree->AddNode("温度字体大小", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382);

	// 自动保存excel 
	pCheckBox = new CCheckBoxUI;
	pCheckBox->Selected(g_data.m_CfgData.m_bAutoSaveExcel ? true : false);
	m_tree->AddNode("自动保存excel", pTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);

	// 十字定位锚 
	pCheckBox = new CCheckBoxUI;
	pCheckBox->Selected(g_data.m_CfgData.m_bCrossAnchor ? true : false);
	m_tree->AddNode("温度曲线十字定位", pTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);
}

void  CSettingDlg::InitGridCfg(CMyTreeCfgUI::Node* pTitleNode, DWORD dwIndex) {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pSubTitleNode = NULL;
	strText.Format("床位%lu参数设置", dwIndex + 1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666 );

	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;
	CCheckBoxUI * pCheckBox = 0;

	// 启用/禁用
	pCheckBox = new CCheckBoxUI;
	pCheckBox->Selected(g_data.m_CfgData.m_GridCfg[dwIndex].m_bSwitch ? true : false);
	m_tree->AddNode("是否启用全部读卡器", pSubTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);

	// 间隔时间
	pCombo = new CComboUI;
	AddComboItem(pCombo, "10秒", 0);
	AddComboItem(pCombo, "1分钟", 1);
	AddComboItem(pCombo, "5分钟", 2);
	AddComboItem(pCombo, "15分钟", 3);
	AddComboItem(pCombo, "30分钟", 4);
	AddComboItem(pCombo, "1小时", 5);

	pCombo->SelectItem(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwCollectInterval);
	m_tree->AddNode("采样间隔", pSubTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// 显示最低温度
	pCombo = new CComboUI;
	AddComboItem(pCombo, "20℃", 0);
	AddComboItem(pCombo, "24℃", 1);
	AddComboItem(pCombo, "28℃", 2);
	AddComboItem(pCombo, "32℃", 3);
	AddComboItem(pCombo, "34℃", 4);
	AddComboItem(pCombo, "36℃", 5);
	pCombo->SelectItem(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwMinTemp);
	m_tree->AddNode("显示的最低温度", pSubTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// 显示最高温度
	pCombo = new CComboUI;
	AddComboItem(pCombo, "42℃", 0);
	AddComboItem(pCombo, "40℃", 1);
	AddComboItem(pCombo, "38℃", 2);
	AddComboItem(pCombo, "34℃", 3);
	AddComboItem(pCombo, "30℃", 4);
	pCombo->SelectItem(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwMaxTemp);
	m_tree->AddNode("显示的最高温度", pSubTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382);

	CMyTreeCfgUI::Node* pSubTitleNode_1 = NULL;
	strText.Format("读卡器");
	pSubTitleNode_1 = m_tree->AddNode(strText, pSubTitleNode, 0, 0, 3, 0xFF666666);

	for ( DWORD i = 0; i < MAX_READERS_PER_GRID; i++ ) {

		CMyTreeCfgUI::Node* pSubTitleNode_2 = NULL;
		strText.Format("读卡器%c", i+'A');
		pSubTitleNode_2 = m_tree->AddNode(strText, pSubTitleNode_1, 0, 0, 3, 0xFF666666);

		pCheckBox = new CCheckBoxUI;
		pCheckBox->Selected(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_bSwitch ? true : false);
		m_tree->AddNode("是否启用", pSubTitleNode_2, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);

		// 低温报警
		pEdit = new CEditUI;
		strText.Format("%.2f", g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_dwLowTempAlarm / 100.0 );
		pEdit->SetText(strText);
		m_tree->AddNode("低温报警", pSubTitleNode_2, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

		// 高温报警
		pEdit = new CEditUI;
		strText.Format("%.2f", g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_dwHighTempAlarm / 100.0);
		pEdit->SetText(strText);
		m_tree->AddNode("高温报警", pSubTitleNode_2, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

		// 床号
		//pEdit = new CEditUI;
		//strText.Format("%lu", g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_dwBed);
		//pEdit->SetText(strText);
		//m_tree->AddNode("Reader编号", pSubTitleNode_2, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

		pCombo = new CComboUI;
		AddComboItem(pCombo, "空", 0);

		for ( DWORD j = 0; j < MAX_GRID_COUNT; j++ ) {
			for ( char k = 'A'; k <= 'F'; k++ ) {
				strText.Format("%lu%c", j + 1, k);
				AddComboItem(pCombo, strText, 6 * j + (k - 'A') + 1);
			}
		}
		pCombo->SelectItem(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_dwBed);
#if DB_FLAG
		pCombo->SetEnabled(false);
#endif
		m_tree->AddNode("Reader编号", pSubTitleNode_2, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382);


		if ( i > 0 ) {
			m_tree->ExpandNode(pSubTitleNode_2, false);
		}
	}

	m_tree->ExpandNode(pSubTitleNode, false);
}

void  CSettingDlg::OnBtnOk(DuiLib::TNotifyUI& msg) {
	m_data = g_data.m_CfgData;
	CDuiString  strText;

	if (!GetCommonConfig()) {
		return;
	}

	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		if ( !GetGridConfig(i) ) {
			return;
		}
	}

	// 检查Reader编号是否重复
	for ( int i = 0; i < MAX_GRID_COUNT; i++ ) {
		for ( int j = 0; j < MAX_READERS_PER_GRID; j++ ) {
			if ( m_data.m_GridCfg[i].m_ReaderCfg[j].m_dwBed > 0 ) {

				for (int m = i; m < MAX_GRID_COUNT; m++) {
					for (int n = 0; n < MAX_READERS_PER_GRID; n++) {
						if ( m == i && n <= j ) {
							continue;
						}

						if (m_data.m_GridCfg[i].m_ReaderCfg[j].m_dwBed == m_data.m_GridCfg[m].m_ReaderCfg[n].m_dwBed) {
							strText.Format("窗口%d读卡器%d的编号和窗口%d读卡器%d的编号重复", i + 1, j + 1, m + 1, n + 1);
							::MessageBox(this->GetHWND(), strText, "设置", 0);
							return;
						}
					}
				}
			}
		}
	}

	PostMessage(WM_CLOSE);
}

BOOL  CSettingDlg::GetCommonConfig() {
	CDuiString  strText;
	CMyTreeCfgUI::ConfigValue  cfgValue;
	bool bGetCfg = false;
	int  nCfgRowIndex = 1;
	int  nNumber = 0;

	// 区号
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_dwAreaNo = cfgValue.m_tag;
	nCfgRowIndex++;

	//列数
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%d", &nNumber)) {
		strText.Format("窗格列数请输入数字");
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	if (nNumber <= 0) {
		strText.Format("窗格列数请输入正数");
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	m_data.m_dwLayoutColumns = nNumber;
	nCfgRowIndex++;

	// 行数
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%d", &nNumber)) {
		strText.Format("窗格行请输入数字");
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	if (nNumber <= 0) {
		strText.Format("窗格行数请输入正数");
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	m_data.m_dwLayoutRows = nNumber;

	if (m_data.m_dwLayoutColumns * m_data.m_dwLayoutRows > MAX_GRID_COUNT) {
		strText.Format("窗口列数(%d) * 窗口行数(%d) = (%d)。请保证小于(%d)", m_data.m_dwLayoutColumns, m_data.m_dwLayoutRows,
			   m_data.m_dwLayoutColumns * m_data.m_dwLayoutRows, MAX_GRID_COUNT);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	nCfgRowIndex++;

	// 皮肤
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_dwSkinIndex = cfgValue.m_tag;
	nCfgRowIndex++;

	// 报警声
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_bAlarmVoiceOff = !cfgValue.m_bCheckbox;
	nCfgRowIndex++;

	// 字体
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_dwTempFont = cfgValue.m_tag;
	nCfgRowIndex++;

	// 自动保存excel
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_bAutoSaveExcel = cfgValue.m_bCheckbox;
	nCfgRowIndex++;

	// 十字定位
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_bCrossAnchor = cfgValue.m_bCheckbox;
	nCfgRowIndex++;

	return TRUE;
}

BOOL  CSettingDlg::GetGridConfig(int nIndex) {
	CDuiString  strText;
	CMyTreeCfgUI::ConfigValue  cfgValue;
	bool bGetCfg = false;
	int  nCfgRowIndex = 10 + ((5 * MAX_READERS_PER_GRID)+6) * nIndex + 2 - 1;
	int  nNumber = 0;
	double dNumber = 0.0;

	// 格子开关
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_GridCfg[nIndex].m_bSwitch = cfgValue.m_bCheckbox;
	nCfgRowIndex++;

	// 采集间隔
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_GridCfg[nIndex].m_dwCollectInterval = cfgValue.m_tag;
	nCfgRowIndex++;

	// 最低显示可读
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_GridCfg[nIndex].m_dwMinTemp = cfgValue.m_tag;
	nCfgRowIndex++;

	// 最高显示可读
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_GridCfg[nIndex].m_dwMaxTemp = cfgValue.m_tag;
	nCfgRowIndex++;

	if (GetMaxTemp(m_data.m_GridCfg[nIndex].m_dwMaxTemp) <=
		GetMinTemp(m_data.m_GridCfg[nIndex].m_dwMinTemp)) {

		strText.Format("窗口%d，最高显示温度必须大于最低显示温度", nIndex + 1 );
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	// 获取Reader配置
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + 5 * i + 2, cfgValue);
		m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_bSwitch = cfgValue.m_bCheckbox;

		bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + 5 * i + 3, cfgValue);
		strText = cfgValue.m_strEdit;
		if (1 != sscanf_s(strText, "%lf", &dNumber)) {
			strText.Format("窗口%d读卡器%d配置，低温报警请输入数字", nIndex + 1, i+1);
			::MessageBox(this->GetHWND(), strText, "设置", 0);
			return FALSE;
		}

		if (dNumber > 42 || dNumber < 20) {
			strText.Format("窗口%d读卡器%d配置，低温报警请输入范围(20~42)", nIndex + 1, i+1);
			::MessageBox(this->GetHWND(), strText, "设置", 0);
			return FALSE;
		}
		m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_dwLowTempAlarm = (DWORD)(dNumber * 100);
		
		bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + 5 * i + 4, cfgValue);
		strText = cfgValue.m_strEdit;
		if (1 != sscanf_s(strText, "%lf", &dNumber)) {
			strText.Format("窗口%d读卡器%d配置，高温报警请输入数字", nIndex + 1, i + 1);
			::MessageBox(this->GetHWND(), strText, "设置", 0);
			return FALSE;
		}

		if (dNumber > 42 || dNumber < 20) {
			strText.Format("窗口%d读卡器%d配置，高温报警请输入范围(20~42)", nIndex + 1, i + 1);
			::MessageBox(this->GetHWND(), strText, "设置", 0);
			return FALSE;
		}
		m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_dwHighTempAlarm = (DWORD)(dNumber * 100);

		if (m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_dwHighTempAlarm < m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_dwLowTempAlarm) {
			strText.Format("窗口%d读卡器%d配置，高温报警请大于低温报警",  nIndex + 1, i+1);
			::MessageBox(this->GetHWND(), strText, "设置", 0);
			return FALSE;
		}

		bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + 5 * i + 5, cfgValue);
		//strText = cfgValue.m_strEdit;
		//if (1 != sscanf_s(strText, "%d", &nNumber)) {
		//	strText.Format("窗口%d读卡器%d配置，床号请输入数字", nIndex + 1, i + 1);
		//	::MessageBox(this->GetHWND(), strText, "设置", 0);
		//	return FALSE;
		//}
		//m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_dwBed = nNumber;
		m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_dwBed = cfgValue.m_tag;
	}

	return TRUE;
}

