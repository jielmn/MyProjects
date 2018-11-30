#include "SettingDlg.h"

#define EIDT_TEXT_COLOR    0xFF386382
#define EDIT_FONT_INDEX    2
#define GRID_SETTING_ITEMS_COUNT  7

CSettingDlg::CSettingDlg() {
	m_dwTempFont = 0;
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

	// ������
	pCombo = new CComboUI;
	AddComboItem(pCombo, "δ����", 0);

	std::vector<TArea *>::iterator it;
	int i = 0;
	int nIndex = 0;
	for (it = g_vArea.begin(); it != g_vArea.end(); ++it, ++i) {
		TArea * pArea = *it;

		strText.Format("%s(��ţ�%lu)", pArea->szAreaName, pArea->dwAreaNo);
		AddComboItem( pCombo, strText, pArea->dwAreaNo );

		if (g_dwAreaNo == pArea->dwAreaNo) {
			nIndex = i + 1;
		}
	}
	pCombo->SelectItem(nIndex);
	SetComboStyle(pCombo);
	m_tree->AddNode(AREA_NO_TEXT, pTitleNode, 0, pCombo);
	

	pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%lu", g_dwLayoutColumns);
	pEdit->SetText(strText);
	m_tree->AddNode(GRIDS_COLUMNS_TEXT, pTitleNode, 0, pEdit);

	pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%lu", g_dwLayoutRows);
	pEdit->SetText(strText);
	m_tree->AddNode(GRIDS_ROWS_TEXT, pTitleNode, 0, pEdit);

	// Ƥ��
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
	m_tree->AddNode(SKIN_CHOICE_TEXT, pTitleNode, 0, pCombo);

	// ������������
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

	// �¶������С
	pCombo = new CComboUI;
	for (DWORD i = 0; i < 8; i++) {
		strText.Format("%lu", i+1);
		AddComboItem(pCombo, strText);
	}
	pCombo->SelectItem(g_dwTemperatureFont - 10);
	SetComboStyle(pCombo);
	m_tree->AddNode("�¶������С", pTitleNode, 0, pCombo);
}

void   CSettingDlg::InitGridCfg(CMyTreeCfgUI::Node* pTitleNode, DWORD dwIndex) {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pSubTitleNode = NULL;
	strText.Format("����%lu��������", dwIndex + 1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode);

	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;

	// ����/����
	pCombo = new CComboUI;
	AddComboItem(pCombo, "����");
	AddComboItem(pCombo, "����");
	if ( g_bGridReaderSwitch[dwIndex] ) {
		pCombo->SelectItem(0);
	}
	else {
		pCombo->SelectItem(1);
	}
	SetComboStyle(pCombo);
	m_tree->AddNode(GRID_READER_SWITCH_TEXT, pSubTitleNode, 0, pCombo);

	// ���ʱ��
	pCombo = new CComboUI;
	AddComboItem(pCombo, "10��");
	AddComboItem(pCombo, "1����");
	AddComboItem(pCombo, "5����");
	AddComboItem(pCombo, "15����");
	AddComboItem(pCombo, "30����");
	AddComboItem(pCombo, "1Сʱ");

	// comob ѡ��index
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


	// ��ʾ����¶�
	pCombo = new CComboUI;
	AddComboItem(pCombo, "20��");
	AddComboItem(pCombo, "24��");
	AddComboItem(pCombo, "28��");
	AddComboItem(pCombo, "32��");

	// ѡ��index
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

	// ���±���
	CEditUI * pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%.2f", g_dwLowTempAlarm[dwIndex] / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode(LOW_ALARM_TEXT, pSubTitleNode, 0, pEdit);

	// ���±���
	pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%.2f", g_dwHighTempAlarm[dwIndex] / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode(HIGH_ALARM_TEXT, pSubTitleNode, 0, pEdit);

	// ����
	pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%lu", g_dwBedNo[dwIndex]);
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

	if ( !GetCommonConfig() ) {
		return;
	}

	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		if ( !GetConfig(i, dwInterval, dwLowAlarm, dwHighAlarm, dwMinTemp, dwBedNo, bSwitch) ) {
			return;
		}
		else {
			m_dwLowTempAlarm[i] = dwLowAlarm;
			m_dwHighTempAlarm[i] = dwHighAlarm;
			m_dwCollectInterval[i] = dwInterval;
			m_dwMyImageMinTemp[i] = dwMinTemp;
			m_dwBedNo[i] = dwBedNo;
			m_bGridReaderSwitch[i] = bSwitch;
		}
	}

	DuiLib::CDuiString  strText;
	DWORD dwCount = MAX_GRID_COUNT;
	for ( DWORD i = 0; i < dwCount; ++i ) {
		if (m_dwBedNo[i] > 0) {
			for (DWORD j = i + 1; j < dwCount; ++j) {
				// ������ظ���bed no
				if (m_dwBedNo[i] == m_dwBedNo[j]) {
					strText.Format("����%lu��%lu��Reader��ش���������ͬ", i + 1, j + 1);
					::MessageBox(GetHWND(), strText, "����", 0);
					return;
				}
			}
		}		
	}

	g_dwLayoutColumns = m_dwLayoutColumns;
	g_dwLayoutRows = m_dwLayoutRows;
	g_dwSkinIndex = m_dwSkinIndex;
	g_bAlarmVoiceOff = m_bAlarmVoiceOff;
	g_dwAreaNo = m_dwAreaNo;
	g_dwTemperatureFont = m_dwTempFont + 10;

	memcpy(g_dwLowTempAlarm, m_dwLowTempAlarm, sizeof(DWORD) * MAX_GRID_COUNT);
	memcpy(g_dwHighTempAlarm, m_dwHighTempAlarm, sizeof(DWORD) * MAX_GRID_COUNT);
	memcpy(g_dwCollectInterval, m_dwCollectInterval, sizeof(DWORD) * MAX_GRID_COUNT);
	memcpy(g_dwMyImageMinTemp, m_dwMyImageMinTemp, sizeof(DWORD) * MAX_GRID_COUNT);
	memcpy(g_dwBedNo, m_dwBedNo, sizeof(DWORD) * MAX_GRID_COUNT);
	memcpy(g_bGridReaderSwitch, m_bGridReaderSwitch, sizeof(BOOL) * MAX_GRID_COUNT);

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

	bGetCfg = m_tree->GetConfigValue( 2, cfgValue );
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%d", &nCols)) {
		strText.Format("������������������");
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}

	if (nCols <= 0) {
		strText.Format("������������������");
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}

	bGetCfg = m_tree->GetConfigValue(3, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%d", &nRows)) {
		strText.Format("����������������");
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}

	if (nRows <= 0) {
		strText.Format("������������������");
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}

	if ( nCols * nRows > MAX_GRID_COUNT ) {
		strText.Format("��������(%d) * ��������(%d) = (%d)���뱣֤С��(%d)", nCols, nRows, nCols * nRows, MAX_GRID_COUNT);
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}
	
	bGetCfg = m_tree->GetConfigValue(4, cfgValue);
	if ( 0 == cfgValue.m_nComboSel ) {
		nSkin = SKIN_BLACK;
	}
	else if ( 1 == cfgValue.m_nComboSel ) {
		nSkin = SKIN_WHITE;
	}
	else {
		nSkin = SKIN_BLACK;
	}

	bGetCfg = m_tree->GetConfigValue(5, cfgValue);
	if (0 == cfgValue.m_nComboSel) {
		bAlarmVoiceOff = FALSE;
	}
	else  {
		bAlarmVoiceOff = TRUE;
	}

	bGetCfg = m_tree->GetConfigValue(6, cfgValue);
	m_dwTempFont = cfgValue.m_nComboSel;

	m_dwLayoutColumns = nCols;
	m_dwLayoutRows = nRows;
	m_dwSkinIndex = nSkin;
	m_bAlarmVoiceOff = bAlarmVoiceOff; 
	m_dwAreaNo = nAreaNo;

	return TRUE;
}

