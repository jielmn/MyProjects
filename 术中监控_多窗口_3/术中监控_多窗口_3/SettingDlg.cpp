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
		//int a = m_tree->CalculateMinHeight();
		//DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)m_tree->GetParent();
		//SIZE tParentScrollPos = pParent->GetScrollPos();
		//SIZE tParentScrollRange = pParent->GetScrollRange();
		//int b = 100;

		//m_tree->SetMinHeight(40000);
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

	strText.Format("ͨ������");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// ������
	pCombo = new CComboUI;
	AddComboItem(pCombo, "δ����", 0);  
	pCombo->SelectItem(0);
	m_tree->AddNode("������", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// �С���
	pEdit = new CEditUI;
	strText.Format("%lu", 0);
	pEdit->SetText(strText);
	m_tree->AddNode("��������", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	pEdit = new CEditUI;
	strText.Format("%lu", 0);
	pEdit->SetText(strText);
	m_tree->AddNode("��������", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	// Ƥ��
	pCombo = new CComboUI;
	AddComboItem(pCombo, "����ʯ", 0);
	AddComboItem(pCombo, "����ֽ", 1);
	pCombo->SelectItem(0);
	m_tree->AddNode("ѡ��Ƥ��", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// ������������ 
	pCheckBox = new CCheckBoxUI;
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
	m_tree->AddNode("�Ƿ�����ȫ��������", pSubTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);

	// ���ʱ��
	pCombo = new CComboUI;
	AddComboItem(pCombo, "10��", 0);
	AddComboItem(pCombo, "1����", 1);
	AddComboItem(pCombo, "5����", 2);
	AddComboItem(pCombo, "15����", 3);
	AddComboItem(pCombo, "30����", 4);
	AddComboItem(pCombo, "1Сʱ", 5);

	pCombo->SelectItem(0);
	m_tree->AddNode("�������", pSubTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// ��ʾ����¶�
	pCombo = new CComboUI;
	AddComboItem(pCombo, "20��", 0);
	AddComboItem(pCombo, "24��", 1);
	AddComboItem(pCombo, "28��", 2);
	AddComboItem(pCombo, "32��", 3);
	pCombo->SelectItem(0);
	m_tree->AddNode("��ʾ������¶�", pSubTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	CMyTreeCfgUI::Node* pSubTitleNode_1 = NULL;
	strText.Format("������");
	pSubTitleNode_1 = m_tree->AddNode(strText, pSubTitleNode, 0, 0, 3, 0xFF666666);

	for ( DWORD i = 0; i < MAX_READERS_PER_GRID; i++ ) {

		CMyTreeCfgUI::Node* pSubTitleNode_2 = NULL;
		strText.Format("������%lu", i+1);
		pSubTitleNode_2 = m_tree->AddNode(strText, pSubTitleNode_1, 0, 0, 3, 0xFF666666);

		pCheckBox = new CCheckBoxUI;
		m_tree->AddNode("�Ƿ�����", pSubTitleNode_2, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);

		// ���±���
		pEdit = new CEditUI;
		strText.Format("%.2f", 35.0);
		pEdit->SetText(strText);
		m_tree->AddNode("���±���", pSubTitleNode_2, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

		// ���±���
		pEdit = new CEditUI;
		strText.Format("%.2f", 40.0);
		pEdit->SetText(strText);
		m_tree->AddNode("���±���", pSubTitleNode_2, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

		// ����
		pEdit = new CEditUI;
		strText.Format("%lu", 0);
		pEdit->SetText(strText);
		m_tree->AddNode("Reader��ش���", pSubTitleNode_2, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

		if ( i > 0 ) {
			m_tree->ExpandNode(pSubTitleNode_2, false);
		}
	}

	m_tree->ExpandNode(pSubTitleNode, false);
}

