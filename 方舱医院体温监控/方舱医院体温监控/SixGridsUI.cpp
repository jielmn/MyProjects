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
		if (m_labels[i]) {
			m_labels[i]->SetText(m_week_days[i]);
		}
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

void  CShiftUI::SetValues(int nIndex, const char * szValue) {
	if (nIndex >= 0 && nIndex < 2) {
		if (szValue)
			m_edits[nIndex]->SetText(szValue);
		else
			m_edits[nIndex]->SetText("");
	}
}

CDuiString  CShiftUI::GetValues(int nIndex) {
	if (nIndex >= 0 && nIndex < 2) {
		return m_edits[nIndex]->GetText();
	}
	return "";
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
				strText.Format("%s(����)", szDate);
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
		strText.Format("%s(����)", szDate);
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
		strText.Format("%s(����)", szDate);
		SetText(strText);
	}
	else {
		Date2String(szDate, sizeof(szDate), s);
		strText.Format("%s(%s)", szDate, GetWeekDayShortName(s.wDayOfWeek));
		SetText(szDate);
	}
}


CMyDateTimeUI::CMyDateTimeUI() : m_callback(m_pManager) {
	m_date = 0;
	m_time = 0;
	m_hour = 0;
	m_minute = 0;
}

CMyDateTimeUI::~CMyDateTimeUI() {

}

LPCTSTR CMyDateTimeUI::GetClass() const {
	return "MyDateTime";
}

void CMyDateTimeUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(_T("MyTime.xml"), (UINT)0, &m_callback, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}	

	m_date = static_cast<CMyDateUI*>(m_pManager->FindControl("date_1"));
	m_hour = static_cast<CEditUI*>(m_pManager->FindControl("edt_1"));
	m_minute = static_cast<CEditUI*>(m_pManager->FindControl("edt_2"));

	if (m_time > 0) {
		SYSTEMTIME s = Time2SysTime(m_time);
		m_date->SetMyTime(&s);

		CDuiString strText;
		strText.Format("%d", (int)s.wHour);
		m_hour->SetText(strText);

		strText.Format("%d", (int)s.wMinute);
		m_minute->SetText(strText);
	}
}

void CMyDateTimeUI::SetTime(time_t t) {
	m_time = t;
	if (m_date > 0) {
		SYSTEMTIME s = Time2SysTime(t);
		m_date->SetMyTime(&s);

		CDuiString strText;
		strText.Format("%d", (int)s.wHour);
		m_hour->SetText(strText);

		strText.Format("%d", (int)s.wMinute);
		m_minute->SetText(strText);
	}
}

time_t CMyDateTimeUI::GetTime() {
	SYSTEMTIME s = m_date->GetTime();
	int nHour = GetIntFromDb(m_hour->GetText());
	int nMinute = GetIntFromDb(m_minute->GetText());

	if (nHour < 0 || nHour >= 24)
		nHour = 0;

	if (nMinute < 0 || nMinute >= 60)
		nMinute = 0;

	time_t t = GetAnyDayZeroTime(SysTime2Time(s)) + 3600 * nHour + 60 * nMinute;
	return t;
}


CMyEventUI::CMyEventUI() : m_callback(m_pManager) {
	m_cmbType = 0;
	m_date_1 = 0;
	m_edt_1 = 0;
	m_edt_2 = 0;
	m_date_2 = 0;
	m_edt_3 = 0;
	m_edt_4 = 0;
	m_lay_2 = 0;
	m_sel = 0;
	m_lay_1 = 0;
	m_bSelected = FALSE;
	m_nType = 0;
	m_time1 = 0;
	m_time2 = 0;
}

CMyEventUI::~CMyEventUI() {
	m_pManager->RemoveNotifier(this);
}

LPCTSTR CMyEventUI::GetClass() const {
	return "MyEvent";
}

void CMyEventUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(_T("Event.xml"), (UINT)0, &m_callback, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}
	this->SetFixedHeight(30);

	m_cmbType = static_cast<CComboUI*>(m_pManager->FindControl("cmbType"));
	m_date_1 = static_cast<CDateTimeUI*>(m_pManager->FindControl("date_1"));
	m_edt_1 = static_cast<CEditUI*>(m_pManager->FindControl("edt_1"));
	m_edt_2 = static_cast<CEditUI*>(m_pManager->FindControl("edt_2"));
	m_date_2 = static_cast<CDateTimeUI*>(m_pManager->FindControl("date_2"));
	m_edt_3 = static_cast<CEditUI*>(m_pManager->FindControl("edt_3"));
	m_edt_4 = static_cast<CEditUI*>(m_pManager->FindControl("edt_4"));
	m_lay_2 = static_cast<CHorizontalLayoutUI*>(m_pManager->FindControl("lay_2"));
	m_sel = m_pManager->FindControl("sel");
	m_lay_1 = static_cast<CHorizontalLayoutUI*>(m_pManager->FindControl("lay_1"));
	m_lay_1->SetBorderSize(this->GetBorderSize());

	CListLabelElementUI * pItem = 0;
	CDuiString strText;

	pItem = new CListLabelElementUI;
	pItem->SetText("����");
	pItem->SetTag(PTYPE_SURGERY);
	m_cmbType->Add(pItem);

	pItem = new CListLabelElementUI;
	pItem->SetText("����");
	pItem->SetTag(PTYPE_BIRTH);
	m_cmbType->Add(pItem);

	pItem = new CListLabelElementUI;
	pItem->SetText("ת��");
	pItem->SetTag(PTYPE_TURN_IN);
	m_cmbType->Add(pItem);

	pItem = new CListLabelElementUI;
	pItem->SetText("ת��");
	pItem->SetTag(PTYPE_TURN_OUT);
	m_cmbType->Add(pItem);

	pItem = new CListLabelElementUI;
	pItem->SetText("��������ֹͣ");
	pItem->SetTag(PTYPE_DEATH);
	m_cmbType->Add(pItem);

	pItem = new CListLabelElementUI;
	pItem->SetText("���");
	pItem->SetTag(PTYPE_HOLIDAY);
	m_cmbType->Add(pItem);

	if (m_nType == PTYPE_SURGERY) {
		m_cmbType->SelectItem(0);
	}
	else if (m_nType == PTYPE_BIRTH) {
		m_cmbType->SelectItem(1);
	}
	else if (m_nType == PTYPE_TURN_IN) {
		m_cmbType->SelectItem(2);
	}
	else if (m_nType == PTYPE_TURN_OUT) {
		m_cmbType->SelectItem(3);
	}
	else if (m_nType == PTYPE_DEATH) {
		m_cmbType->SelectItem(4);
	}
	else if (m_nType == PTYPE_HOLIDAY) {
		m_cmbType->SelectItem(5);
	}
	else {
		m_cmbType->SelectItem(0);
	}

	if (m_nType == PTYPE_HOLIDAY) {
		m_lay_2->SetVisible(true);
	}
	else {
		m_lay_2->SetVisible(false);
	}

	if (m_time1 > 0) {
		char szTime[256];
		SYSTEMTIME s = Time2SysTime(m_time1);
		m_date_1->SetTime(&s);
		Date2String(szTime, sizeof(szTime), &m_time1);
		m_date_1->SetText(szTime);

		struct tm  tmp;
		localtime_s(&tmp, &m_time1);
		strText.Format("%d", tmp.tm_hour);
		m_edt_1->SetText(strText);
		strText.Format("%d", tmp.tm_min);
		m_edt_2->SetText(strText);
	}

	if (m_time2 > 0) {
		char szTime[256];
		SYSTEMTIME s = Time2SysTime(m_time2);
		m_date_2->SetTime(&s);
		Date2String(szTime, sizeof(szTime), &m_time2);
		m_date_2->SetText(szTime);

		struct tm  tmp;
		localtime_s(&tmp, &m_time2);
		strText.Format("%d", tmp.tm_hour);
		m_edt_3->SetText(strText);
		strText.Format("%d", tmp.tm_min);
		m_edt_4->SetText(strText);
	}

	if (m_bSelected)
		m_sel->SetVisible(true);
	else
		m_sel->SetVisible(false); 

	m_pManager->AddNotifier(this);
}

