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

	m_tDate = time(0);
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
	

	strText.Format("���˻�����Ϣ");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// ����
	pEdit = new CEditUI;
	m_tree->AddNode("����", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382); 

	// �Ա�
	pCombo = new CComboUI;
	AddComboItem(pCombo, "(��)", 0);
	AddComboItem(pCombo, "��", 1);
	AddComboItem(pCombo, "Ů", 2);
	pCombo->SelectItem(0);
	pCombo->SetFixedWidth(120);
	m_tree->AddNode("�Ա�", pTitleNode, 0, pCombo, 2, 0xFF386382, 2, 0xFF386382);

	// ����
	pEdit = new CEditUI;
	pEdit->SetNumberOnly(true);
	m_tree->AddNode("����", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// �����
	pEdit = new CEditUI;
	m_tree->AddNode("�����", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// סԺ��
	pEdit = new CEditUI;
	m_tree->AddNode("סԺ��", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// ��Ժ����
	strText.Format("��Ժ");
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666);
	pCheckBox = new CCheckBoxUI;
	pCheckBox->SetFixedWidth(20);
	m_tree->AddNode("�Ƿ���Ժ", pSubTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);
	pDateTime = new DuiLib::CDateTimeUI;
	pDateTime->SetFixedWidth(140);
	m_tree->AddNode("��Ժ����", pSubTitleNode, 0, pDateTime, 2, 0xFF386382, 2, 0xFF386382);

	// �Ʊ�
	pEdit = new CEditUI;
	m_tree->AddNode("�Ʊ�", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// ����
	pEdit = new CEditUI;
	m_tree->AddNode("����", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// ����
	pEdit = new CEditUI;
	m_tree->AddNode("����", pTitleNode, 0, pEdit, 2, 0xFF386382, 2, 0xFF386382);

	// ����
	strText.Format("����");
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666);
	pCheckBox = new CCheckBoxUI;
	pCheckBox->SetFixedWidth(20);
	m_tree->AddNode("�Ƿ�����", pSubTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);
	pDateTime = new DuiLib::CDateTimeUI;
	pDateTime->SetFixedWidth(140);
	m_tree->AddNode("��������", pSubTitleNode, 0, pDateTime, 2, 0xFF386382, 2, 0xFF386382);
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

	strText.Format("���˷���������");
	pTitleNode = m_tree->AddNode(strText, 0, 0, 0, 3, 0xFF666666);

	// ����  
	strText.Format("����");   
	pSixGrids = new CSixGridsUI;
	pSixGrids->SetMode(1);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pSixGrids, 3, 0xFF666666); 


	for (int i = 0; i < 7; i++) {
		pSixGrids = new CSixGridsUI;
		m_tree->AddNode(week_days[i], pSubTitleNode, 0, pSixGrids, 2, 0xFF386382, 2, 0xFF386382, 30 );     
	}

	// ���� 
	strText.Format("����");
	pSixGrids = new CSixGridsUI;
	pSixGrids->SetMode(1);	
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pSixGrids, 3, 0xFF666666);

	for (int i = 0; i < 7; i++) {
		pSixGrids = new CSixGridsUI;
		m_tree->AddNode(week_days[i], pSubTitleNode, 0, pSixGrids, 2, 0xFF386382, 2, 0xFF386382, 30);
	}
	 
	// ��������
	strText.Format("��������");
	pSevenGrids = new CSevenGridsUI;
	pSevenGrids->SetMode(1);
	GetSevenDayStr(week_days, 7, now, TRUE);
	pSevenGrids->SetWeekStr(week_days, 7);
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pSevenGrids, 3, 0xFF666666);

	const char * item_title[7] = { "������", "����(��) ml", "������ ml", "�ܳ��� ml", "Ѫѹ kpa", "���� kg", "����ҩ��" };
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
			pDays[i] += "(����)";         
		}
		else {
			pDays[i] += "(";
			pDays[i] += GetWeekDayShortName( (nWeekDayIndex + 1 + i) % 7 );
			pDays[i] += ")";
		}
	}
}

