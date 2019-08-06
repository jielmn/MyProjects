#include "PatientDataDlg.h"
#include "SixGridsUI.h"


CPatientDataDlg::CPatientDataDlg() {
	m_tree = 0;
	m_switch = 0;

	memset(m_szTagId, 0, sizeof(m_szTagId));
	memset(m_szUIPName, 0, sizeof(m_szUIPName));

	m_waiting_bar = 0;
	m_bBusy = FALSE;

	memset( &m_patient_info, 0, sizeof(m_patient_info) );
	memset( m_patient_data,  0, sizeof(m_patient_data) );
}

void   CPatientDataDlg::Notify(DuiLib::TNotifyUI& msg) {
	CDuiString  name = msg.pSender->GetName();

	if (msg.sType == "windowinit") {
		OnMyInited();
	}
	else if (msg.sType == "click") {
		if ( name == "btnPrintPreview" ) {
			OnPrintPreview();
		}
		else if (name == "btnReturn") {
			OnReturn();
		}
		else if (name == "btnPrint") {
			this->PostMessage(WM_CLOSE);
		}
	}
	WindowImplBase::Notify(msg);
}

void   CPatientDataDlg::InitWindow() {
	WindowImplBase::InitWindow();
}

void  CPatientDataDlg::OnMyInited() {
	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl(MYTREE_PATIENT_DATA_NAME);
	m_switch = (CTabLayoutUI *)m_PaintManager.FindControl("switch");
	m_waiting_bar = (CWaitingBarUI *)m_PaintManager.FindControl("waiting_bar");
	m_btnPreview = (CButtonUI *)m_PaintManager.FindControl("btnPrintPreview");
	m_btnPrint = (CButtonUI *)m_PaintManager.FindControl("btnPrint");
	m_btnReturn = (CButtonUI *)m_PaintManager.FindControl("btnReturn");

	InitInfo();
	InitData();

	CBusiness::GetInstance()->QueryPatientInfoAsyn(m_szTagId);
	SetBusy(TRUE);
}

LRESULT  CPatientDataDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CPatientDataDlg::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	if ( uMsg == UM_PATIENT_INFO ) {
		OnPatientInfoRet(wParam, lParam);
	}
	else if (uMsg == UM_PATIENT_DATA) {
		OnPatientDataRet(wParam, lParam);
	}
	return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