void CMyEventUI::Notify(TNotifyUI& msg) {
	if (msg.sType == "itemselect") {
		if ( msg.pSender == m_cmbType ) {
			int nSel = m_cmbType->GetCurSel();
			int nTag = m_cmbType->GetItemAt(nSel)->GetTag();
			if (nTag == PTYPE_HOLIDAY) {
				m_lay_2->SetVisible(true);
			}
			else {
				m_lay_2->SetVisible(false);
			}
		}
	}
	else if (msg.sType == "setfocus") {
		if (msg.pSender == m_cmbType || msg.pSender == m_edt_1 || msg.pSender == m_edt_2
			|| msg.pSender == m_edt_3 || msg.pSender == m_edt_4 || msg.pSender == m_date_1
			|| msg.pSender == m_date_2 ) {
			//����UIManager�����Ϣ��Ҫ����
			m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
		}
	}
}

void CMyEventUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_BUTTONDOWN) {
		//����UIManager�����Ϣ��Ҫ����
		m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
	}
	CContainerUI::DoEvent(event);
}

void CMyEventUI::SetSelected(BOOL bSel) {
	m_bSelected = bSel;
	if (m_sel) {
		if (bSel)
			m_sel->SetVisible(true);
		else
			m_sel->SetVisible(false);
	}		
}

void CMyEventUI::GetValue(int & nDbId, int & nType, time_t & t1, time_t & t2) {
	int nSel = m_cmbType->GetCurSel();
	CControlUI * pCtl = m_cmbType->GetItemAt(nSel);
	if (pCtl)
		nType = m_cmbType->GetItemAt(nSel)->GetTag();
	else
		nType = PTYPE_SURGERY;

	SYSTEMTIME s1 = m_date_1->GetTime();
	SYSTEMTIME s2 = m_date_1->GetTime();
	int nHour1   = GetIntFromDb(m_edt_1->GetText());
	int nMinute1 = GetIntFromDb(m_edt_2->GetText());
	int nHour2   = GetIntFromDb(m_edt_3->GetText());
	int nMinute2 = GetIntFromDb(m_edt_4->GetText());

	if (nHour1 < 0 || nHour1 >= 24)
		nHour1 = 0;

	if (nHour2 < 0 || nHour2 >= 24)
		nHour2 = 0;

	if (nMinute1 < 0 || nMinute1 >= 60)
		nMinute1 = 0;

	if (nMinute2 < 0 || nMinute2 >= 60)
		nMinute2 = 0;
	
	t1 = GetAnyDayZeroTime(SysTime2Time(s1)) + 3600 * nHour1 + 60 * nMinute1;
	t2 = GetAnyDayZeroTime(SysTime2Time(s2)) + 3600 * nHour2 + 60 * nMinute2;

	nDbId = GetTag();
}

void CMyEventUI::SetValue(int nDbId, int nType, time_t t1, time_t t2 /*= 0*/) {
	SetTag(nDbId);
	m_nType = nType;
	m_time1 = t1;
	m_time2 = t2;
	if (0 == m_cmbType)
		return;

	if (nType == PTYPE_SURGERY) {
		m_cmbType->SelectItem(0);
	}
	else if (nType == PTYPE_BIRTH) {
		m_cmbType->SelectItem(1);
	}
	else if (nType == PTYPE_TURN_IN) {
		m_cmbType->SelectItem(2);
	}
	else if (nType == PTYPE_TURN_OUT) {
		m_cmbType->SelectItem(3);
	}
	else if (nType == PTYPE_DEATH) {
		m_cmbType->SelectItem(4);
	}
	else if (nType == PTYPE_HOLIDAY) {
		m_cmbType->SelectItem(5);
	}
	else {
		m_cmbType->SelectItem(0);
	}

	if (nType == PTYPE_HOLIDAY) {
		m_lay_2->SetVisible(true);
	}
	else {
		m_lay_2->SetVisible(false);
	}

	SYSTEMTIME  s1 = Time2SysTime(t1);
	m_date_1->SetTime(&s1);

	char szTime[256];
	Date2String(szTime, sizeof(szTime), &t1);
	m_date_1->SetText(szTime);

	struct tm  tmp;
	localtime_s(&tmp, &t1);

	CDuiString  strText;
	strText.Format("%d", tmp.tm_hour);
	m_edt_1->SetText(strText);
	strText.Format("%d", tmp.tm_min);
	m_edt_2->SetText(strText);

	if (nType == PTYPE_HOLIDAY && t2 > 0) {
		SYSTEMTIME  s2 = Time2SysTime(t2);
		m_date_2->SetTime(&s2);

		Date2String(szTime, sizeof(szTime), &t2);
		m_date_2->SetText(szTime);

		struct tm  tmp;
		localtime_s(&tmp, &t2);

		CDuiString  strText;
		strText.Format("%d", tmp.tm_hour);
		m_edt_3->SetText(strText);
		strText.Format("%d", tmp.tm_min);
		m_edt_4->SetText(strText);
	}	
}