BOOL  CSettingDlg::GetConfig(int nIndex, DWORD & dwInterval, DWORD & dwLowAlarm,
	DWORD & dwHighAlarm, DWORD & dwMinTemp, DWORD & dwBedNo, BOOL & bSwitch )
{
	CMyTreeCfgUI::ConfigValue  cfgValue;
	CDuiString  strText;
	double dbTemp = 0.0;
	int    nBedNo = 0;
	bool   bGetCfg = false;

	bGetCfg = m_tree->GetConfigValue(8 + nIndex * GRID_SETTING_ITEMS_COUNT + 1, cfgValue);
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

	dwInterval = 10;
	bGetCfg = m_tree->GetConfigValue( 8 + nIndex * GRID_SETTING_ITEMS_COUNT + 2, cfgValue);
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
	bGetCfg = m_tree->GetConfigValue(8 + nIndex * GRID_SETTING_ITEMS_COUNT + 3, cfgValue);
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


	bGetCfg = m_tree->GetConfigValue( 8 + nIndex * GRID_SETTING_ITEMS_COUNT + 4, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%lf", &dbTemp)) {
		strText.Format("����%d���ã����±�������������", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}

	if (dbTemp > 42 || dbTemp < 20) {
		strText.Format("����%d���ã����±��������뷶Χ(20~42)", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}
	dwLowAlarm = (DWORD)(dbTemp * 100);

	bGetCfg = m_tree->GetConfigValue( 8 + nIndex * GRID_SETTING_ITEMS_COUNT + 5, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%lf", &dbTemp)) {
		strText.Format("����%d���ã����±�������������", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}

	if (dbTemp > 42 || dbTemp < 20) {
		strText.Format("����%d���ã����±��������뷶Χ(20~42)", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}
	if (dbTemp < dwLowAlarm / 100.0) {
		strText.Format("����%d���ã����±�������ڵ��±���", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}
	dwHighAlarm = (DWORD)(dbTemp * 100);

	bGetCfg = m_tree->GetConfigValue(8 + nIndex * GRID_SETTING_ITEMS_COUNT + 6, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%d", &nBedNo)) {
		strText.Format("����%d���ã�Reader��ش�λ������������", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}

	if ( nBedNo < 0 || nBedNo > MAX_BED_ID ) {
		strText.Format("����%d���ã�Reader��ش�λ�ŵķ�Χ��0��200(����0��ʾû�й�����Reader)", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}
	dwBedNo = nBedNo;

	return TRUE;
}