// �ӽ����ȡ�����info
void  CPatientDataDlg::GetPatientInfo(PatientInfo * pInfo) {
	memset(pInfo, 0, sizeof(PatientInfo));

	CMyTreeCfgUI::ConfigValue   cfgValue;
	int nRow = 1;

	// ����
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_szPName, cfgValue.m_strEdit, MAX_TAG_PNAME_LENGTH);
	nRow++;

	// �Ա�
	m_tree->GetConfigValue(nRow, cfgValue);
	pInfo->m_sex = cfgValue.m_nComboSel;
	nRow++;

	// ����
	m_tree->GetConfigValue(nRow, cfgValue);
	sscanf_s(cfgValue.m_strEdit, "%d", &pInfo->m_age);
	nRow++;

	// �����
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_szOutpatientNo, cfgValue.m_strEdit, MAX_OUTPATIENT_NO_LENGTH);
	nRow++;

	// סԺ��
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_szHospitalAdmissionNo, cfgValue.m_strEdit, MAX_HOSPITAL_ADMISSION_NO_LENGTH);
	nRow += 2;

	// ��Ժ����
	m_tree->GetConfigValue(nRow, cfgValue);
	if ( cfgValue.m_bCheckbox ) {
		m_tree->GetConfigValue(nRow+1, cfgValue);
		pInfo->m_in_hospital = cfgValue.m_time;
	}
	else {
		pInfo->m_in_hospital = 0;
	}
	nRow += 2;

	// ����
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_szMedicalDepartment, cfgValue.m_strEdit, MAX_MEDICAL_DEPARTMENT_LENGTH);
	nRow++;

	// ����
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_szWard, cfgValue.m_strEdit, MAX_WARD_LENGTH);
	nRow++;

	// ����
	m_tree->GetConfigValue(nRow, cfgValue);
	STRNCPY(pInfo->m_szBedNo, cfgValue.m_strEdit, MAX_BED_NO_LENGTH);
	nRow += 2;

	// ����
	m_tree->GetConfigValue(nRow, cfgValue);
	if (cfgValue.m_bCheckbox) {
		m_tree->GetConfigValue(nRow + 1, cfgValue);
		pInfo->m_surgery = cfgValue.m_time;
	}
	else {
		pInfo->m_surgery = 0;
	}
	nRow += 2;
}

// �ӽ����ȡ�����data
void  CPatientDataDlg::GetPatientData(PatientData * pData, DWORD dwSize) {
	assert(dwSize >= 7);
	memset(pData, 0, sizeof(PatientData) * 7);

	CMyTreeCfgUI::ConfigValue   cfgValue;
	int nRow = 17;

	// ����
	for (int i = 0; i < 7; i++) {
		m_tree->GetConfigValue(nRow, cfgValue);
		for (int j = 0; j < 6; j++) {
			sscanf_s(cfgValue.m_Values[j], "%d", &pData[i].m_pulse[j]);
		}		
		nRow++;
	}

	// ����
	nRow++;
	for (int i = 0; i < 7; i++) {
		m_tree->GetConfigValue(nRow, cfgValue);
		for (int j = 0; j < 6; j++) {
			sscanf_s(cfgValue.m_Values[j], "%d", &pData[i].m_breath[j]);
		}
		nRow++;
	}

	// ������
	nRow++;
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		sscanf_s(cfgValue.m_Values[i], "%d", &pData[i].m_defecate);
	}
	nRow++;

	// ����
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		sscanf_s(cfgValue.m_Values[i], "%d", &pData[i].m_urine);
	}
	nRow++;

	// ������
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		sscanf_s(cfgValue.m_Values[i], "%d", &pData[i].m_income);
	}
	nRow++;

	// �ܳ���
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		sscanf_s(cfgValue.m_Values[i], "%d", &pData[i].m_output);
	}
	nRow++;

	// Ѫѹ
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		float f = 0.0f;
		sscanf_s(cfgValue.m_Values[i], "%f", &f);
		pData[i].m_blood_pressure = (int)(f * 100);
	}
	nRow++;

	// ����
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		float f = 0.0f;
		sscanf_s(cfgValue.m_Values[i], "%f", &f);
		pData[i].m_weight = (int)(f * 100);
	}
	nRow++;

	// ����ҩ��
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		STRNCPY( pData[i].m_szIrritability, cfgValue.m_Values[i],MAX_IRRITABILITY_LENGTH );
	}
	nRow++;
}