DuiLib::CControlUI * CPatientDataDlg::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, MYTREE_CLASS_NAME)) {
		return new CMyTreeCfgUI(180);
	}
	else if (0 == strcmp(pstrClass, "WaitingBar")) {
		return new CWaitingBarUI;
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void   CPatientDataDlg::InitInfo() {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pTitleNode = NULL;
	CMyTreeCfgUI::Node* pSubTitleNode = NULL;
	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;
	CCheckBoxUI * pCheckBox = 0;
	CDateTimeUI * pDateTime = 0;
	

	strText.Format("病人基础信息");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// 姓名
	pEdit = new CEditUI;
	m_tree->AddNode("姓名", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382); 

	// 性别
	pCombo = new CComboUI;
	AddComboItem(pCombo, "(空)", 0);
	AddComboItem(pCombo, "男", 1);
	AddComboItem(pCombo, "女", 2);
	pCombo->SelectItem(0);
	pCombo->SetFixedWidth(60);
	m_tree->AddNode("性别", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382);

	// 年龄
	pEdit = new CEditUI;
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("年龄", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 门诊号
	pEdit = new CEditUI;
	m_tree->AddNode("门诊号", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 住院号
	pEdit = new CEditUI;
	m_tree->AddNode("住院号", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 入院日期
	strText.Format("入院");
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666);
	pCheckBox = new CCheckBoxUI;
	pCheckBox->SetFixedWidth(20);
	m_tree->AddNode("是否入院", pSubTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);
	pDateTime = new DuiLib::CDateTimeUI;
	pDateTime->SetFixedWidth(140);
	m_tree->AddNode("入院日期", pSubTitleNode, 0, pDateTime, 2, 0xFF386382, 2, 0xFF386382);

	// 科别
	pEdit = new CEditUI;
	m_tree->AddNode("科别", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 病室
	pEdit = new CEditUI;
	m_tree->AddNode("病室", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 床号
	pEdit = new CEditUI;
	m_tree->AddNode("床号", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// 手术
	strText.Format("手术");
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666);
	pCheckBox = new CCheckBoxUI;
	pCheckBox->SetFixedWidth(20);
	m_tree->AddNode("是否手术", pSubTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);
	pDateTime = new DuiLib::CDateTimeUI;
	pDateTime->SetFixedWidth(140);
	m_tree->AddNode("手术日期", pSubTitleNode, 0, pDateTime, 2, 0xFF386382, 2, 0xFF386382);
}

void  CPatientDataDlg::AddComboItem(CComboUI * pCombo, const char * szItem, UINT_PTR tag) {
	CListLabelElementUI * pElement = new CListLabelElementUI;
	pElement->SetText(szItem);
	pElement->SetTag(tag);
	pCombo->Add(pElement);
}

void  CPatientDataDlg::InitData() {
	DuiLib::CDuiString  strText;

	CMyTreeCfgUI::Node* pTitleNode = NULL;
	CComboUI * pCombo = 0;
	CListLabelElementUI * pElement = 0;
	CEditUI * pEdit = 0;
	CCheckBoxUI * pCheckBox = 0;
	CDateTimeUI * pDateTime = 0;
	CMyTreeCfgUI::Node* pSubTitleNode = NULL;
	CMyTreeCfgUI::Node* pSubTitleNode_1 = NULL;
	CSixGridsUI * pSixGrids = 0;
	CSevenGridsUI * pSevenGrids = 0;
	CDuiString  week_days[7];

	time_t now = time(0);
	GetSevenDayStr(week_days, 7, now);

	strText.Format("病人非体温数据");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// 脉搏  
	strText.Format("脉搏");   
	pSixGrids = new CSixGridsUI;
	pSixGrids->SetMode(1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pSixGrids, 3, 0xFF666666); 


	for (int i = 0; i < 7; i++) {
		pSixGrids = new CSixGridsUI;
		m_tree->AddNode(week_days[i], pSubTitleNode, 0, pSixGrids, 2, 0xFF386382, 2, 0xFF386382, 30 );     
	}

	// 呼吸 
	strText.Format("呼吸");
	pSixGrids = new CSixGridsUI;
	pSixGrids->SetMode(1);	
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pSixGrids, 3, 0xFF666666);

	for (int i = 0; i < 7; i++) {
		pSixGrids = new CSixGridsUI;
		m_tree->AddNode(week_days[i], pSubTitleNode, 0, pSixGrids, 2, 0xFF386382, 2, 0xFF386382, 30);
	}
	 
	// 其他数据
	strText.Format("其他数据");
	pSevenGrids = new CSevenGridsUI;
	pSevenGrids->SetMode(1);
	GetSevenDayStr(week_days, 7, now, TRUE);
	pSevenGrids->SetWeekStr(week_days, 7);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pSevenGrids, 3, 0xFF666666);

	const char * item_title[7] = { "大便次数", "尿量(次) ml", "总入量 ml", "总出量 ml", "血压 kpa", "体重 kg", "过敏药物" };
	for (int i = 0; i < 7; i++) {
		pSevenGrids = new CSevenGridsUI;
		if (i == 6)
			pSevenGrids->SetNumberOnly(FALSE); 
		m_tree->AddNode(item_title[i], pSubTitleNode, 0, pSevenGrids, 2, 0xFF386382, 2, 0xFF386382, 30);    
	}
}

void CPatientDataDlg::OnPrintPreview() {
	m_switch->SelectItem(1);
}

void CPatientDataDlg::OnReturn() {
	m_switch->SelectItem(0);      
}

void  CPatientDataDlg::GetSevenDayStr(CDuiString * pDays, DWORD dwSize, time_t tLastTime, BOOL bMonthDay /*= FALSE*/) {
	assert(dwSize >= 7);
	if ( dwSize < 7 ) {
		return;
	}

	char buf[256];
	time_t tZeroTime = GetAnyDayZeroTime(tLastTime);
	time_t tTodayZeroTime = GetTodayZeroTime();
	int nWeekDayIndex = GetWeekDay(tZeroTime);

	for (int i = 0; i < 7; i++) {
		time_t t = tZeroTime - (6-i) * 3600 * 24;
		if (bMonthDay)
			Date2String_md(buf, sizeof(buf), &t);
		else
			Date2String(buf, sizeof(buf), &t);

		pDays[i] = buf;
		if (t == tTodayZeroTime) {
			pDays[i] += "(今天)";         
		}
		else {
			pDays[i] += "(";
			pDays[i] += GetWeekDayShortName( (nWeekDayIndex + 1 + i) % 7 );
			pDays[i] += ")";
		}
	}
}

void CPatientDataDlg::OnFinalMessage(HWND hWnd) {
	WindowImplBase::OnFinalMessage(hWnd);
}

void CPatientDataDlg::SetBusy(BOOL bBusy /*= TRUE*/) {
	if (m_bBusy == bBusy)
		return;

	if ( bBusy ) {
		m_tree->SetMouseChildEnabled(false);
		//m_waiting_bar->SetVisible(true);
		//m_waiting_bar->Start();
		m_btnPreview->SetEnabled(false);
		m_btnPrint->SetEnabled(false);
		m_btnReturn->SetEnabled(false);
	}
	else {
		m_tree->SetMouseChildEnabled(true);
		//m_waiting_bar->Stop();
		//m_waiting_bar->SetVisible(false);		
		m_btnPreview->SetEnabled(true);
		m_btnPrint->SetEnabled(true);
		m_btnReturn->SetEnabled(true);

	}
	
	m_bBusy = bBusy;
}

void  CPatientDataDlg::OnPatientInfo(PatientInfo * pInfo) {
	PatientInfo * pNewInfo = new PatientInfo;
	memcpy(pNewInfo, pInfo, sizeof(PatientInfo));

	::PostMessage(GetHWND(), UM_PATIENT_INFO, (WPARAM)pNewInfo, 0);
}

void  CPatientDataDlg::OnPatientData(PatientData * pData, DWORD dwSize) {
	assert(dwSize >= 7);

	PatientData * pNewData = new PatientData[7];
	memcpy(pNewData, pData, sizeof(PatientData) * 7);

	::PostMessage(GetHWND(), UM_PATIENT_DATA, (WPARAM)pNewData, 0);
}

void  CPatientDataDlg::OnPatientInfoRet(WPARAM wParam, LPARAM  lParam) {
	PatientInfo * pInfo = (PatientInfo *)wParam;
	memcpy(&m_patient_info, pInfo, sizeof(PatientInfo));
	delete pInfo;

	if (m_patient_info.m_szPName[0] == '\0') {
		STRNCPY(m_patient_info.m_szPName, m_szUIPName, MAX_TAG_PNAME_LENGTH);
	}

	CMyTreeCfgUI::ConfigValue   cfgValue;
	int nRow = 1;

	// 姓名
	cfgValue.m_strEdit = m_patient_info.m_szPName;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 性别
	cfgValue.m_nComboSel = m_patient_info.m_sex;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 年龄
	if ( m_patient_info.m_age > 0 ) {
		cfgValue.m_strEdit.Format("%d", m_patient_info.m_age);
	}
	else {
		cfgValue.m_strEdit = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 门诊号
	cfgValue.m_strEdit = m_patient_info.m_szOutpatientNo;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 住院号
	cfgValue.m_strEdit = m_patient_info.m_szHospitalAdmissionNo;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow+=2;
	
	// 入院日期
	if ( m_patient_info.m_in_hospital > 0 ) {
		cfgValue.m_bCheckbox = TRUE;
		m_tree->SetConfigValue(nRow, cfgValue);

		cfgValue.m_time = m_patient_info.m_in_hospital;
		m_tree->SetConfigValue(nRow+1, cfgValue);
	}
	else {
		cfgValue.m_bCheckbox = FALSE;
		m_tree->SetConfigValue(nRow, cfgValue);
		m_tree->Invalidate();
	}
	nRow+=2;

	// 科室
	cfgValue.m_strEdit = m_patient_info.m_szMedicalDepartment;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 病室
	cfgValue.m_strEdit = m_patient_info.m_szWard;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 床号
	cfgValue.m_strEdit = m_patient_info.m_szBedNo;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow += 2;

	// 手术
	if (m_patient_info.m_surgery > 0) {
		cfgValue.m_bCheckbox = TRUE;
		m_tree->SetConfigValue(nRow, cfgValue);

		cfgValue.m_time = m_patient_info.m_surgery;
		m_tree->SetConfigValue(nRow + 1, cfgValue);
	}
	else {
		cfgValue.m_bCheckbox = FALSE;
		m_tree->SetConfigValue(nRow, cfgValue);
		m_tree->Invalidate();
	}
	nRow += 2;	

	time_t now = time(0);
	time_t tFirstDay = now - 3600 * 24 * 6;
	CBusiness::GetInstance()->QueryPatientDataAsyn(m_szTagId, tFirstDay);
}

void  CPatientDataDlg::OnPatientDataRet(WPARAM wParam, LPARAM  lParam) {
	PatientData * pData = (PatientData *)wParam;
	memcpy(m_patient_data, pData, sizeof(PatientData) * 7);
	delete[] pData;

	CMyTreeCfgUI::ConfigValue   cfgValue;
	int nRow = 17;

	// 脉搏
	for (int i = 0; i < 7; i++) {		
		for (int j = 0; j < 6; j++) {
			if (m_patient_data[i].m_pulse[j] > 0)
				cfgValue.m_Values[j].Format("%d", m_patient_data[i].m_pulse[j]);
			else
				cfgValue.m_Values[j] = "";
		}
		m_tree->SetConfigValue(nRow, cfgValue);
		nRow++;
	}

	// 呼吸
	nRow++;
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 6; j++) {
			if (m_patient_data[i].m_breath[j] > 0)
				cfgValue.m_Values[j].Format("%d", m_patient_data[i].m_breath[j]);
			else
				cfgValue.m_Values[j] = "";
		}
		m_tree->SetConfigValue(nRow, cfgValue);
		nRow++;
	}

	// 大便次数
	nRow++;
	for (int i = 0; i < 7; i++) {
		if (m_patient_data[i].m_defecate > 0)
			cfgValue.m_Values[i].Format("%d", m_patient_data[i].m_defecate);
		else
			cfgValue.m_Values[i] = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 尿量
	for (int i = 0; i < 7; i++) {
		if (m_patient_data[i].m_urine > 0)
			cfgValue.m_Values[i].Format("%d", m_patient_data[i].m_urine);
		else
			cfgValue.m_Values[i] = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 总入量
	for (int i = 0; i < 7; i++) {
		if (m_patient_data[i].m_income > 0)
			cfgValue.m_Values[i].Format("%d", m_patient_data[i].m_income);
		else
			cfgValue.m_Values[i] = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 总出量
	for (int i = 0; i < 7; i++) {
		if (m_patient_data[i].m_output > 0)
			cfgValue.m_Values[i].Format("%d", m_patient_data[i].m_output);
		else
			cfgValue.m_Values[i] = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 血压
	for (int i = 0; i < 7; i++) {
		if (m_patient_data[i].m_blood_pressure > 0)
			cfgValue.m_Values[i].Format("%.1f", m_patient_data[i].m_blood_pressure/100.0);
		else
			cfgValue.m_Values[i] = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 体重
	for (int i = 0; i < 7; i++) {
		if (m_patient_data[i].m_weight > 0)
			cfgValue.m_Values[i].Format("%.1f", m_patient_data[i].m_weight / 100.0);
		else
			cfgValue.m_Values[i] = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// 过敏药物
	for (int i = 0; i < 7; i++) {
		cfgValue.m_Values[i] = m_patient_data[i].m_szIrritability;
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;
	

	SetBusy(FALSE);
}