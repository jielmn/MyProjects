#include "SettingDlg.h"

#define EIDT_TEXT_COLOR    0xFF386382
#define EDIT_FONT_INDEX    2

#define COMMON_ITEMS_COUNT   4
#define GRID_SETTING_ITEMS_COUNT  3

CSettingDlg::CSettingDlg() {

}


void   CSettingDlg::Notify(DuiLib::TNotifyUI& msg) {
	CDuiString name = msg.pSender->GetName();

	if (msg.sType == "killfocus") {
		if (name == "a" || name == "b") {
			m_tree->RemoveAt(3);
		}
	}
	WindowImplBase::Notify(msg); 
}

void   CSettingDlg::InitWindow() {
	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl(MYTREE_CONFIG_NAME);

	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pTitleNode = NULL;
	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;
	CCheckBoxUI * pCheckBox = 0;
	int i = 0;
	int nIndex = 0;

	strText.Format("通用设置");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// 行、列
	pEdit = new CEditUI;
	pEdit->SetName("a");
	strText.Format("%lu", g_data.m_CfgData.m_dwLayoutColumns);
	pEdit->SetText(strText);
	m_tree->AddNode("窗格列数", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	pEdit = new CEditUI;
	pEdit->SetName("b");
	strText.Format("%lu", g_data.m_CfgData.m_dwLayoutRows);
	pEdit->SetText(strText);
	m_tree->AddNode("窗格行数", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	for (int i = 0; i < 4; i++) {
		DuiLib::CDuiString  strText;
		DWORD dwIndex = i;

		CMyTreeCfgUI::Node* pSubTitleNode = NULL;
		strText.Format("床位%lu参数设置", i + 1);
		pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666);

		CComboUI * pCombo = 0;
		CListLabelElementUI * pElement = 0;
		CEditUI * pEdit = 0;
		CCheckBoxUI * pCheckBox = 0;

		// 启用/禁用
		pCheckBox = new CCheckBoxUI;
		pCheckBox->Selected(g_data.m_CfgData.m_GridCfg[dwIndex].m_bSwitch ? true : false);
		m_tree->AddNode("是否启用全部读卡器", pSubTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);
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