CTempUI::CTempUI() {
	m_btn = 0;
	m_temp1 = 0;
	m_temp2 = 0;
	m_t1 = 0;
    m_t2 = 0;
	m_nSel = 0;
}

CTempUI::~CTempUI() {
	m_pManager->RemoveNotifier(this);
}

LPCTSTR CTempUI::GetClass() const {
	return "TempUI";
}

void CTempUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(_T("TempUI.xml"), (UINT)0, 0, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
		m_pManager->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		return;
	} 

	m_btn = static_cast<CButtonUI*>(m_pManager->FindControl("btn_1"));
	m_temp1 = static_cast<CEditUI*>(m_pManager->FindControl("edt_1"));
	m_temp2 = static_cast<CEditUI*>(m_pManager->FindControl("edt_2")); 

	CDuiString  strText;
	if (m_t1 > 0) {
		strText.Format("%.2f", (float)m_t1 / 100.0f);
	}
	else {
		strText = "";
	}
	m_temp1->SetText(strText);

	if (m_t2 > 0) {
		strText.Format("%.2f", (float)m_t2 / 100.0f);
		m_temp2->SetVisible(true);
	}
	else {
		strText = "";
		m_temp2->SetVisible(false);
	}
	m_temp2->SetText(strText);
}

void CTempUI::Notify(TNotifyUI& msg) {
	if (msg.sType == "click") {
		if (msg.pSender == m_btn) {
			bool b = m_temp2->IsVisible();
			m_temp2->SetVisible(!b);

			if (b) {
				m_nSel = 0;
			}
			else {
				m_nSel = 1;
			}
			m_pManager->SendNotify(this, "tempui_setfocus");
		}
	}
	else if (msg.sType == "setfocus") {
		if ( msg.pSender == m_temp1) {
			m_nSel = 0;
			m_pManager->SendNotify(this, "tempui_setfocus");
		}
		else if (msg.pSender == m_temp2) {
			m_nSel = 1;
			m_pManager->SendNotify(this, "tempui_setfocus");
		}
	}
	else if (msg.sType == "killfocus") {
		if (msg.pSender == m_temp1 || msg.pSender == m_temp2) {
			//OutputDebugString("==== killfocus \n");
			m_pManager->SendNotify(this, "tempui_killfocus");
		}
	}
}

void CTempUI::GetValue(int & t1, int &t2) {
	CDuiString  strText;
	float  fValue = 0.0f;

	t1 = t2 = 0;

	strText = m_temp1->GetText();
	if ( 1 == sscanf_s(strText, "%f", &fValue) ) {
		t1 = (int)(fValue * 100);
	}

	if (!m_temp2->IsVisible())
		return;

	strText = m_temp2->GetText();
	if (1 == sscanf_s(strText, "%f", &fValue)) {
		t2 = (int)(fValue * 100);
	}
}

void CTempUI::SetValue(int t1, int t2 /*= 0*/) {
	m_t1 = t1;
	m_t2 = t2;

	if (0 == m_temp1)
		return;

	CDuiString  strText;
	if (m_t1 > 0) {
		strText.Format("%.2f", (float)m_t1 /100.0f );
	}
	else {
		strText = "";
	}
	m_temp1->SetText(strText);

	if (m_t2 > 0) {
		strText.Format("%.2f", (float)m_t2 / 100.0f);
		m_temp2->SetVisible(true);
	}
	else {
		strText = "";
		m_temp2->SetVisible(false);
	}
	m_temp2->SetText(strText);
}

int CTempUI::GetSel() {
	return m_nSel;
}

void CTempUI::SetFocusValue(int t) {
	assert(m_nSel >= 0 && m_nSel <= 1);

	if (m_nSel == 0) {
		m_t1 = t;
	}
	else {
		m_t2 = t;
	}

	if (0 == m_temp1)
		return;

	CDuiString  strText;
	if (t > 0) {
		strText.Format("%.2f", (float)t / 100.0f);
	}
	else {
		strText = "";
	}

	if (m_nSel == 0) {		
		m_temp1->SetText(strText);
	}
	else {
		m_temp2->SetText(strText);
	}
}



CSixTempUI::CSixTempUI() : m_callback(m_pManager) {
	memset(m_values, 0, sizeof(m_values));
	memset(m_temp, 0, sizeof(m_temp));
}

CSixTempUI::~CSixTempUI() {

}

LPCTSTR CSixTempUI::GetClass() const {
	return "SixTempUI";
}

void CSixTempUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(_T("SixTempUI.xml"), (UINT)0, &m_callback, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}

	CDuiString  strText;
	for (int i = 0; i < 6; i++) {
		strText.Format("temp_%d", i + 1);
		m_temp[i] = static_cast<CTempUI*>(m_pManager->FindControl(strText));
		m_temp[i]->SetTag(i);
		m_temp[i]->SetValue(m_values[i][0], m_values[i][1]);
	}
}

void  CSixTempUI::SetValues(int nIndex, int t1, int t2 /*= 0*/) {
	if (nIndex >= 0 && nIndex < 6) {
		m_values[nIndex][0] = t1;
		m_values[nIndex][1] = t2;

		if (0 == m_temp[nIndex])
			return;

		m_temp[nIndex]->SetValue(t1, t2);
	}
}

void  CSixTempUI::GetValues(int nIndex, int & t1, int & t2) {
	if (nIndex >= 0 && nIndex < 6) {
		m_temp[nIndex]->GetValue(t1, t2);
	}
}



CPatientImg::CPatientImg(HWND hWnd) : m_hWnd(hWnd) {
	m_pVec = 0;
	m_tStart = 0;
	m_tEnd = 0;
	m_fSecondsPerPixel = 0.0f;

	m_hCommonThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x66, 0x66, 0x66));
	m_hBrighterThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x99, 0x99, 0x99));
	m_hCommonBrush = ::CreateSolidBrush(RGB(0x43, 0x42, 0x48));

	m_temperature_pen   = new Pen(Gdiplus::Color( 0xFF00FF00 ), 1.0);
	m_temperature_brush = new SolidBrush(Gdiplus::Color(0xFF00FF00));
	m_bSetSecondsPerPixel = FALSE;
}

