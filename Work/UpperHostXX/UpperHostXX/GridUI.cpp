#include "GridUI.h"


CGridUI::CGridUI() {
	m_nIndex = 0;
	m_lblIndex = 0;
	m_textValue = 0;
}

CGridUI::~CGridUI() {

}

void CGridUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create("grid.xml", (UINT)0, 0, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}

	m_lblIndex = static_cast<CLabelUI*>(m_pManager->FindControl("lblIndex"));
	m_textValue = static_cast<CTextUI*>(m_pManager->FindControl("textValue"));
	m_textValue->SetShowHtml(true);

	SetIndex();
	SetText();
}

void CGridUI::SetIndex(int nIndex) {
	m_nIndex = nIndex;
	SetIndex();
}

int  CGridUI::GetIndex() const {
	return m_nIndex;
}

void CGridUI::SetText(const char * szText) {
	m_strText = szText;
	SetText();
}

CDuiString CGridUI::GetText() const {
	return m_strText;
}

void CGridUI::SetIndex() {
	if (m_lblIndex) {
		CDuiString strText;
		strText.Format("%d", m_nIndex);
		m_lblIndex->SetText(strText);
	}
}

void CGridUI::SetText() {
	if (m_textValue) {
		m_textValue->SetText(m_strText);
	}
}