#include "PatientDataDlg.h"
#include "SixGridsUI.h"

CPatientDataDlg::CPatientDataDlg() {
	m_tree = 0;
}

void   CPatientDataDlg::Notify(DuiLib::TNotifyUI& msg) {
	if (msg.sType == "windowinit") {
		OnMyInited();
	}
	WindowImplBase::Notify(msg);
}

void   CPatientDataDlg::InitWindow() {
	WindowImplBase::InitWindow();
}

void  CPatientDataDlg::OnMyInited() {
	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl(MYTREE_PATIENT_DATA_NAME);
	InitInfo();
	InitData();
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
	

	strText.Format("病人基础信息");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// 姓名
	pEdit = new CEditUI;
	m_tree->AddNode("姓名", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382); 

	// 性别
	pCombo = new CComboUI;
	AddComboItem(pCombo, "男", 0);
	AddComboItem(pCombo, "女", 1);
	pCombo->SelectItem(0);
	m_tree->AddNode("性别", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382);

	// 年龄
	pEdit = new CEditUI;
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("年龄", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 门诊号
	pEdit = new CEditUI;
	m_tree->AddNode("门诊号", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 住院号
	pEdit = new CEditUI;
	m_tree->AddNode("住院号", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 入院日期
	pDateTime = new DuiLib::CDateTimeUI;
	m_tree->AddNode("入院日期", pTitleNode, 0, pDateTime, 2, 0xFF386382, 2, 0xFF386382); 

	// 科别
	pEdit = new CEditUI;
	m_tree->AddNode("科别", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 病室
	pEdit = new CEditUI;
	m_tree->AddNode("病室", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 床号
	pEdit = new CEditUI;
	m_tree->AddNode("床号", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);
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
	CMyTreeCfgUI::Node* pSubTitleNode_1 = NULL;
	CSixGridsUI * pSixGrids = 0;
	CSevenGridsUI * pSevenGrids = 0;

	strText.Format("病人非体温数据");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// 脉搏  
	strText.Format("脉搏");   
	pSixGrids = new CSixGridsUI;
	pSixGrids->SetMode(1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pSixGrids, 3, 0xFF666666); 


	for (int i = 0; i < 7; i++) {
		pSixGrids = new CSixGridsUI;
		m_tree->AddNode("1", pSubTitleNode, 0, pSixGrids, 2, 0xFF386382, 2, 0xFF386382, 30 );     
	}

	// 呼吸 
	strText.Format("呼吸");
	pSixGrids = new CSixGridsUI;
	pSixGrids->SetMode(1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pSixGrids, 3, 0xFF666666);

	for (int i = 0; i < 7; i++) {
		pSixGrids = new CSixGridsUI;
		m_tree->AddNode("1", pSubTitleNode, 0, pSixGrids, 2, 0xFF386382, 2, 0xFF386382, 30);
	}
	 
	// 其他数据
	strText.Format("其他数据");
	pSevenGrids = new CSevenGridsUI;
	pSevenGrids->SetMode(1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pSevenGrids, 3, 0xFF666666);

	const char * item_title[7] = { "大便次数", "尿量(次) ml", "总入量 ml", "总出量 ml", "血压 kpa", "体重 kg", "过敏药物" };
	for (int i = 0; i < 7; i++) {
		pSevenGrids = new CSevenGridsUI;
		m_tree->AddNode(item_title[i], pSubTitleNode, 0, pSevenGrids, 2, 0xFF386382, 2, 0xFF386382, 30);  
	}
}