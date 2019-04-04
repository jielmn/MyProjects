#include "ComDataUI.h"

CComDataUI::CComDataUI() {
	m_richData = 0;
	m_lblTime = 0;
}

CComDataUI::~CComDataUI() {

}

LPCTSTR CComDataUI::GetClass() const {
	return "ComData";
}

void CComDataUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(_T("ComDataItem.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll(); 
		return;
	}

	m_richData = static_cast<CRichEditUI*>(m_pManager->FindControl(_T("rchData")));
	m_lblTime = static_cast<CLabelUI*>(m_pManager->FindControl(_T("lblTime")));

	m_richData->SetText(m_strData);
	m_lblTime->SetText(m_strTime); 
}

void CComDataUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CComDataUI::SetDataText(LPCTSTR pstrText) {
	m_strData = pstrText;
}

void CComDataUI::SetTimeText(LPCTSTR pstrText) {
	m_strTime = pstrText;
}