void CPatientDataDlg::OnFinalMessage(HWND hWnd) {
	PatientInfo info;
	PatientData data[7];

	GetPatientInfo(&info);
	STRNCPY(info.m_szTagId, m_szTagId, MAX_TAG_ID_LENGTH);

	// ��������
	if ( 0 != strcmp(info.m_szPName, m_patient_info.m_szPName) ) {
		CBusiness::GetInstance()->SaveTagPNameAsyn(m_szTagId, info.m_szPName);
	}

	if ( IsPatientInfoChanged(&info) ) {
		// �������ݿ�
		CBusiness::GetInstance()->SavePatientInfoAsyn(&info);
	}

	GetPatientData(data, 7);

	for (DWORD i = 0; i < 7; i++) {
		if (IsPatientInfoChanged( &data[i], i ) ) {
			// �������ݿ�
		}
	}
	

	STRNCPY( m_szUIPName, info.m_szPName, MAX_TAG_PNAME_LENGTH );
	WindowImplBase::OnFinalMessage(hWnd);
}

// ����info�Ƿ�ı�
BOOL  CPatientDataDlg::IsPatientInfoChanged(PatientInfo * pInfo) {

	if ( pInfo->m_sex != m_patient_info.m_sex )
		return TRUE;

	if (pInfo->m_age != m_patient_info.m_age)
		return TRUE;

	if ( 0 != strcmp(pInfo->m_szOutpatientNo, m_patient_info.m_szOutpatientNo) )
		return TRUE;

	if (0 != strcmp(pInfo->m_szHospitalAdmissionNo, m_patient_info.m_szHospitalAdmissionNo))
		return TRUE;

	if (0 != strcmp(pInfo->m_szMedicalDepartment, m_patient_info.m_szMedicalDepartment))
		return TRUE;

	if (0 != strcmp(pInfo->m_szWard, m_patient_info.m_szWard))
		return TRUE;

	if (0 != strcmp(pInfo->m_szBedNo, m_patient_info.m_szBedNo))
		return TRUE;

	if ( pInfo->m_in_hospital != m_patient_info.m_in_hospital )
		return TRUE;

	if ( pInfo->m_surgery != m_patient_info.m_surgery )
		return TRUE;

	return FALSE;
}

