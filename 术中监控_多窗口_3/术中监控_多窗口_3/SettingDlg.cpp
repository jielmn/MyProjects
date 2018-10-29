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
	strText.Format("����");
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

	strText.Format("ͨ������");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// ������
	pCombo = new CComboUI;
	AddComboItem(pCombo, "δ����", 0);  	
	for (it = g_vArea.begin(); it != g_vArea.end(); ++it, ++i) {
		TArea * pArea = *it;

		strText.Format("%s(��ţ�%lu)", pArea->szAreaName, pArea->dwAreaNo);
		AddComboItem(pCombo, strText, pArea->dwAreaNo);
		if ( g_data.m_CfgData.m_dwAreaNo == pArea->dwAreaNo) {
			nIndex = i + 1;
		}
	}
	pCombo->SelectItem(nIndex);
	m_tree->AddNode("������", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// �С���
	pEdit = new CEditUI;
	strText.Format("%lu", g_data.m_CfgData.m_dwLayoutColumns);
	pEdit->SetText(strText);
	m_tree->AddNode("��������", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	pEdit = new CEditUI;
	strText.Format("%lu", g_data.m_CfgData.m_dwLayoutRows);
	pEdit->SetText(strText);
	m_tree->AddNode("��������", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	// Ƥ��
	pCombo = new CComboUI;
	AddComboItem(pCombo, "����ʯ", 0);
	AddComboItem(pCombo, "����ֽ", 1);
	pCombo->SelectItem(g_data.m_CfgData.m_dwSkinIndex);
	m_tree->AddNode("ѡ��Ƥ��", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// ������������ 
	pCheckBox = new CCheckBoxUI;
	pCheckBox->Selected(g_data.m_CfgData.m_bAlarmVoiceOff ? false : true);
	m_tree->AddNode("������������", pTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382 );
}

void  CSettingDlg::InitGridCfg(CMyTreeCfgUI::Node* pTitleNode, DWORD dwIndex) {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pSubTitleNode = NULL;
	strText.Format("����%lu��������", dwIndex + 1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666 );

	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;
	CCheckBoxUI * pCheckBox = 0;

	// ����/����
	pCheckBox = new CCheckBoxUI;
	pCheckBox->Selected(g_data.m_CfgData.m_GridCfg[dwIndex].m_bSwitch ? true : false);
	m_tree->AddNode("�Ƿ�����ȫ��������", pSubTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);

	// ���ʱ��
	pCombo = new CComboUI;
	AddComboItem(pCombo, "10��", 0);
	AddComboItem(pCombo, "1����", 1);
	AddComboItem(pCombo, "5����", 2);
	AddComboItem(pCombo, "15����", 3);
	AddComboItem(pCombo, "30����", 4);
	AddComboItem(pCombo, "1Сʱ", 5);

	pCombo->SelectItem(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwCollectInterval);
	m_tree->AddNode("�������", pSubTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// ��ʾ����¶�
	pCombo = new CComboUI;
	AddComboItem(pCombo, "20��", 0);
	AddComboItem(pCombo, "24��", 1);
	AddComboItem(pCombo, "28��", 2);
	AddComboItem(pCombo, "32��", 3);
	pCombo->SelectItem(g_data.m_CfgData.m_GridCfg[dwIndex].m_dwMinTemp);
	m_tree->AddNode("��ʾ������¶�", pSubTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	CMyTreeCfgUI::Node* pSubTitleNode_1 = NULL;
	strText.Format("������");
	pSubTitleNode_1 = m_tree->AddNode(strText, pSubTitleNode, 0, 0, 3, 0xFF666666);

	for ( DWORD i = 0; i < MAX_READERS_PER_GRID; i++ ) {

		CMyTreeCfgUI::Node* pSubTitleNode_2 = NULL;
		strText.Format("������%lu", i+1);
		pSubTitleNode_2 = m_tree->AddNode(strText, pSubTitleNode_1, 0, 0, 3, 0xFF666666);

		pCheckBox = new CCheckBoxUI;
		pCheckBox->Selected(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_bSwitch ? true : false);
		m_tree->AddNode("�Ƿ�����", pSubTitleNode_2, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);

		// ���±���
		pEdit = new CEditUI;
		strText.Format("%.2f", g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_dwLowTempAlarm / 100.0 );
		pEdit->SetText(strText);
		m_tree->AddNode("���±���", pSubTitleNode_2, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

		// ���±���
		pEdit = new CEditUI;
		strText.Format("%.2f", g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_dwHighTempAlarm / 100.0);
		pEdit->SetText(strText);
		m_tree->AddNode("���±���", pSubTitleNode_2, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

		// ����
		pEdit = new CEditUI;
		strText.Format("%lu", g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_dwBed);
		pEdit->SetText(strText);
		m_tree->AddNode("Reader��ش���", pSubTitleNode_2, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

		if ( i > 0 ) {
			m_tree->ExpandNode(pSubTitleNode_2, false);
		}
	}

	m_tree->ExpandNode(pSubTitleNode, false);
}

void  CSettingDlg::OnBtnOk(DuiLib::TNotifyUI& msg) {
	m_data = g_data.m_CfgData;

	if (!GetCommonConfig()) {
		return;
	}

	for (int i = 0; i < MAX_GRID_COUNT; i++) {
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

	// ����
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_dwAreaNo = cfgValue.m_tag;
	nCfgRowIndex++;

	//����
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%d", &nNumber)) {
		strText.Format("������������������");
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}

	if (nNumber <= 0) {
		strText.Format("������������������");
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}
	m_data.m_dwLayoutColumns = nNumber;
	nCfgRowIndex++;

	// ����
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	strText = cfgValue.m_strEdit;
	if (1 != sscanf_s(strText, "%d", &nNumber)) {
		strText.Format("����������������");
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}

	if (nNumber <= 0) {
		strText.Format("������������������");
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}
	m_data.m_dwLayoutRows = nNumber;

	if (m_data.m_dwLayoutColumns * m_data.m_dwLayoutRows > MAX_GRID_COUNT) {
		strText.Format("��������(%d) * ��������(%d) = (%d)���뱣֤С��(%d)", m_data.m_dwLayoutColumns, m_data.m_dwLayoutRows,
			   m_data.m_dwLayoutColumns * m_data.m_dwLayoutRows, MAX_GRID_COUNT);
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}
	nCfgRowIndex++;

	// Ƥ��
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_dwSkinIndex = cfgValue.m_tag;
	nCfgRowIndex++;

	// ������
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_bAlarmVoiceOff = !cfgValue.m_bCheckbox;

	return TRUE;
}

BOOL  CSettingDlg::GetGridConfig(int nIndex) {
	CDuiString  strText;
	CMyTreeCfgUI::ConfigValue  cfgValue;
	bool bGetCfg = false;
	int  nCfgRowIndex = 7 + 50 * nIndex + 2 - 1;
	int  nNumber = 0;
	double dNumber = 0.0;

	// ���ӿ���
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_GridCfg[nIndex].m_bSwitch = cfgValue.m_bCheckbox;
	nCfgRowIndex++;

	// �ɼ����
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_GridCfg[nIndex].m_dwCollectInterval = cfgValue.m_tag;
	nCfgRowIndex++;

	// �����ʾ�ɶ�
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_GridCfg[nIndex].m_dwMinTemp = cfgValue.m_tag;
	nCfgRowIndex++;

	// ��ȡReader����
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + 5 * i + 2, cfgValue);
		m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_bSwitch = cfgValue.m_bCheckbox;

		bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + 5 * i + 3, cfgValue);
		strText = cfgValue.m_strEdit;
		if (1 != sscanf_s(strText, "%lf", &dNumber)) {
			strText.Format("����%d������%d���ã����±�������������", nIndex + 1, i+1);
			::MessageBox(this->GetHWND(), strText, "����", 0);
			return FALSE;
		}

		if (dNumber > 42 || dNumber < 20) {
			strText.Format("����%d������%d���ã����±��������뷶Χ(20~42)", nIndex + 1, i+1);
			::MessageBox(this->GetHWND(), strText, "����", 0);
			return FALSE;
		}
		m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_dwLowTempAlarm = (DWORD)(dNumber * 100);
		
		bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + 5 * i + 4, cfgValue);
		strText = cfgValue.m_strEdit;
		if (1 != sscanf_s(strText, "%lf", &dNumber)) {
			strText.Format("����%d������%d���ã����±�������������", nIndex + 1, i + 1);
			::MessageBox(this->GetHWND(), strText, "����", 0);
			return FALSE;
		}

		if (dNumber > 42 || dNumber < 20) {
			strText.Format("����%d������%d���ã����±��������뷶Χ(20~42)", nIndex + 1, i + 1);
			::MessageBox(this->GetHWND(), strText, "����", 0);
			return FALSE;
		}
		m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_dwHighTempAlarm = (DWORD)(dNumber * 100);

		if (m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_dwHighTempAlarm < m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_dwLowTempAlarm) {
			strText.Format("����%d������%d���ã����±�������ڵ��±���",  nIndex + 1, i+1);
			::MessageBox(this->GetHWND(), strText, "����", 0);
			return FALSE;
		}

		bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + 5 * i + 5, cfgValue);
		strText = cfgValue.m_strEdit;
		if (1 != sscanf_s(strText, "%d", &nNumber)) {
			strText.Format("����%d������%d���ã���������������", nIndex + 1, i + 1);
			::MessageBox(this->GetHWND(), strText, "����", 0);
			return FALSE;
		}
		m_data.m_GridCfg[nIndex].m_ReaderCfg[i].m_dwBed = nNumber;
	}

	return TRUE;
}