CPatientImg::~CPatientImg() {
	DeleteObject(m_hCommonThreadPen);
	DeleteObject(m_hBrighterThreadPen);
	DeleteObject(m_hCommonBrush);
	delete m_temperature_pen;
	delete m_temperature_brush;
}

bool CPatientImg::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);
	CDuiString  strText;

	// GDI+
	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	CContainerUI * pParent = (CContainerUI *)GetParent();

	// self rectangle and width, height
	RECT rect   = GetPos();
	int  width  = pParent->GetFixedWidth();
	int  height = rect.bottom - rect.top;

	// ˮƽ������λ��
	int  nScrollX = pParent->GetScrollPos().cx;

	// ��ߣ�����¶�
	int  nMinTemp, nMaxTemp;
	GetMaxMinShowTemp(nMinTemp, nMaxTemp);

	// ���϶ȸ���
	int  nCelsiusCount = nMaxTemp - nMinTemp;
	// ÿ�����϶ȵĸ߶�
	int  nHeightPerCelsius = GetCelsiusHeight(height, nCelsiusCount);
	// ��ֱ����
	int  nVMargin = (height - nHeightPerCelsius * nCelsiusCount) / 2;
	// ����¶ȵ�Y����ϵֵ
	int  nMaxY = rect.top + nVMargin;

	// ȫͼ��Ϊ��߿̶�������ұ�����ͼ
	RECT rectScale;
	rectScale.left = rect.left + nScrollX;
	rectScale.top = rect.top;
	rectScale.right = rectScale.left + SCALE_RECT_WIDTH;
	rectScale.bottom = rect.bottom;  

	// ��ˮƽ�̶���
	DrawScaleLine(hDC, nCelsiusCount, nHeightPerCelsius, nMaxY, rectScale, rect);

	// ���߿�
	DrawBorder(hDC, rectScale, width);

	// �鿴��������
	int nPointsCnt = GetTempCount();
	// ���û�����ݾͲ��ػ��� 
	if (nPointsCnt > 0) {
		// ���¶�����
		POINT  top_left;
		top_left.x = rect.left + SCALE_RECT_WIDTH;
		top_left.y = nMaxY;

		DrawPolyline(m_tStart, m_tEnd, m_fSecondsPerPixel, nMaxTemp, nHeightPerCelsius,
			top_left, graphics, TRUE, m_temperature_pen, m_temperature_brush );

		// ��Ч����
		RECT rValid;
		rValid.left = rectScale.right;
		rValid.right = rectScale.left + width - 1;
		rValid.top = rectScale.top;
		rValid.bottom = rectScale.bottom;

		top_left.y += nHeightPerCelsius * nCelsiusCount;
		// ��ʱ���ı�
		DrawTimeText(hDC, m_tStart, m_tEnd, m_fSecondsPerPixel, top_left, rValid);

		// ��ע��
		top_left.y = nMaxY;

		// ���λ��
		POINT cursor_point;
		GetCursorPos(&cursor_point);
		::ScreenToClient(m_hWnd, &cursor_point);

		// ��ʮ����
		DrawCrossLine(hDC, rValid, cursor_point, m_tStart, m_fSecondsPerPixel, nMaxTemp, nHeightPerCelsius, top_left);
		
	}
	else {
		// ���¶�����
		POINT  top_left;
		top_left.x = rect.left + SCALE_RECT_WIDTH;
		top_left.y = nMaxY;

		// ��Ч����
		RECT rValid;
		rValid.left = rectScale.right;
		rValid.right = rectScale.left + width - 1;
		rValid.top = rectScale.top;
		rValid.bottom = rectScale.bottom;

		top_left.y += nHeightPerCelsius * nCelsiusCount;
		// ��ʱ���ı�
		DrawTimeText(hDC, m_tStart, m_tEnd, m_fSecondsPerPixel, top_left, rValid);
	}

	// ���̶�ֵ
	DrawScale(hDC, nCelsiusCount, nHeightPerCelsius, nMaxY, nMaxTemp, rectScale, width);

	char szDate[256];
	Date2String(szDate, sizeof(szDate), &m_tStart);
	int nWeekDayIndex = GetWeekDay(m_tStart);
	strText.Format("%s %s", szDate, GetWeekDayName(nWeekDayIndex));
	RECT tmpRect = { rectScale.left, rectScale.top, rectScale.right + width - 1, rectScale.top + nVMargin };
	::DrawText(hDC, strText, strText.GetLength(), &tmpRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	 
	return true;
}

void CPatientImg::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_MOUSEMOVE) {
		//::OutputDebugString("== on mouse move! \n");
		this->Invalidate();
	}
	else if (event.Type == UIEVENT_DBLCLICK) {
		m_pManager->SendNotify(this, "get_temp", GetDbClickTemp() );
	}
	CControlUI::DoEvent(event);
}

LPCTSTR CPatientImg::GetClass() const {
	return "PatientImage";
}

void  CPatientImg::GetMaxMinShowTemp(int & nMinTemp, int & nMaxTemp) {
	nMinTemp = 3500;
	nMaxTemp = 3900;

	if ( m_pVec == 0 ) {
		nMinTemp = 35;
		nMinTemp = 39;
		return;
	}

	const std::vector<TempItem * > & v = *m_pVec;
	std::vector<TempItem * >::const_iterator it;
	BOOL bFirst = TRUE;

	for (it = v.begin(); it != v.end(); ++it) {
		TempItem * pItem = *it;
		if ( pItem->m_time < m_tStart )
			continue;
		if (pItem->m_time >= m_tEnd)
			break;
		if (bFirst) {
			nMaxTemp = pItem->m_dwTemp;
			nMinTemp = pItem->m_dwTemp;
			bFirst = FALSE;
		}
		else {
			if ((int)pItem->m_dwTemp > nMaxTemp) {
				nMaxTemp = pItem->m_dwTemp;
			}
			else if ((int)pItem->m_dwTemp < nMinTemp) {
				nMinTemp = pItem->m_dwTemp;
			}
		}
	}

	// ������С��ʾ�¶ȱ���Ϊ����
	// min: 2100~2199 ==> 2100
	// max: 3800 ==> 3800, 3801~3899 ==> 3900
	nMinTemp = nMinTemp / 100;
	int nReminder = nMaxTemp % 100;
	if (0 != nReminder) {
		nMaxTemp = (nMaxTemp / 100 + 1);
	}
	else {
		nMaxTemp /= 100;
	}

	if (nMinTemp == nMaxTemp) {
		nMaxTemp = nMinTemp + 1;
	}
}

