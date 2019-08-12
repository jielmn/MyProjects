#include "SixGridsUI.h"


CSixGridsUI::CSixGridsUI() {
	memset(m_edits, 0, sizeof(m_edits));
	m_top = 0;
	m_bottom = 0;
	m_nMode = 0;
	m_bNumberOnly = TRUE;
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

	CDuiString strText;
	for (int i = 0; i < 6; i++) {
		strText.Format("edt_%d", i + 1);
		m_edits[i] = static_cast<DuiLib::CEditUI*>(m_pManager->FindControl(strText));
		if (!m_bNumberOnly) {
			m_edits[i]->SetNumberOnly(false);
		}
	}
}

void CSixGridsUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void  CSixGridsUI::SetMode(int nMode) {
	m_nMode = nMode; 
}

void  CSixGridsUI::SetValues(int nIndex, const char * szValue) {
	if (nIndex >= 0 && nIndex < 6) {
		if ( szValue)
			m_edits[nIndex]->SetText(szValue);
		else
			m_edits[nIndex]->SetText("");
	}
}

CDuiString  CSixGridsUI::GetValues(int nIndex) {
	if (nIndex >= 0 && nIndex < 6) {
		return m_edits[nIndex]->GetText();
	}
	return "";
}

void  CSixGridsUI::SetNumberOnly(BOOL bOnly) {
	m_bNumberOnly = bOnly;
}








CSevenGridsUI::CSevenGridsUI() {
	memset(m_edits, 0, sizeof(m_edits));
	memset(m_labels, 0, sizeof(m_labels));
	m_top = 0;
	m_bottom = 0;
	m_nMode = 0;
	m_bNumberOnly = TRUE;
	m_nFont = 2;
}

CSevenGridsUI::~CSevenGridsUI() {

}

void  CSevenGridsUI::SetMode(int nMode) {
	m_nMode = nMode;
}

void  CSevenGridsUI::SetNumberOnly(BOOL bOnly) {
	m_bNumberOnly = bOnly;
}

void  CSevenGridsUI::SetWeekStr(CDuiString * pWeek, DWORD dwSize) {
	assert(dwSize >= 7);
	if (dwSize < 7)
		return;

	for (int i = 0; i < 7; i++) {
		m_week_days[i] = pWeek[i];
	}
}

void  CSevenGridsUI::SetFont(int nFont) {
	m_nFont = nFont;
}

void  CSevenGridsUI::SetValues(int nIndex, const char * szValue) {
	if (nIndex >= 0 && nIndex < 7) {
		if (szValue)
			m_edits[nIndex]->SetText(szValue);
		else
			m_edits[nIndex]->SetText("");
	}
}

CDuiString  CSevenGridsUI::GetValues(int nIndex) {
	if (nIndex >= 0 && nIndex < 7) {
		return m_edits[nIndex]->GetText();
	}
	return "";
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

	CDuiString strText;
	for (int i = 0; i < 7; i++) {
		strText.Format("edt_%d", i + 1);
		m_edits[i] = static_cast<DuiLib::CEditUI*>(m_pManager->FindControl(strText));		
		m_edits[i]->SetFont(m_nFont);

		if (!m_bNumberOnly) {
			m_edits[i]->SetNumberOnly(false);
		}

		strText.Format("lbl_%d", i + 1);
		m_labels[i] = static_cast<DuiLib::CLabelUI*>(m_pManager->FindControl(strText));
		m_labels[i]->SetText(m_week_days[i]); 
	}
}

void   CSevenGridsUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue); 
}



CShiftUI::CShiftUI() {

}

CShiftUI::~CShiftUI() {

}

LPCTSTR CShiftUI::GetClass() const {
	return "Shift";
}

void CShiftUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(_T("Shift.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}

	CDuiString strText;
	for (int i = 0; i < 2; i++) {
		strText.Format("edt_%d", i + 1);
		m_edits[i] = static_cast<DuiLib::CEditUI*>(m_pManager->FindControl(strText));
	}
}


CMyDateUI::CMyDateUI() {
	
}

CMyDateUI::~CMyDateUI() {
	m_pManager->RemoveNotifier(this);
}

LPCTSTR CMyDateUI::GetClass() const {
	return "MyDate";
}

void CMyDateUI::Notify(TNotifyUI& msg) {
	if (msg.sType == "killfocus") {
		if (msg.pSender == this) {
			char szDate[256];
			CDuiString strText;
			SYSTEMTIME s = GetTime();
			if (IsToday(s)) {
				Date2String(szDate, sizeof(szDate), s);
				strText.Format("%s(今天)", szDate);
				SetText(strText);
			}
			else {
				Date2String(szDate, sizeof(szDate), s);
				strText.Format("%s(%s)", szDate, GetWeekDayShortName(s.wDayOfWeek));
				SetText(strText);				
			}
		}
	}
}

void CMyDateUI::SetMyTime(SYSTEMTIME* pst) {
	assert(pst);
	SetTime(pst);

	char szDate[256];
	CDuiString strText;
	if ( IsToday(*pst) ) {
		Date2String(szDate, sizeof(szDate), *pst);
		strText.Format("%s(今天)", szDate);
		SetText(strText);
	}
	else {
		Date2String(szDate, sizeof(szDate), *pst);
		strText.Format("%s(%s)", szDate, GetWeekDayShortName(pst->wDayOfWeek));
		SetText(strText);
	}
}

void CMyDateUI::DoInit() {
	CDateTimeUI::DoInit();
	m_pManager->AddNotifier(this);

	SYSTEMTIME s = GetTime();
	char szDate[256];
	CDuiString strText;
	if (IsToday(s)) {
		Date2String(szDate, sizeof(szDate), s);
		strText.Format("%s(今天)", szDate);
		SetText(strText);
	}
	else {
		Date2String(szDate, sizeof(szDate), s);
		strText.Format("%s(%s)", szDate, GetWeekDayShortName(s.wDayOfWeek));
		SetText(szDate);
	}
}


CMyEventUI::CMyEventUI() : m_callback(m_pManager) {

}

CMyEventUI::~CMyEventUI() {

}

LPCTSTR CMyEventUI::GetClass() const {
	return "MyEvent";
}

void CMyEventUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(_T("Event.xml"), (UINT)0, &m_callback, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}
}

void CMyEventUI::Notify(TNotifyUI& msg) {

}