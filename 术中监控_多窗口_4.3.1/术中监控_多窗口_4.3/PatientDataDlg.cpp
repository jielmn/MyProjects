#include "PatientDataDlg.h"


CPatientDataDlg::CPatientDataDlg() {
	m_tree = 0;
}

void   CPatientDataDlg::Notify(DuiLib::TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}

void   CPatientDataDlg::InitWindow() {
	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl(MYTREE_PATIENT_DATA_NAME);
	InitInfo(); 
	InitData();

	WindowImplBase::InitWindow();
}

LRESULT  CPatientDataDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CPatientDataDlg::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, MYTREE_CLASS_NAME)) {
		return new CMyTreeCfgUI(180);
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void   CPatientDataDlg::InitInfo() {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pTitleNode = NULL;
	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;
	CCheckBoxUI * pCheckBox = 0;
	CDateTimeUI * pDateTime = 0;
	

	strText.Format("���˻�����Ϣ");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// ����
	pEdit = new CEditUI;
	m_tree->AddNode("����", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382); 

	// �Ա�
	pCombo = new CComboUI;
	AddComboItem(pCombo, "��", 0);
	AddComboItem(pCombo, "Ů", 1);
	pCombo->SelectItem(0);
	m_tree->AddNode("�Ա�", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382);

	// ����
	pEdit = new CEditUI;
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("����", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// �����
	pEdit = new CEditUI;
	m_tree->AddNode("�����", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// סԺ��
	pEdit = new CEditUI;
	m_tree->AddNode("סԺ��", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// ��Ժ����
	pDateTime = new DuiLib::CDateTimeUI;
	m_tree->AddNode("��Ժ����", pTitleNode, 0, pDateTime, 2, 0xFF386382, 2, 0xFF386382); 

	// �Ʊ�
	pEdit = new CEditUI;
	m_tree->AddNode("�Ʊ�", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// ����
	pEdit = new CEditUI;
	m_tree->AddNode("����", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// ����
	pEdit = new CEditUI;
	m_tree->AddNode("����", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);
}

void  CPatientDataDlg::AddComboItem(CComboUI * pCombo, const char * szItem, UINT_PTR tag) {
	CListLabelElementUI * pElement = new CListLabelElementUI;
	pElement->SetText(szItem);
	pElement->SetTag(tag);
	pCombo->Add(pElement);
}

void  CPatientDataDlg::InitData() {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pTitleNode = NULL;
	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;
	CCheckBoxUI * pCheckBox = 0;
	CDateTimeUI * pDateTime = 0;
	CMyTreeCfgUI::Node* pSubTitleNode = NULL;


	strText.Format("���˷���������");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// ����
	strText.Format("����");
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666);

	
}