int   CPatientImg::GetCelsiusHeight(int height, int nCelsiusCount, int nVMargin /*= MIN_MYIMAGE_VMARGIN*/) {
	int h = height / nCelsiusCount;
	int r = height % nCelsiusCount;
	// int nVMargin = MIN_MYIMAGE_VMARGIN;

	if (nVMargin * 2 > r) {
		int nSpared = (nVMargin * 2 - r - 1) / nCelsiusCount + 1;
		h -= nSpared;
	}
	return h;
}

// ��ˮƽ�̶���
void   CPatientImg::DrawScaleLine(HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY,
	const RECT & rectScale, const RECT & rect) {
	int nVInterval = BRIGHT_DARK_INTERVAL;
	for (int i = 0; i < nCelsiusCnt + 1; i++) {
		if (nVInterval >= BRIGHT_DARK_INTERVAL) {
			::SelectObject(hDC, m_hBrighterThreadPen);
			nVInterval = nHeightPerCelsius;
		}
		else {
			::SelectObject(hDC, m_hCommonThreadPen);
			nVInterval += nHeightPerCelsius;
		}
		int  nTop = nMaxY + i * nHeightPerCelsius;
		::MoveToEx(hDC, rectScale.right, nTop, 0);
		::LineTo(hDC, rect.right, nTop);
	}
}

// ���߿�
void   CPatientImg::DrawBorder(HDC hDC, const RECT & rectScale, int width) {

	::MoveToEx(hDC, rectScale.left, rectScale.top, 0);
	::LineTo(hDC, rectScale.left, rectScale.bottom);

	int nX = rectScale.left + width - 1;
	::MoveToEx(hDC, nX, rectScale.top, 0);
	::LineTo(hDC, nX, rectScale.bottom);
}

// ���̶�ֵ
void  CPatientImg::DrawScale(HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY, int nMaxTemp,
	const RECT & rectScale, int width, BOOL bDrawRectangle /*= TRUE*/,
	DWORD dwTextColor /*= RGB(255, 255, 255)*/) {
	CDuiString strText;
	int nVInterval = BRIGHT_DARK_INTERVAL;
	::SetTextColor(hDC, dwTextColor);
	::SelectObject(hDC, m_hCommonThreadPen);

	if (bDrawRectangle)
		::Rectangle(hDC, rectScale.left, rectScale.top, rectScale.right, rectScale.bottom);

	RECT r = rectScale;
	r.left++;
	r.right--;
	r.top++;
	r.bottom--;
	if (bDrawRectangle)
		::FillRect(hDC, &r, m_hCommonBrush);

	for (int i = 0; i < nCelsiusCnt + 1; i++) {
		if (nVInterval >= BRIGHT_DARK_INTERVAL) {
			int  nTop = nMaxY + i * nHeightPerCelsius;
			int  nTemperature = nMaxTemp - i;
			strText.Format("%d��", nTemperature);
			::TextOut(hDC, rectScale.right + (-40), nTop + (-8),
				strText, strText.GetLength());
			nVInterval = nHeightPerCelsius;
		}
		else {
			nVInterval += nHeightPerCelsius;
		}
	}
}

// �¶����ݸ���
DWORD  CPatientImg::GetTempCount() {
	if (m_pVec == 0)
		return 0;
	if (m_tStart == 0)
		return 0;
	if (m_tEnd <= m_tStart)
		return 0;

	const std::vector<TempItem * > & v = *m_pVec;
	std::vector<TempItem * >::const_iterator it;
	DWORD  dwCnt = 0;

	for (it = v.begin(); it != v.end(); it++) {
		TempItem * pItem = *it;
		if ( pItem->m_time < m_tStart )
			continue;
		if (pItem->m_time >= m_tEnd)
			break;
		dwCnt++;
	}

	return dwCnt;
}

void   CPatientImg::DrawPolyline(time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel,
	int    nMaxTemp, int nHeightPerCelsius, POINT  tTopLeft, Graphics & graphics,
	BOOL  bDrawPoints, Pen * pen, SolidBrush * brush) {

	if (m_pVec == 0)
		return;
	if (m_tStart == 0)
		return;
	if (m_tEnd <= m_tStart)
		return;

	const std::vector<TempItem * > & vTempData = *m_pVec;

	DWORD dwMaxTempCnt = vTempData.size();
	Gdiplus::Point * points = new Gdiplus::Point[dwMaxTempCnt];
	vector<TempItem *>::const_iterator it;

	// �ҵ���һ����
	for (it = vTempData.begin(); it != vTempData.end(); it++) {
		TempItem * pItem = *it;
		if (pItem->m_time >= tFirstTime) {
			break;
		}
	}

	// �ҵ���һ����������ĵ㲻�ں���ʼʱ���Ƚ�
	// ��ʱ�洢��ͬx�����vector
	vector<TempItem *>  vTmp;
	// ��ʱvector��item�й�ͬ�ĵ㣬���ǵ�x������ͬ
	int nTmpX = 0;
	// points����Ĵ�С
	int cnt = 0;

	for (; it != vTempData.end(); it++) {
		TempItem * pItem = *it;
		// ������ʱ����ֵ
		if (tLastTime > 0) {
			// ���������Χ
			if (pItem->m_time > tLastTime) {
				break;
			}
		}

		int  nX = (int)((pItem->m_time - tFirstTime) / fSecondsPerPixel);

		if (vTmp.size() == 0) {
			vTmp.push_back(pItem);
			nTmpX = nX;
		}
		else {
			// ���ƫ�������ϴε���ͬ����������ʱvector��
			if (nX <= nTmpX + 1) {
				vTmp.push_back(pItem);
			}
			else {
				vector<TempItem *>::iterator  it_tmp;
				int  sum = 0;
				for (it_tmp = vTmp.begin(); it_tmp != vTmp.end(); ++it_tmp) {
					TempItem * pTmpItem = *it_tmp;
					sum += pTmpItem->m_dwTemp;
				}
				// ��ƽ��ֵ
				int  ave = sum / vTmp.size();

				points[cnt].X = nTmpX + tTopLeft.x;
				points[cnt].Y = tTopLeft.y + (int)((nMaxTemp - ave / 100.0) * nHeightPerCelsius);
				cnt++;

				vTmp.clear();
				vTmp.push_back(pItem);
				nTmpX = nX;
			}
		}
	}

	if (vTmp.size() > 0) {
		vector<TempItem *>::iterator  it_tmp;
		int  sum = 0;
		for (it_tmp = vTmp.begin(); it_tmp != vTmp.end(); ++it_tmp) {
			TempItem * pTmpItem = *it_tmp;
			sum += pTmpItem->m_dwTemp;
		}
		// ��ƽ��ֵ
		int  ave = sum / vTmp.size();

		points[cnt].X = nTmpX + tTopLeft.x;
		points[cnt].Y = tTopLeft.y + (int)((nMaxTemp - ave / 100.0) * nHeightPerCelsius);
		cnt++;

		vTmp.clear();
	}

	//if (bDrawPoints && fSecondsPerPixel > 0.0f && fSecondsPerPixel < 6.0f) {
		for (int m = 0; m < cnt; ++m) {
			DrawPoint(brush, graphics, points[m].X, points[m].Y, 0, 3);
		}
	//}

	if (cnt > 1)
		graphics.DrawLines(pen, points, cnt);
	else if (1 == cnt)
		DrawPoint(brush, graphics, points[0].X, points[0].Y, 0, 3);

	delete[] points;
}

