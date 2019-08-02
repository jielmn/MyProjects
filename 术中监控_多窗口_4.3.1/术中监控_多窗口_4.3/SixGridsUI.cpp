#include "SixGridsUI.h"


CSixGridsUI::CSixGridsUI() {
	memset(m_edits, 0, sizeof(m_edits));
	m_top = 0;
	m_bottom = 0;
	m_nMode = 0;
}

CSixGridsUI::~CSixGridsUI() {

}

LPCTSTR  CSixGridsUI::GetClass() const {
	return _T("SixGrids");
}

void CSixGridsUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(_T("SixGrids.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();  
		return;                   
	}

	m_top = static_cast<DuiLib::CHorizontalLayoutUI*>(m_pManager->FindControl("top"));
	m_bottom = static_cast<DuiLib::CHorizontalLayoutUI*>(m_pManager->FindControl("bottom"));
	if (m_nMode == 0) {
		m_top->SetVisible(false);
	}
	else {
		m_bottom->SetVisible(false);
	}
}

void CSixGridsUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void  CSixGridsUI::SetMode(int nMode) {
	m_nMode = nMode;
}








CSevenGridsUI::CSevenGridsUI() {
	memset(m_edits, 0, sizeof(m_edits));
	m_top = 0;
	m_bottom = 0;
	m_nMode = 0;
}

CSevenGridsUI::~CSevenGridsUI() {

}

void  CSevenGridsUI::SetMode(int nMode) {
	m_nMode = nMode;
}

LPCTSTR CSevenGridsUI::GetClass() const {
	return _T("SevenGrids");
}

void  CSevenGridsUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(_T("SevenGrids.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}

	m_top = static_cast<DuiLib::CHorizontalLayoutUI*>(m_pManager->FindControl("top"));
	m_bottom = static_cast<DuiLib::CHorizontalLayoutUI*>(m_pManager->FindControl("bottom"));
	if (m_nMode == 0) {
		m_top->SetVisible(false);
	}
	else {
		m_bottom->SetVisible(false);
	}
}

void   CSevenGridsUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue); 
}