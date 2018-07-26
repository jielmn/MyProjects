#include "SettingDlg.h"


CSettingDlg::CSettingDlg() {

}


void   CSettingDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();

	//if (msg.sType == "click") {
	//	if (name == BUTTON_OK_NAME) {
	//		OnBtnOk(msg);
	//	}
	//	else {
	//		OnClick(msg);
	//	}
	//}
	WindowImplBase::Notify(msg); 
}

void   CSettingDlg::InitWindow() {
	//m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl(MYTREE_CONFIG_NAME);
	//assert(m_tree);

	//InitCommonCfg();

	//m_tree->SetMinHeight(950);
	WindowImplBase::InitWindow(); 
}

//LRESULT  CSettingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
//	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
//}

DuiLib::CControlUI * CSettingDlg::CreateControl(LPCTSTR pstrClass) {
	//if (0 == strcmp(pstrClass, MYTREE_CLASS_NAME)) {
	//	return new CMyTreeCfgUI();
	//}
	return WindowImplBase::CreateControl(pstrClass);
}

void  CSettingDlg::InitCommonCfg() {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pTitleNode = NULL;
	strText.Format(COMMON_SETTING_TEXT);
	pTitleNode = m_tree->AddNode(strText);

	CEditUI * pEdit = new CEditUI;
	SetEditStyle(pEdit);
	strText.Format("%lu", g_dwLayoutColumns);
	pEdit->SetText(strText);
	m_tree->AddNode(GRIDS_COLUMNS_TEXT, pTitleNode, 0, pEdit);
}

void  CSettingDlg::SetEditStyle(CEditUI * pEdit) {
	pEdit->SetTextColor(0xFF386382);
	pEdit->SetFont(2);
}

void  CSettingDlg::OnBtnOk(DuiLib::TNotifyUI& msg) {

}

void  CSettingDlg::OnClick(DuiLib::TNotifyUI& msg) {
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
				}
			}
		}
		pFindControl = pFindControl->GetParent();
		nDepth++;
	}
}