void  CPatientImg::DrawPoint(SolidBrush * brush, Graphics & g, int x, int y, HDC hDc, int radius) {
	g.FillEllipse(brush, x - radius, y - radius, 2 * radius, 2 * radius);
}

void CPatientImg::MyInvalidate(BOOL bReset /*= TRUE*/, int nWidth /*= 0*/) {
	if (m_pVec == 0)
		return;
	if (m_tStart == 0)
		return;
	if (m_tEnd <= m_tStart)
		return;

	CContainerUI * pParent = (CContainerUI *)GetParent();
	int  width = pParent->GetFixedWidth();
	if (0 == width)
		width = nWidth;

	m_bSetSecondsPerPixel = !bReset;

	if (!m_bSetSecondsPerPixel) {
		m_fSecondsPerPixel = (float)(m_tEnd - m_tStart) / (width - SCALE_RECT_WIDTH);
		if (m_fSecondsPerPixel < 1.0f) {
			m_fSecondsPerPixel = 1.0f;
		}
		else if (m_fSecondsPerPixel > 200.0f) {
			m_fSecondsPerPixel = 200.0f;
		}
		m_bSetSecondsPerPixel = TRUE;
	}
	else {
		width = (int)((m_tEnd - m_tStart) / m_fSecondsPerPixel) + SCALE_RECT_WIDTH;
	}

	this->SetMinWidth(width);
	Invalidate();
}


// ��ʱ���ı�
void    CPatientImg::DrawTimeText(HDC hDC, time_t  tFirstTime, time_t tLastTime,
	float fSecondsPerPixel, POINT  top_left, const RECT & rValid) {
	char szTime[256];
	::SetTextColor(hDC, RGB(255, 255, 255));
	const int OFFSET_Y = 5;
	const int TIME_TEXT_INTERVAL = 80;
	const int TIME_TEXT_WIDTH = 60;
	BOOL  bFirst = TRUE;

	int nDivide = 3600;
	int  remainder = tFirstTime % nDivide;
	time_t  time_point = tFirstTime + (nDivide - remainder);
	int  time_width = (int)((time_point - tFirstTime) / fSecondsPerPixel);
	// �������㹻�󣬿�ʼ��text
	if (time_width >= TIME_TEXT_INTERVAL / 2) {
		Time2String(szTime, sizeof(szTime), &time_point);
		//::TextOut(hDC, top_left.x + time_width - TIME_TEXT_WIDTH / 2, top_left.y + OFFSET_Y, szTime, strlen(szTime));
		::ExtTextOut(hDC, top_left.x + time_width - TIME_TEXT_WIDTH / 2, top_left.y + OFFSET_Y, ETO_CLIPPED, &rValid, szTime, strlen(szTime), 0);
		bFirst = FALSE;
	}


	int last_time_width = time_width;
	time_point += nDivide;
	while (time_point <= tLastTime) {
		time_width = (int)((time_point - tFirstTime) / fSecondsPerPixel);
		if (bFirst) {
			// �������㹻�󣬿�ʼ��text
			if ((time_width - last_time_width) >= TIME_TEXT_INTERVAL / 2) {
				Time2String(szTime, sizeof(szTime), &time_point);
				//::TextOut(hDC, top_left.x + time_width - TIME_TEXT_WIDTH / 2, top_left.y + OFFSET_Y, szTime, strlen(szTime));
				::ExtTextOut(hDC, top_left.x + time_width - TIME_TEXT_WIDTH / 2, top_left.y + OFFSET_Y, ETO_CLIPPED, &rValid, szTime, strlen(szTime), 0);
				last_time_width = time_width;
				bFirst = FALSE;
			}
		}
		else {
			// �������㹻�󣬿�ʼ��text
			if ((time_width - last_time_width) >= TIME_TEXT_INTERVAL) {
				Time2String(szTime, sizeof(szTime), &time_point);
				//::TextOut(hDC, top_left.x + time_width - TIME_TEXT_WIDTH / 2, top_left.y + OFFSET_Y, szTime, strlen(szTime));
				::ExtTextOut(hDC, top_left.x + time_width - TIME_TEXT_WIDTH / 2, top_left.y + OFFSET_Y, ETO_CLIPPED, &rValid, szTime, strlen(szTime), 0);
				last_time_width = time_width;
			}
		}


		time_point += nDivide;
	}
}

// ��ʮ����
void   CPatientImg::DrawCrossLine(HDC hDC, const RECT & rValid, const POINT & cursor_point,
	time_t tFirstTime, float fSecondsPerPixel, int nMaxTemp, int nHeightPerCelsius,
	POINT  top_left) {
	CDuiString strText;

	// ������㻭ʮ���ߵ�Ҫ��
	if ( ::PtInRect(&rValid, cursor_point) ) {

		::SelectObject(hDC, m_hCommonThreadPen);
		::MoveToEx(hDC, cursor_point.x, rValid.top, 0);
		::LineTo(hDC, cursor_point.x, rValid.bottom);
		::MoveToEx(hDC, rValid.left, cursor_point.y, 0);
		::LineTo(hDC, rValid.right, cursor_point.y);

		float  cursot_temp = nMaxTemp + (float)(top_left.y - cursor_point.y) / nHeightPerCelsius;
		time_t cursor_time = (time_t)((float)(cursor_point.x - top_left.x) * fSecondsPerPixel) + tFirstTime;

		char szTime[256];
		Time2String(szTime, sizeof(szTime), &cursor_time);

		strText.Format("%.2f��,%s", cursot_temp, szTime);

		// �ж�������ߣ������ұ�
		if (rValid.right - cursor_point.x >= 120)
			::TextOut(hDC, cursor_point.x + 5, cursor_point.y - 20, strText, strText.GetLength());
		else
			::TextOut(hDC, cursor_point.x - 120, cursor_point.y - 20, strText, strText.GetLength());
	}
}

