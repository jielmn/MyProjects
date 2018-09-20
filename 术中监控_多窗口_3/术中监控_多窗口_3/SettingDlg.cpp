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
	strText.Format("窗格");
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

	strText.Format("通用设置");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// 病区号
	pCombo = new CComboUI;
	AddComboItem(pCombo, "未设置", 0);  
	pCombo->SelectItem(0);
	m_tree->AddNode("病区号", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// 行、列
	pEdit = new CEditUI;
	strText.Format("%lu", 0);
	pEdit->SetText(strText);
	m_tree->AddNode("窗格列数", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	pEdit = new CEditUI;
	strText.Format("%lu", 0);
	pEdit->SetText(strText);
	m_tree->AddNode("窗格行数", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

	// 皮肤
	pCombo = new CComboUI;
	AddComboItem(pCombo, "黑曜石", 0);
	AddComboItem(pCombo, "白宣纸", 1);
	pCombo->SelectItem(0);
	m_tree->AddNode("选择皮肤", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// 报警声音开关 
	pCheckBox = new CCheckBoxUI;
	m_tree->AddNode("报警声音开关", pTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382 );
}

void  CSettingDlg::InitGridCfg(CMyTreeCfgUI::Node* pTitleNode, DWORD dwIndex) {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pSubTitleNode = NULL;
	strText.Format("窗格%lu参数设置", dwIndex + 1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666 );

	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;
	CCheckBoxUI * pCheckBox = 0;

	// 启用/禁用
	pCheckBox = new CCheckBoxUI;
	m_tree->AddNode("是否启用全部读卡器", pSubTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);

	// 间隔时间
	pCombo = new CComboUI;
	AddComboItem(pCombo, "10秒", 0);
	AddComboItem(pCombo, "1分钟", 1);
	AddComboItem(pCombo, "5分钟", 2);
	AddComboItem(pCombo, "15分钟", 3);
	AddComboItem(pCombo, "30分钟", 4);
	AddComboItem(pCombo, "1小时", 5);

	pCombo->SelectItem(0);
	m_tree->AddNode("采样间隔", pSubTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	// 显示最低温度
	pCombo = new CComboUI;
	AddComboItem(pCombo, "20℃", 0);
	AddComboItem(pCombo, "24℃", 1);
	AddComboItem(pCombo, "28℃", 2);
	AddComboItem(pCombo, "32℃", 3);
	pCombo->SelectItem(0);
	m_tree->AddNode("显示的最低温度", pSubTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382 );

	CMyTreeCfgUI::Node* pSubTitleNode_1 = NULL;
	strText.Format("读卡器");
	pSubTitleNode_1 = m_tree->AddNode(strText, pSubTitleNode, 0, 0, 3, 0xFF666666);

	for ( DWORD i = 0; i < MAX_READERS_PER_GRID; i++ ) {

		CMyTreeCfgUI::Node* pSubTitleNode_2 = NULL;
		strText.Format("读卡器%lu", i+1);
		pSubTitleNode_2 = m_tree->AddNode(strText, pSubTitleNode_1, 0, 0, 3, 0xFF666666);

		pCheckBox = new CCheckBoxUI;
		m_tree->AddNode("是否启用", pSubTitleNode_2, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);

		// 低温报警
		pEdit = new CEditUI;
		strText.Format("%.2f", 35.0);
		pEdit->SetText(strText);
		m_tree->AddNode("低温报警", pSubTitleNode_2, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

		// 高温报警
		pEdit = new CEditUI;
		strText.Format("%.2f", 40.0);
		pEdit->SetText(strText);
		m_tree->AddNode("高温报警", pSubTitleNode_2, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

		// 床号
		pEdit = new CEditUI;
		strText.Format("%lu", 0);
		pEdit->SetText(strText);
		m_tree->AddNode("Reader相关床号", pSubTitleNode_2, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382 );

		if ( i > 0 ) {
			m_tree->ExpandNode(pSubTitleNode_2, false);
		}
	}

	m_tree->ExpandNode(pSubTitleNode, false);
}