// ����data�Ƿ�ı�
BOOL  CPatientDataDlg::IsPatientInfoChanged(PatientData * pData, int nIndex) {
	assert(nIndex >= 0 && nIndex < 7);

	for (int i = 0; i < 6; i++) {
		if (pData->m_pulse[i] != m_patient_data[nIndex].m_pulse[i]) {
			return TRUE;
		}
	}

	for (int i = 0; i < 6; i++) {
		if (pData->m_breath[i] != m_patient_data[nIndex].m_breath[i]) {
			return TRUE;
		}
	}

	if ( pData->m_defecate != m_patient_data[nIndex].m_defecate )
		return TRUE;

	if (pData->m_urine != m_patient_data[nIndex].m_urine )
		return TRUE;

	if (pData->m_income != m_patient_data[nIndex].m_income )
		return TRUE;

	if (pData->m_output != m_patient_data[nIndex].m_output )
		return TRUE;

	if (pData->m_blood_pressure != m_patient_data[nIndex].m_blood_pressure)
		return TRUE;

	if (pData->m_weight != m_patient_data[nIndex].m_weight)
		return TRUE;

	if ( 0 != strcmp( pData->m_szIrritability, m_patient_data[nIndex].m_szIrritability) )
		return TRUE;

	return FALSE;
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

	CMyTreeCfgUI::ConfigValue   cfgValue;
	int nRow = 1;

	// ����
	cfgValue.m_strEdit = m_patient_info.m_szPName;
	if (cfgValue.m_strEdit.GetLength() == 0)
		cfgValue.m_strEdit = m_szUIPName;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// �Ա�
	cfgValue.m_nComboSel = m_patient_info.m_sex;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// ����
	if ( m_patient_info.m_age > 0 ) {
		cfgValue.m_strEdit.Format("%d", m_patient_info.m_age);
	}
	else {
		cfgValue.m_strEdit = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// �����
	cfgValue.m_strEdit = m_patient_info.m_szOutpatientNo;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// סԺ��
	cfgValue.m_strEdit = m_patient_info.m_szHospitalAdmissionNo;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow+=2;
	
	// ��Ժ����
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

	// ����
	cfgValue.m_strEdit = m_patient_info.m_szMedicalDepartment;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// ����
	cfgValue.m_strEdit = m_patient_info.m_szWard;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// ����
	cfgValue.m_strEdit = m_patient_info.m_szBedNo;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow += 2;

	// ����
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

	time_t tFirstDay = m_tDate - 3600 * 24 * 6;
	CBusiness::GetInstance()->QueryPatientDataAsyn(m_szTagId, tFirstDay);
}

void  CPatientDataDlg::OnPatientDataRet(WPARAM wParam, LPARAM  lParam) {
	PatientData * pData = (PatientData *)wParam;
	memcpy(m_patient_data, pData, sizeof(PatientData) * 7);
	delete[] pData;

	CMyTreeCfgUI::ConfigValue   cfgValue;
	int nRow = 17;

	// ����
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

	// ����
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

	// ������
	nRow++;
	for (int i = 0; i < 7; i++) {
		if (m_patient_data[i].m_defecate > 0)
			cfgValue.m_Values[i].Format("%d", m_patient_data[i].m_defecate);
		else
			cfgValue.m_Values[i] = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// ����
	for (int i = 0; i < 7; i++) {
		if (m_patient_data[i].m_urine > 0)
			cfgValue.m_Values[i].Format("%d", m_patient_data[i].m_urine);
		else
			cfgValue.m_Values[i] = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// ������
	for (int i = 0; i < 7; i++) {
		if (m_patient_data[i].m_income > 0)
			cfgValue.m_Values[i].Format("%d", m_patient_data[i].m_income);
		else
			cfgValue.m_Values[i] = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// �ܳ���
	for (int i = 0; i < 7; i++) {
		if (m_patient_data[i].m_output > 0)
			cfgValue.m_Values[i].Format("%d", m_patient_data[i].m_output);
		else
			cfgValue.m_Values[i] = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// Ѫѹ
	for (int i = 0; i < 7; i++) {
		if (m_patient_data[i].m_blood_pressure > 0)
			cfgValue.m_Values[i].Format("%.1f", m_patient_data[i].m_blood_pressure/100.0);
		else
			cfgValue.m_Values[i] = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// ����
	for (int i = 0; i < 7; i++) {
		if (m_patient_data[i].m_weight > 0)
			cfgValue.m_Values[i].Format("%.1f", m_patient_data[i].m_weight / 100.0);
		else
			cfgValue.m_Values[i] = "";
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// ����ҩ��
	for (int i = 0; i < 7; i++) {
		cfgValue.m_Values[i] = m_patient_data[i].m_szIrritability;
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;
	

	SetBusy(FALSE);
}