#include "SettingDlg.h"

/*
  通用设置         ............................      8项
      7
  //手持测温设置 ..............................        3项
  // 2
  床位
      床位1         ..............................    23 项
	      1 + 3 + 3 * 6
*/

#define   SUBITEMS_COUNT_PER_BED                   23
#define   COMMON_AND_HAND_ITEMS_COUNT              8


CSettingDlg::CSettingDlg() {
	memset(&m_data, 0, sizeof(m_data));
	m_pBedTitleNode = 0;
	m_edGridCnt = 0;
}


void   CSettingDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	if ( msg.sType == "click" && name == "btnOK" ) {
		OnBtnOk(msg);
	}
	else if (msg.sType == "killfocus") {
		if (name == "edGridsCnt") {
			OnGridsChanged();
		}
	}
	WindowImplBase::Notify(msg); 
}

void   CSettingDlg::InitWindow() {
	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl(MYTREE_CONFIG_NAME);
	assert(m_tree);

	InitCommonCfg();

	// InitHandReaderCfg();

	DuiLib::CDuiString  strText;
	strText.Format("床位");
	m_pBedTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666 );

	for (DWORD i = 0; i < m_data.m_dwLayoutGridsCnt; i++) {
		InitGridCfg(m_pBedTitleNode, i);
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
		if ( m_data.m_dwAreaNo == pArea->dwAreaNo) {
			nIndex = i + 1;
		}
	}
	pCombo->SelectItem(nIndex);
	m_tree->AddNode("病区号", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// 窗格总数
	pEdit = new CEditUI;
	strText.Format("%lu", m_data.m_dwLayoutGridsCnt);
	pEdit->SetText(strText);
	pEdit->SetName("edGridsCnt");
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("窗格总数", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);
	m_edGridCnt = pEdit;

	// 行、列
	pEdit = new CEditUI;
	strText.Format("%lu", m_data.m_dwLayoutColumns);
	pEdit->SetText(strText);
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("每页窗格列数", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	pEdit = new CEditUI;
	strText.Format("%lu", m_data.m_dwLayoutRows);
	pEdit->SetText(strText);
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("每页窗格行数", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	// 报警声音开关 
	pCheckBox = new CCheckBoxUI;
	pCheckBox->Selected(m_data.m_bAlarmVoiceOff ? false : true);
	m_tree->AddNode("报警声音开关", pTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382 );

	// 自动保存excel 
	pCheckBox = new CCheckBoxUI;
	pCheckBox->Selected(m_data.m_bAutoSaveExcel ? true : false);
	m_tree->AddNode("自动保存excel", pTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);

	// 十字定位锚 
	pCheckBox = new CCheckBoxUI;
	pCheckBox->Selected(m_data.m_bCrossAnchor ? true : false);
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

	// 间隔时间
	pCombo = new CComboUI;
	AddComboItem(pCombo, "10秒", 0);
	AddComboItem(pCombo, "1分钟", 1);
	AddComboItem(pCombo, "5分钟", 2);
	AddComboItem(pCombo, "15分钟", 3);
	AddComboItem(pCombo, "30分钟", 4);
	AddComboItem(pCombo, "1小时", 5);

	pCombo->SelectItem(m_data.m_GridCfg[dwIndex].m_dwCollectInterval);
	m_tree->AddNode("采样间隔", pSubTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// 显示最低温度
	//pEdit = new CEditUI;
	//strText.Format("%lu", m_data.m_GridCfg[dwIndex].m_dwMinTemp);
	//pEdit->SetText(strText);
	//pEdit->SetNumberOnly(true);
	//m_tree->AddNode("显示的最低温度(℃)", pSubTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	// 显示最高温度
	//pEdit = new CEditUI;
	//strText.Format("%lu", m_data.m_GridCfg[dwIndex].m_dwMaxTemp);
	//pEdit->SetText(strText);
	//pEdit->SetNumberOnly(true);
	//m_tree->AddNode("显示的最高温度(℃)", pSubTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 手持读卡器的报警部分
	CMyTreeCfgUI::Node* pHandeReaderTitleNode = NULL;
	strText.Format("手持读卡器");
	pHandeReaderTitleNode = m_tree->AddNode(strText, pSubTitleNode, 0, 0, 3, 0xFF666666);

	pEdit = new CEditUI;
	strText.Format("%.2f", m_data.m_GridCfg[dwIndex].m_HandReaderCfg.m_dwHighTempAlarm / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode("高温报警(℃)", pHandeReaderTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	pEdit = new CEditUI;
	strText.Format("%.2f", m_data.m_GridCfg[dwIndex].m_HandReaderCfg.m_dwLowTempAlarm / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode("低温报警(℃)", pHandeReaderTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	m_tree->ExpandNode(pHandeReaderTitleNode, false);


	for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
		CMyTreeCfgUI::Node* pReaderTitleNode = NULL;
		strText.Format( "读卡器%c", (char)('A'+j) );
		pReaderTitleNode = m_tree->AddNode(strText, pSubTitleNode, 0, 0, 3, 0xFF666666);

		pEdit = new CEditUI;
		strText.Format("%.2f", m_data.m_GridCfg[dwIndex].m_ReaderCfg[j].m_dwHighTempAlarm / 100.0 );
		pEdit->SetText(strText);
		m_tree->AddNode("高温报警(℃)", pReaderTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

		pEdit = new CEditUI;
		strText.Format("%.2f", m_data.m_GridCfg[dwIndex].m_ReaderCfg[j].m_dwLowTempAlarm / 100.0);
		pEdit->SetText(strText);
		m_tree->AddNode("低温报警(℃)", pReaderTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

		m_tree->ExpandNode(pReaderTitleNode, false);
	}

	m_tree->ExpandNode(pSubTitleNode, false);	
}

void  CSettingDlg::OnBtnOk(DuiLib::TNotifyUI& msg) {
	m_data = m_data;
	CDuiString  strText;

	if (!GetCommonConfig()) {
		return;
	}

	//if (!GetHandReaderConfig()) {
	//	return;
	//}

	for (int i = 0; i < (int)m_data.m_dwLayoutGridsCnt; i++) {
		if ( !GetGridConfig(i) ) {
			return;
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

	// 格子数目
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	strText = cfgValue.m_strEdit;
	sscanf_s(strText, "%lu", &m_data.m_dwLayoutGridsCnt);
	nCfgRowIndex++;

	//列数
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	strText = cfgValue.m_strEdit;
	sscanf_s(strText, "%d", &nNumber);
	if (nNumber <= 0) {
		strText.Format("每页窗格列数请输入正数");
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	m_data.m_dwLayoutColumns = nNumber;
	nCfgRowIndex++;

	// 行数
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	strText = cfgValue.m_strEdit;
	sscanf_s(strText, "%d", &nNumber);
	if (nNumber <= 0) {
		strText.Format("每页窗格行数请输入正数");
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	m_data.m_dwLayoutRows = nNumber;

	if (m_data.m_dwLayoutColumns * m_data.m_dwLayoutRows <= 1) {
		strText.Format("窗口列数(%d) * 窗口行数(%d) = (%d)。请保证大于(%d)", m_data.m_dwLayoutColumns, m_data.m_dwLayoutRows,
			m_data.m_dwLayoutColumns * m_data.m_dwLayoutRows, 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
	}
	nCfgRowIndex++;

	// 报警声
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_bAlarmVoiceOff = !cfgValue.m_bCheckbox;
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
	// const  DWORD  ITEMS_COUNT_PER_READER = 3;
	bool bGetCfg = false;
	int  nCfgRowIndex = COMMON_AND_HAND_ITEMS_COUNT + SUBITEMS_COUNT_PER_BED * nIndex + 2;
	int  nNumber = 0;
	double dNumber = 0.0;

	// 采集间隔
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_GridCfg[nIndex].m_dwCollectInterval = cfgValue.m_tag;
	nCfgRowIndex++;

	// 最低显示可读
	//bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	//sscanf_s(cfgValue.m_strEdit, "%lu", &m_data.m_GridCfg[nIndex].m_dwMinTemp);
	//nCfgRowIndex++;
	//if (m_data.m_GridCfg[nIndex].m_dwMinTemp < MIN_TEMP_IN_SHOW) {
	//	strText.Format("床位%d，最低显示温度必须大于等于%lu℃", nIndex + 1, MIN_TEMP_IN_SHOW);
	//	::MessageBox(this->GetHWND(), strText, "设置", 0);
	//	return FALSE;
	//}


	// 最高显示可读
	//bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	//sscanf_s(cfgValue.m_strEdit, "%lu", &m_data.m_GridCfg[nIndex].m_dwMaxTemp);
	//nCfgRowIndex++;
	//if (m_data.m_GridCfg[nIndex].m_dwMinTemp > MAX_TEMP_IN_SHOW) {
	//	strText.Format("床位%d，最高显示温度必须小于等于%lu℃", nIndex + 1, MAX_TEMP_IN_SHOW);
	//	::MessageBox(this->GetHWND(), strText, "设置", 0);
	//	return FALSE;
	//}

	//if ( m_data.m_GridCfg[nIndex].m_dwMaxTemp <= m_data.m_GridCfg[nIndex].m_dwMinTemp ) {
	//	strText.Format("床位%d，最高显示温度必须大于最低显示温度", nIndex + 1);
	//	::MessageBox(this->GetHWND(), strText, "设置", 0);
	//	return FALSE;
	//}

	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + 1, cfgValue);
	sscanf_s(cfgValue.m_strEdit, "%lf", &dNumber);
	int nHigh = (int)(dNumber * 100);

	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + 2, cfgValue);
	sscanf_s(cfgValue.m_strEdit, "%lf", &dNumber);
	int nLow = (int)(dNumber * 100);

	if (nLow >= nHigh) {
		strText.Format("床位%d手持读卡器，高温报警必须大于低温报警", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	else if (nLow < 0 || nHigh < 0) {
		strText.Format("床位%d手持读卡器，高温报警和低温报警必须为正数", nIndex + 1 );
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}
	m_data.m_GridCfg[nIndex].m_HandReaderCfg.m_dwHighTempAlarm = nHigh;
	m_data.m_GridCfg[nIndex].m_HandReaderCfg.m_dwLowTempAlarm = nLow;
	nCfgRowIndex += 3;

	for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
		bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + j * 3 + 1, cfgValue);
		sscanf_s(cfgValue.m_strEdit, "%lf", &dNumber);
		int nHigh = (int)(dNumber * 100);

		bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + j * 3 + 2, cfgValue);
		sscanf_s(cfgValue.m_strEdit, "%lf", &dNumber);
		int nLow = (int)(dNumber * 100);

		if ( nLow >= nHigh ) {
			strText.Format("床位%d读卡器%c，高温报警必须大于低温报警", nIndex + 1, (char)('A'+j));
			::MessageBox(this->GetHWND(), strText, "设置", 0);
			return FALSE;
		}
		else if (nLow < 0 || nHigh < 0) {
			strText.Format("床位%d读卡器%c，高温报警和低温报警必须为正数", nIndex + 1, (char)('A' + j));
			::MessageBox(this->GetHWND(), strText, "设置", 0);
			return FALSE;
		}

		m_data.m_GridCfg[nIndex].m_ReaderCfg[j].m_dwHighTempAlarm = nHigh;
		m_data.m_GridCfg[nIndex].m_ReaderCfg[j].m_dwLowTempAlarm = nLow;
	}

	return TRUE;
}


void    CSettingDlg::InitHandReaderCfg() {
	DuiLib::CDuiString  strText;
	CMyTreeCfgUI::Node* pTitleNode = NULL;
	CComboUI * pCombo = 0;
	CEditUI * pEdit = 0;

	strText.Format("手持测温设置");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// 显示最低温度
	pEdit = new CEditUI;
	strText.Format("%.2f", m_data.m_dwHandReaderHighTempAlarm / 100.0 );
	pEdit->SetText(strText);
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("高温报警(℃)", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 显示最高温度
	pEdit = new CEditUI;
	strText.Format("%.2f", m_data.m_dwHandReaderLowTempAlarm / 100.0 );
	pEdit->SetText(strText);
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("低温报警(℃)", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);
}

BOOL    CSettingDlg::GetHandReaderConfig() {

	CDuiString  strText;
	CMyTreeCfgUI::ConfigValue  cfgValue;
	bool bGetCfg = false;
	double dMax = 0.0;
	double dMin = 0.0;
	int  nCfgRowIndex = 9;  

	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	sscanf_s(cfgValue.m_strEdit, "%lf", &dMax );
	nCfgRowIndex++;

	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	sscanf_s(cfgValue.m_strEdit, "%lf", &dMin );
	nCfgRowIndex++;

	if ( dMin >= dMax ) {
		strText.Format("手持测温，低温报警必须大于高温报警");
		::MessageBox(this->GetHWND(), strText, "设置", 0);
		return FALSE;
	}

	m_data.m_dwHandReaderHighTempAlarm = (DWORD)(dMax * 100.0);
	m_data.m_dwHandReaderLowTempAlarm  = (DWORD)(dMin * 100.0);

	return TRUE;
}


// 窗格总数改变
void   CSettingDlg::OnGridsChanged() {
	bool bGetCfg = false;
	CMyTreeCfgUI::ConfigValue  cfgValue;
	CDuiString strText;
	DWORD  dwNumber = 0;
	int ret = 0;

	bGetCfg = m_tree->GetConfigValue(2, cfgValue);
	strText = cfgValue.m_strEdit;
	ret = sscanf_s(strText, "%lu", &dwNumber);
	assert(1 == ret);

	if ( dwNumber > MAX_GRID_COUNT ) {
		dwNumber = MAX_GRID_COUNT;
		strText.Format("%lu", dwNumber);
		m_edGridCnt->SetText(strText);
	}
	else if (dwNumber == 0) {
		dwNumber = 1;
		strText.Format("%lu", dwNumber);
		m_edGridCnt->SetText(strText);
	}

	if ( dwNumber == m_data.m_dwLayoutGridsCnt ) {
		return;
	}

	// 如果格子数变多了
	if ( dwNumber > m_data.m_dwLayoutGridsCnt ) {
		int nCnt = m_tree->GetCount();
		DWORD  dwDiff =  dwNumber - m_data.m_dwLayoutGridsCnt;

		// 初始化增加的格子，给默认值
		for (DWORD i = 0; i < dwDiff; i++) {
			DWORD  dwIndex = m_data.m_dwLayoutGridsCnt + i;
			memset( &m_data.m_GridCfg[dwIndex], 0, sizeof(GridCfg));
			m_data.m_GridCfg[dwIndex].m_dwMinTemp = DEFAULT_MIN_TEMP_IN_SHOW;
			m_data.m_GridCfg[dwIndex].m_dwMaxTemp = DEFAULT_MAX_TEMP_IN_SHOW;
			m_data.m_GridCfg[dwIndex].m_HandReaderCfg.m_dwLowTempAlarm = DEFAULT_LOW_TEMP_ALARM;
			m_data.m_GridCfg[dwIndex].m_HandReaderCfg.m_dwHighTempAlarm = DEFAULT_HIGH_TEMP_ALARM;
			for ( DWORD j = 0; j < MAX_READERS_PER_GRID; j++ ) {
				m_data.m_GridCfg[dwIndex].m_ReaderCfg[j].m_dwHighTempAlarm = DEFAULT_HIGH_TEMP_ALARM;
				m_data.m_GridCfg[dwIndex].m_ReaderCfg[j].m_dwLowTempAlarm  = DEFAULT_LOW_TEMP_ALARM;
			}
		}

		// 界面显示
		for (DWORD i = 0; i < dwDiff; i++) {
			InitGridCfg(m_pBedTitleNode, m_data.m_dwLayoutGridsCnt+i);
		}
	}
	// 格子数变少了
	else {
		int nCnt = m_tree->GetCount();
		DWORD  dwDiff = m_data.m_dwLayoutGridsCnt - dwNumber;
		for ( DWORD i = 0; i < dwDiff; i++ ) {
			m_tree->RemoveAt( nCnt - SUBITEMS_COUNT_PER_BED * dwDiff );
		}
	}
	m_data.m_dwLayoutGridsCnt = dwNumber;
}