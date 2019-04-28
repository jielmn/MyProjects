#include "EditableButtonUI.h"

#define EDITABLE_BUTTON_XML      \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>" \
"<Window>"  \
"<HorizontalLayout>"  \
"<Edit name=\"edt\" textcolor=\"#FF000000\" visible=\"false\" />" \
"<Button name=\"btn\" textcolor=\"#FFFFFFFF\" textpadding=\"4,0,4,0\" text=\"\" align=\"left\" />" \
"</HorizontalLayout>"  \
"</Window>"

CEditableButtonUI::CEditableButtonUI() { 
	m_dwBtnColor = 0xFF000000;
	m_dwEditColor = 0xFF000000;
	m_nBtnFont = -1;
	m_nEdtFont = -1; 
	m_bInited = FALSE;
}

CEditableButtonUI::~CEditableButtonUI() {

}

void CEditableButtonUI::DoInit() {
	if (m_bInited) {
		return;                     
	}

	CDialogBuilder builder;
	CContainerUI* pChildWindow =
		static_cast<CHorizontalLayoutUI*>(
			builder.Create( _T(EDITABLE_BUTTON_XML), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
		m_pManager->AddNotifier(this);

		m_btn  = static_cast<CButtonUI*>(m_pManager->FindControl(_T("btn")));
		m_edit = static_cast<CEditUI*>(m_pManager->FindControl(_T("edt")));
		m_btn->SetText(m_strMyText);
		m_edit->SetText(m_strMyText);
		m_btn->SetTextColor(m_dwBtnColor);
		m_edit->SetTextColor(m_dwEditColor);
		m_btn->SetFont(m_nBtnFont);
		m_edit->SetFont(m_nEdtFont);
		m_btn->SetHotImage(m_strHotImg);
	}
	else {
		this->RemoveAll();
	}

	m_bInited = TRUE;
}

void CEditableButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	if (0 == StrICmp(pstrName, "text")) {
		m_strMyText = pstrValue;
	}
	else if (0 == StrICmp(pstrName, "editcolor")) {
		sscanf(pstrValue, "#%x", &m_dwEditColor);
	}
	else if (0 == StrICmp(pstrName, "btncolor")) {    
		sscanf(pstrValue, "#%x", &m_dwBtnColor); 
	}
	else if (0 == StrICmp(pstrName, "editfont")) {
		sscanf(pstrValue, "%d", &m_nEdtFont);
	}
	else if (0 == StrICmp(pstrName, "btnfont")) {
		sscanf(pstrValue, "%d", &m_nBtnFont);
	}
	else if (0 == StrICmp(pstrName, "hotimage")) {
		m_strHotImg = pstrValue;
	}
	else {
		CContainerUI::SetAttribute(pstrName, pstrValue);
	}	
}

void CEditableButtonUI::Notify(TNotifyUI& msg) { 
	if (msg.sType == "click") {
		if (msg.pSender == m_btn) {
			m_btn->SetVisible(false);
			m_edit->SetText(m_btn->GetText());
			m_edit->SetVisible(true);
			m_edit->SetFocus();
		}
	}
	else if (msg.sType == "killfocus") {
		if (msg.pSender == m_edit) {
			m_edit->SetVisible(false);
			m_btn->SetText(m_edit->GetText());
			m_btn->SetVisible(true); 
			m_pManager->SendNotify(this, "textchanged");
		}
	}
}

CDuiString CEditableButtonUI::GetText() const {
	return m_btn->GetText();
}

void CEditableButtonUI::SetEnabled(bool bEnable /*= true*/) {
	if (m_btn)
		m_btn->SetEnabled(bEnable);
	if (m_edit)
		m_edit->SetEnabled(bEnable);
}