int CPatientImg::GetDbClickTemp() {

	// �鿴��������
	int nPointsCnt = GetTempCount();
	if (0 == nPointsCnt) {
		return 0;
	}

	CContainerUI * pParent = (CContainerUI *)GetParent();
	// self rectangle and width, height
	RECT rect = GetPos();
	int  width = pParent->GetFixedWidth();
	int  height = rect.bottom - rect.top;

	// ˮƽ������λ��
	int  nScrollX = pParent->GetScrollPos().cx;

	// ��ߣ�����¶�
	int  nMinTemp, nMaxTemp;
	GetMaxMinShowTemp(nMinTemp, nMaxTemp);

	// ���϶ȸ���
	int  nCelsiusCount = nMaxTemp - nMinTemp;
	// ÿ�����϶ȵĸ߶�
	int  nHeightPerCelsius = GetCelsiusHeight(height, nCelsiusCount);
	// ��ֱ����
	int  nVMargin = (height - nHeightPerCelsius * nCelsiusCount) / 2;
	// ����¶ȵ�Y����ϵֵ
	int  nMaxY = rect.top + nVMargin;

	// ȫͼ��Ϊ��߿̶�������ұ�����ͼ
	RECT rectScale;
	rectScale.left = rect.left + nScrollX;
	rectScale.top = rect.top;
	rectScale.right = rectScale.left + SCALE_RECT_WIDTH;
	rectScale.bottom = rect.bottom;

	// ���¶�����
	POINT  top_left;
	top_left.x = rect.left + SCALE_RECT_WIDTH;
	top_left.y = nMaxY;

	// ���λ��
	POINT cursor_point;
	GetCursorPos(&cursor_point);
	::ScreenToClient(m_hWnd, &cursor_point);
	
	float  cursot_temp = nMaxTemp + (float)(top_left.y - cursor_point.y) / nHeightPerCelsius;
	return (int)(cursot_temp * 100.0f);
}

CHumanUI::CHumanUI() {

}

CHumanUI::~CHumanUI() {

}

LPCTSTR  CHumanUI::GetClass() const {
	return "HumanUI";
}

void CHumanUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(_T("human.xml"), (UINT)0, 0, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}

	m_lblTemp[0] = static_cast<CLabelUI*>(m_pManager->FindControl("t1"));
	m_lblTemp[1] = static_cast<CLabelUI*>(m_pManager->FindControl("t2"));
	m_lblTemp[2] = static_cast<CLabelUI*>(m_pManager->FindControl("t3"));

	m_lblDeltaTemp[0] = static_cast<CLabelUI*>(m_pManager->FindControl("dt1"));
	m_lblDeltaTemp[1] = static_cast<CLabelUI*>(m_pManager->FindControl("dt2"));
	m_lblDeltaTemp[2] = static_cast<CLabelUI*>(m_pManager->FindControl("dt3"));

	this->SetFixedHeight(600); 
}

void  CHumanUI::SetTemp(int nIndex, CDuiString strText) {
	if (!m_bInitiated)
		return;

	if (nIndex < 0 || nIndex > 2)
		return;

	m_lblTemp[nIndex]->SetText(strText);
}

void  CHumanUI::SetTempColor(int nIndex, DWORD  dwColor) {
	if (!m_bInitiated)
		return;

	if (nIndex < 0 || nIndex > 2)
		return;

	m_lblTemp[nIndex]->SetTextColor(dwColor);
}

void  CHumanUI::SetDelta(int nIndex1, int nIndex2, BOOL bConnected1, BOOL bConnected2, int nTemp1, int nTemp2) {
	if (!m_bInitiated)
		return;

	if (nIndex1 < 0 || nIndex1 > 2)
		return;

	if (nIndex2 < 0 || nIndex2 > 2)
		return;

	if (nIndex1 == nIndex2)
		return;	

	// ���˳���Ǵ�С�򵽣�����
	if (nIndex1 > nIndex2) {
		int nIndex = 0;
		BOOL bConnected = FALSE;
		int nTemp = 0;

		bConnected = bConnected1;
		bConnected1 = bConnected2;
		bConnected2 = bConnected;

		nTemp = nTemp1;
		nTemp1 = nTemp2;
		nTemp2 = nTemp;

		nIndex = nIndex1;
		nIndex1 = nIndex2;
		nIndex2 = nIndex;
	}

	CDuiString strText;
	int nDelta = 0;

	if (0 == nIndex1) {
		if (nIndex2 == 1) {
			if ( !bConnected1 || !bConnected2 ) {
				m_lblDeltaTemp[0]->SetText("");
			}
			else {
				if (g_data.m_bDelta[0]) {
					nDelta = nTemp1 - nTemp2;
				}
				else {
					nDelta = nTemp2 - nTemp1;
				}
				strText.Format("%.2f", nDelta / 100.0f);
				m_lblDeltaTemp[0]->SetText(strText);
			}			
		}
		else if (nIndex2 == 2) {
			if (!bConnected1 || !bConnected2) {
				m_lblDeltaTemp[1]->SetText("");
			}
			else {
				if (g_data.m_bDelta[1]) {
					nDelta = nTemp1 - nTemp2;
				}
				else {
					nDelta = nTemp2 - nTemp1;
				}
				strText.Format("%.2f", nDelta / 100.0f);
				m_lblDeltaTemp[1]->SetText(strText);
			}
		}
	}
	else if (1 == nIndex1) {
		if (nIndex2 == 2) {
			if (!bConnected1 || !bConnected2) {
				m_lblDeltaTemp[2]->SetText("");
			}
			else {
				if (g_data.m_bDelta[2]) {
					nDelta = nTemp1 - nTemp2;
				}
				else {
					nDelta = nTemp2 - nTemp1;
				}
				strText.Format("%.2f", nDelta / 100.0f);
				m_lblDeltaTemp[2]->SetText(strText);
			}
		}
	}

}


CCubeItemUI::CCubeItemUI() {
	m_lblBedNo = 0;
	m_lblName = 0;
	m_lblPhone = 0;
	m_lblTemp = 0;
	m_lblTime = 0;
	m_lblBinding = 0;

	m_nBedNo = 0;
	m_nTemp = 0;
	m_time = 0;
	m_bBinding = FALSE;
}

