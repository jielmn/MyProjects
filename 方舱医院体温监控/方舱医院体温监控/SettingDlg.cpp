#include "SettingDlg.h"

/*
  ͨ������         ............................      8��
      7
  //�ֲֳ������� ..............................        3��
  // 2
  ��λ
      ��λ1         ..............................    23 ��
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
	strText.Format("��λ");
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

	strText.Format("ͨ������");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// ������
	pCombo = new CComboUI;
	AddComboItem(pCombo, "δ����", 0);  	
	for (it = g_vArea.begin(); it != g_vArea.end(); ++it, ++i) {
		TArea * pArea = *it;

		strText.Format("%s(��ţ�%lu)", pArea->szAreaName, pArea->dwAreaNo);
		AddComboItem(pCombo, strText, pArea->dwAreaNo);
		if ( m_data.m_dwAreaNo == pArea->dwAreaNo) {
			nIndex = i + 1;
		}
	}
	pCombo->SelectItem(nIndex);
	m_tree->AddNode("������", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// ��������
	pEdit = new CEditUI;
	strText.Format("%lu", m_data.m_dwLayoutGridsCnt);
	pEdit->SetText(strText);
	pEdit->SetName("edGridsCnt");
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("��������", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);
	m_edGridCnt = pEdit;

	// �С���
	pEdit = new CEditUI;
	strText.Format("%lu", m_data.m_dwLayoutColumns);
	pEdit->SetText(strText);
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("ÿҳ��������", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	pEdit = new CEditUI;
	strText.Format("%lu", m_data.m_dwLayoutRows);
	pEdit->SetText(strText);
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("ÿҳ��������", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	// ������������ 
	pCheckBox = new CCheckBoxUI;
	pCheckBox->Selected(m_data.m_bAlarmVoiceOff ? false : true);
	m_tree->AddNode("������������", pTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382 );

	// �Զ�����excel 
	pCheckBox = new CCheckBoxUI;
	pCheckBox->Selected(m_data.m_bAutoSaveExcel ? true : false);
	m_tree->AddNode("�Զ�����excel", pTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);

	// ʮ�ֶ�λê 
	pCheckBox = new CCheckBoxUI;
	pCheckBox->Selected(m_data.m_bCrossAnchor ? true : false);
	m_tree->AddNode("�¶�����ʮ�ֶ�λ", pTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);
}

void  CSettingDlg::InitGridCfg(CMyTreeCfgUI::Node* pTitleNode, DWORD dwIndex) {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pSubTitleNode = NULL;
	strText.Format("��λ%lu��������", dwIndex + 1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666 );

	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;
	CCheckBoxUI * pCheckBox = 0;

	// ���ʱ��
	pCombo = new CComboUI;
	AddComboItem(pCombo, "10��", 0);
	AddComboItem(pCombo, "1����", 1);
	AddComboItem(pCombo, "5����", 2);
	AddComboItem(pCombo, "15����", 3);
	AddComboItem(pCombo, "30����", 4);
	AddComboItem(pCombo, "1Сʱ", 5);

	pCombo->SelectItem(m_data.m_GridCfg[dwIndex].m_dwCollectInterval);
	m_tree->AddNode("�������", pSubTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// ��ʾ����¶�
	//pEdit = new CEditUI;
	//strText.Format("%lu", m_data.m_GridCfg[dwIndex].m_dwMinTemp);
	//pEdit->SetText(strText);
	//pEdit->SetNumberOnly(true);
	//m_tree->AddNode("��ʾ������¶�(��)", pSubTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	// ��ʾ����¶�
	//pEdit = new CEditUI;
	//strText.Format("%lu", m_data.m_GridCfg[dwIndex].m_dwMaxTemp);
	//pEdit->SetText(strText);
	//pEdit->SetNumberOnly(true);
	//m_tree->AddNode("��ʾ������¶�(��)", pSubTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// �ֳֶ������ı�������
	CMyTreeCfgUI::Node* pHandeReaderTitleNode = NULL;
	strText.Format("�ֳֶ�����");
	pHandeReaderTitleNode = m_tree->AddNode(strText, pSubTitleNode, 0, 0, 3, 0xFF666666);

	pEdit = new CEditUI;
	strText.Format("%.2f", m_data.m_GridCfg[dwIndex].m_HandReaderCfg.m_dwHighTempAlarm / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode("���±���(��)", pHandeReaderTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	pEdit = new CEditUI;
	strText.Format("%.2f", m_data.m_GridCfg[dwIndex].m_HandReaderCfg.m_dwLowTempAlarm / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode("���±���(��)", pHandeReaderTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	m_tree->ExpandNode(pHandeReaderTitleNode, false);


	for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
		CMyTreeCfgUI::Node* pReaderTitleNode = NULL;
		strText.Format( "������%c", (char)('A'+j) );
		pReaderTitleNode = m_tree->AddNode(strText, pSubTitleNode, 0, 0, 3, 0xFF666666);

		pEdit = new CEditUI;
		strText.Format("%.2f", m_data.m_GridCfg[dwIndex].m_ReaderCfg[j].m_dwHighTempAlarm / 100.0 );
		pEdit->SetText(strText);
		m_tree->AddNode("���±���(��)", pReaderTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

		pEdit = new CEditUI;
		strText.Format("%.2f", m_data.m_GridCfg[dwIndex].m_ReaderCfg[j].m_dwLowTempAlarm / 100.0);
		pEdit->SetText(strText);
		m_tree->AddNode("���±���(��)", pReaderTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

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

	// ����
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_dwAreaNo = cfgValue.m_tag;
	nCfgRowIndex++;

	// ������Ŀ
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	strText = cfgValue.m_strEdit;
	sscanf_s(strText, "%lu", &m_data.m_dwLayoutGridsCnt);
	nCfgRowIndex++;

	//����
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	strText = cfgValue.m_strEdit;
	sscanf_s(strText, "%d", &nNumber);
	if (nNumber <= 0) {
		strText.Format("ÿҳ������������������");
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}
	m_data.m_dwLayoutColumns = nNumber;
	nCfgRowIndex++;

	// ����
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	strText = cfgValue.m_strEdit;
	sscanf_s(strText, "%d", &nNumber);
	if (nNumber <= 0) {
		strText.Format("ÿҳ������������������");
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}
	m_data.m_dwLayoutRows = nNumber;

	if (m_data.m_dwLayoutColumns * m_data.m_dwLayoutRows <= 1) {
		strText.Format("��������(%d) * ��������(%d) = (%d)���뱣֤����(%d)", m_data.m_dwLayoutColumns, m_data.m_dwLayoutRows,
			m_data.m_dwLayoutColumns * m_data.m_dwLayoutRows, 1);
		::MessageBox(this->GetHWND(), strText, "����", 0);
	}
	nCfgRowIndex++;

	// ������
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_bAlarmVoiceOff = !cfgValue.m_bCheckbox;
	nCfgRowIndex++;

	// �Զ�����excel
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_bAutoSaveExcel = cfgValue.m_bCheckbox;
	nCfgRowIndex++;

	// ʮ�ֶ�λ
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

	// �ɼ����
	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	m_data.m_GridCfg[nIndex].m_dwCollectInterval = cfgValue.m_tag;
	nCfgRowIndex++;

	// �����ʾ�ɶ�
	//bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	//sscanf_s(cfgValue.m_strEdit, "%lu", &m_data.m_GridCfg[nIndex].m_dwMinTemp);
	//nCfgRowIndex++;
	//if (m_data.m_GridCfg[nIndex].m_dwMinTemp < MIN_TEMP_IN_SHOW) {
	//	strText.Format("��λ%d�������ʾ�¶ȱ�����ڵ���%lu��", nIndex + 1, MIN_TEMP_IN_SHOW);
	//	::MessageBox(this->GetHWND(), strText, "����", 0);
	//	return FALSE;
	//}


	// �����ʾ�ɶ�
	//bGetCfg = m_tree->GetConfigValue(nCfgRowIndex, cfgValue);
	//sscanf_s(cfgValue.m_strEdit, "%lu", &m_data.m_GridCfg[nIndex].m_dwMaxTemp);
	//nCfgRowIndex++;
	//if (m_data.m_GridCfg[nIndex].m_dwMinTemp > MAX_TEMP_IN_SHOW) {
	//	strText.Format("��λ%d�������ʾ�¶ȱ���С�ڵ���%lu��", nIndex + 1, MAX_TEMP_IN_SHOW);
	//	::MessageBox(this->GetHWND(), strText, "����", 0);
	//	return FALSE;
	//}

	//if ( m_data.m_GridCfg[nIndex].m_dwMaxTemp <= m_data.m_GridCfg[nIndex].m_dwMinTemp ) {
	//	strText.Format("��λ%d�������ʾ�¶ȱ�����������ʾ�¶�", nIndex + 1);
	//	::MessageBox(this->GetHWND(), strText, "����", 0);
	//	return FALSE;
	//}

	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + 1, cfgValue);
	sscanf_s(cfgValue.m_strEdit, "%lf", &dNumber);
	int nHigh = (int)(dNumber * 100);

	bGetCfg = m_tree->GetConfigValue(nCfgRowIndex + 2, cfgValue);
	sscanf_s(cfgValue.m_strEdit, "%lf", &dNumber);
	int nLow = (int)(dNumber * 100);

	if (nLow >= nHigh) {
		strText.Format("��λ%d�ֳֶ����������±���������ڵ��±���", nIndex + 1);
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}
	else if (nLow < 0 || nHigh < 0) {
		strText.Format("��λ%d�ֳֶ����������±����͵��±�������Ϊ����", nIndex + 1 );
		::MessageBox(this->GetHWND(), strText, "����", 0);
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
			strText.Format("��λ%d������%c�����±���������ڵ��±���", nIndex + 1, (char)('A'+j));
			::MessageBox(this->GetHWND(), strText, "����", 0);
			return FALSE;
		}
		else if (nLow < 0 || nHigh < 0) {
			strText.Format("��λ%d������%c�����±����͵��±�������Ϊ����", nIndex + 1, (char)('A' + j));
			::MessageBox(this->GetHWND(), strText, "����", 0);
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

	strText.Format("�ֲֳ�������");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// ��ʾ����¶�
	pEdit = new CEditUI;
	strText.Format("%.2f", m_data.m_dwHandReaderHighTempAlarm / 100.0 );
	pEdit->SetText(strText);
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("���±���(��)", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// ��ʾ����¶�
	pEdit = new CEditUI;
	strText.Format("%.2f", m_data.m_dwHandReaderLowTempAlarm / 100.0 );
	pEdit->SetText(strText);
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("���±���(��)", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);
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
		strText.Format("�ֲֳ��£����±���������ڸ��±���");
		::MessageBox(this->GetHWND(), strText, "����", 0);
		return FALSE;
	}

	m_data.m_dwHandReaderHighTempAlarm = (DWORD)(dMax * 100.0);
	m_data.m_dwHandReaderLowTempAlarm  = (DWORD)(dMin * 100.0);

	return TRUE;
}


// ���������ı�
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

	// ��������������
	if ( dwNumber > m_data.m_dwLayoutGridsCnt ) {
		int nCnt = m_tree->GetCount();
		DWORD  dwDiff =  dwNumber - m_data.m_dwLayoutGridsCnt;

		// ��ʼ�����ӵĸ��ӣ���Ĭ��ֵ
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

		// ������ʾ
		for (DWORD i = 0; i < dwDiff; i++) {
			InitGridCfg(m_pBedTitleNode, m_data.m_dwLayoutGridsCnt+i);
		}
	}
	// ������������
	else {
		int nCnt = m_tree->GetCount();
		DWORD  dwDiff = m_data.m_dwLayoutGridsCnt - dwNumber;
		for ( DWORD i = 0; i < dwDiff; i++ ) {
			m_tree->RemoveAt( nCnt - SUBITEMS_COUNT_PER_BED * dwDiff );
		}
	}
	m_data.m_dwLayoutGridsCnt = dwNumber;
}