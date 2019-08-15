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
	pItem->SetText("手术");
	pItem->SetTag(PTYPE_SURGERY);
	m_cmbType->Add(pItem);

	pItem = new CListLabelElementUI;
	pItem->SetText("分娩");
	pItem->SetTag(PTYPE_BIRTH);
	m_cmbType->Add(pItem);

	pItem = new CListLabelElementUI;
	pItem->SetText("转入");
	pItem->SetTag(PTYPE_TURN_IN);
	m_cmbType->Add(pItem);

	pItem = new CListLabelElementUI;
	pItem->SetText("转出");
	pItem->SetTag(PTYPE_TURN_OUT);
	m_cmbType->Add(pItem);

	pItem = new CListLabelElementUI;
	pItem->SetText("呼吸心跳停止");
	pItem->SetTag(PTYPE_DEATH);
	m_cmbType->Add(pItem);

	pItem = new CListLabelElementUI;
	pItem->SetText("请假");
	pItem->SetTag(PTYPE_HOLIDAY);
	m_cmbType->Add(pItem);

	m_cmbType->SelectItem(m_nType);

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
			//告诉UIManager这个消息需要处理
			m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
		}
	}
}

void CMyEventUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_BUTTONDOWN) {
		//告诉UIManager这个消息需要处理
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
	nType = m_cmbType->GetCurSel();
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
			m_pManager->SendNotify(this, "tempui_setfocus");
		}
	}
	else if (msg.sType == "setfocus") {
		if ( msg.pSender == m_temp1 || msg.pSender == m_temp2) {
			//OutputDebugString("==== setfocus \n");
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