CCubeItemUI::~CCubeItemUI() {

}

LPCTSTR CCubeItemUI::GetClass() const {
	return _T("CubeItem");
}

void CCubeItemUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(_T("CubeItem.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow); 
	}
	else {
		this->RemoveAll();
		return;
	}

	m_lblBedNo = (CLabelUI *)m_pManager->FindControl("lblBedNo");
	m_lblName  = (CLabelUI *)m_pManager->FindControl("lblName");
	m_lblPhone = (CLabelUI *)m_pManager->FindControl("lblPhone");
	m_lblTemp = (CLabelUI *)m_pManager->FindControl("lblTemp");
	m_lblTime = (CLabelUI *)m_pManager->FindControl("lblTime");
	m_lblBinding = (CLabelUI *)m_pManager->FindControl("lblBinding");

	SetBedNo();
	SetPatientName();
	SetPhone();
	SetTemperature();
	SetTime();
	SetBinding();

	this->SetFixedHeight(30); 
	this->SetFixedWidth(760);   
}

void CCubeItemUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_RBUTTONDOWN) {
		m_pManager->SendNotify(this, "cube_menu");
	}
	CContainerUI::DoEvent(event);
}

void  CCubeItemUI::SetBedNo(int nBedNo) {
	m_nBedNo = nBedNo;
	SetBedNo();
}

void  CCubeItemUI::SetBedNo() {
	CDuiString  strText;
	if (m_bInitiated) {
		strText.Format("%d", m_nBedNo);
		m_lblBedNo->SetText(strText);
	}
}

int   CCubeItemUI::GetBedNo() {
	return m_nBedNo;
}

void  CCubeItemUI::SetPatientName(const char * szName) {
	m_strName = szName;
	SetPatientName();
}

void CCubeItemUI::SetPatientName() {
	if (m_bInitiated) {
		m_lblName->SetText(m_strName);
	}
}

CDuiString  CCubeItemUI::GetPatientName() {
	return m_strName;
}

void  CCubeItemUI::SetPhone(const char * szPhone) {
	m_strPhone = szPhone;
	SetPhone();
}

void  CCubeItemUI::SetPhone() {
	if (m_bInitiated) {
		m_lblPhone->SetText(m_strPhone);
	}
}

CDuiString  CCubeItemUI::GetPhone() {
	return m_strPhone;
}

void  CCubeItemUI::SetTemperature(int nTemp) {
	m_nTemp = nTemp;
	SetTemperature();
}

int   CCubeItemUI::GetTemperature() {
	return m_nTemp;
}

void  CCubeItemUI::SetTemperature() {
	CDuiString  strText;
	if (m_bInitiated) {
		if (m_nTemp > 0) {
			strText.Format("%.1f", m_nTemp / 100.0);
			m_lblTemp->SetText(strText);
			if ( m_nTemp >= g_data.m_nCubeHighTemp ) {
				m_lblTemp->SetTextColor(HIGH_TEMP_TEXT_COLOR);
			}
			else {
				m_lblTemp->SetTextColor(0xFFFFFFFF);
			}
		}
		else {
			m_lblTemp->SetText("");
		}
	}
}

void  CCubeItemUI::SetTime(time_t t) {
	m_time = t;
	SetTime();
}

time_t   CCubeItemUI::GetTime() {
	return m_time;
}

void  CCubeItemUI::SetTime( ) {
	char szTime[256] = {0};
	if ( m_bInitiated ) {
		if (m_time > 0) {
			LmnFormatTime(szTime, sizeof(szTime), m_time, "%Y-%m-%d %H:%M:%S");
			m_lblTime->SetText(szTime);
		}
		else {
			m_lblTime->SetText("");
		}
	}
}

void  CCubeItemUI::SetBinding(BOOL bBinding) {
	m_bBinding = bBinding;
	SetBinding();
}

void  CCubeItemUI::SetBinding() {
	if ( m_bInitiated ) {
		if (m_bBinding) {
			m_lblBinding->SetText("�Ѱ�");
			m_lblBinding->SetTextColor(0xFFFFFFFF);
		}
		else {
			m_lblBinding->SetText("δ��");
			m_lblBinding->SetTextColor(CUBE_WARNING_COLOR);
		}
	}
}

BOOL  CCubeItemUI::GetBinding() {
	return m_bBinding;
}


CNewTagUI::CNewTagUI() {
	m_lblTemp = 0;
	m_lblTime = 0;

	m_nTemp = 0;
	m_time = 0;
}

CNewTagUI::~CNewTagUI() {

}

LPCTSTR  CNewTagUI::GetClass() const {
	return "NewTag";
}

void  CNewTagUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(_T("NewTag.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}

	m_lblTemp = (CLabelUI *)m_pManager->FindControl("lblTemp"); 
	m_lblTime = (CLabelUI *)m_pManager->FindControl("lblTime");

	SetTemperature();
	SetTime();
}

void  CNewTagUI::SetTemperature(int nTemp) {
	m_nTemp = nTemp;
	SetTemperature();
}

void  CNewTagUI::SetTemperature() {
	CDuiString  strText;
	if (m_bInitiated) {
		if (m_nTemp > 0) {
			strText.Format("%.1f��", m_nTemp / 100.0);
			m_lblTemp->SetText(strText);
		}
		else {
			m_lblTemp->SetText("δ����");
		}
	}
}

int   CNewTagUI::GetTemperature() {
	return m_nTemp;
}

void  CNewTagUI::SetTime(time_t t) {
	m_time = t;
	SetTime();
}

time_t   CNewTagUI::GetTime() {
	return m_time;
}

void  CNewTagUI::SetTime() {
	char szTime[256] = { 0 };
	if (m_bInitiated) {
		if (m_time > 0) {
			LmnFormatTime(szTime, sizeof(szTime), m_time, "%Y-%m-%d %H:%M:%S");
			m_lblTime->SetText(szTime);
		}
		else {
			m_lblTime->SetText("");
		}
	}
}

void  CNewTagUI::Fade() {
	m_lblTemp->SetTextColor(CUBE_FADE_TEXTCOLOR);
	m_lblTime->SetTextColor(CUBE_FADE_TEXTCOLOR);
}

void  CNewTagUI::Refresh() {
	m_lblTemp->SetTextColor(CUBE_REFRESH_TEXTCOLOR);
	m_lblTime->SetTextColor(CUBE_REFRESH_TEXTCOLOR);
}