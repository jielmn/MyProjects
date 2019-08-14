#include "PatientDataDlg.h"
#include "SixGridsUI.h"
#include "resource.h"

#define TIMER_REDRAW_PREVIEW      1001
#define INTERVAL_REDRAW_PREVIEW   100

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
	m_preview = 0;

	m_date_start = 0;
	m_date_end = 0;
	m_lay_events = 0;
	m_selected_event = -1;
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
			OnPrint();
			this->PostMessage(WM_CLOSE);
		}
		else if (name == "btnZoomIn") {
			m_preview->ZoomIn();
		}
		else if (name == "btnZoomOut") {
			m_preview->ZoomOut();
		}
		else if (name == "btnAddEvent") {
			OnAddMyEvent();
		}
		else if (name == "btnDelEvent") {
			OnDelMyEvent();
		}
		else {
			if ( msg.pSender->GetClass() == "MyEvent" ) {
				OnMyEventSelected(msg.pSender);
			}
		}
	}
	else if (msg.sType == "textchanged") {
		if (name == "DateTimeS") {
			OnDateStartChanged();			
		}
	}
	else if (msg.sType == "killfocus") {
		if (name == "DateTimeS") {
			OnDateStartKillFocus();
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
	m_preview = (CPatientDataPrintPreviewUI *)m_PaintManager.FindControl("preview");
	m_date_start = (CMyDateUI *)m_PaintManager.FindControl("DateTimeS");
	m_date_end = (CMyDateUI *)m_PaintManager.FindControl("DateTimeE");

	m_tree->SetSelectedItemBkColor(0xFFFFFFFF);
	m_tree->SetHotItemBkColor(0xFFFFFFFF);

	InitInfo();
	InitData();

	time_t tZeroTime = GetAnyDayZeroTime(m_tDate);
	time_t tFirstDay = tZeroTime - 3600 * 24 * 6;
	SYSTEMTIME s = Time2SysTime(tFirstDay);
	m_date_start->SetMyTime(&s);
	CBusiness::GetInstance()->QueryPatientInfoAsyn(m_szTagId);
	SetBusy(TRUE);
}

LRESULT  CPatientDataDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_TIMER) {
		if (wParam == TIMER_REDRAW_PREVIEW) {
			m_preview->Invalidate();
			KillTimer(GetHWND(), wParam);  
		}
	}
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
		return new CMyTreeCfgUI(160);
	}
	else if (0 == strcmp(pstrClass, "WaitingBar")) {
		return new CWaitingBarUI;
	}
	else if (0 == strcmp(pstrClass, "PatientDataPrintPreview")) {
		return new CPatientDataPrintPreviewUI;
	}
	else if (0 == strcmp(pstrClass, "MyDateTime")) {
		return new CMyDateUI;
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
	CMyDateUI * pDateTime = 0;
	CShiftUI * pShift = 0;
	CHorizontalLayoutUI * pHLayout = 0;
	CButtonUI * pBtn = 0;
	CControlUI * pCtl = 0;

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
	pDateTime = new CMyDateUI;
	pDateTime->SetFixedWidth(140);
	m_tree->AddNode("��Ժ����", pSubTitleNode, 0, pDateTime, 2, 0xFF386382, 2, 0xFF386382);

	// ��Ժ����
	strText.Format("��Ժ");
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, 0, 3, 0xFF666666);
	pCheckBox = new CCheckBoxUI;
	pCheckBox->SetFixedWidth(20);
	m_tree->AddNode("�Ƿ��Ժ", pSubTitleNode, 0, pCheckBox, 2, 0xFF386382, 2, 0xFF386382);
	pDateTime = new CMyDateUI;
	pDateTime->SetFixedWidth(140);
	m_tree->AddNode("��Ժ����", pSubTitleNode, 0, pDateTime, 2, 0xFF386382, 2, 0xFF386382);

	// �Ʊ�
	pShift = new CShiftUI;    
	m_tree->AddNode("�Ʊ�", pTitleNode, 0, pShift, 2, 0xFF386382, -1, -1, 30 );         

	// ����
	pShift = new CShiftUI;
	m_tree->AddNode("����", pTitleNode, 0, pShift, 2, 0xFF386382, -1, -1, 30);

	// ����
	pShift = new CShiftUI;
	m_tree->AddNode("����", pTitleNode, 0, pShift, 2, 0xFF386382, -1, -1, 30);

	// �����¼�
	strText.Format("�¼�");
	pHLayout = new CHorizontalLayoutUI;
	pCtl = new CControlUI;
	pHLayout->Add(pCtl);

	pBtn = new CButtonUI;
	pBtn->SetText("���");
	pBtn->SetFixedWidth(60);
	pBtn->SetNormalImage("file='win7_button_normal.png' corner='5,5,5,5' hole='false'");
	pBtn->SetHotImage("file='win7_button_hot.png' corner='5,5,5,5' hole='false'");
	pBtn->SetPushedImage("file='win7_button_pushed.png' corner='5,5,5,5' hole='false'");
	pBtn->SetFont(1);
	pBtn->SetTextColor(0xFF386382);
	pBtn->SetName("btnAddEvent");
	pHLayout->Add(pBtn);

	pCtl = new CControlUI;
	pCtl->SetFixedWidth(10);
	pHLayout->Add(pCtl);

	pBtn = new CButtonUI;
	pBtn->SetText("ɾ��");
	pBtn->SetFixedWidth(60);
	pBtn->SetNormalImage("file='win7_button_normal.png' corner='5,5,5,5' hole='false'");
	pBtn->SetHotImage("file='win7_button_hot.png' corner='5,5,5,5' hole='false'");
	pBtn->SetPushedImage("file='win7_button_pushed.png' corner='5,5,5,5' hole='false'");
	pBtn->SetFont(1);
	pBtn->SetTextColor(0xFF386382);
	pBtn->SetName("btnDelEvent");
	pHLayout->Add(pBtn);  
	
	pSubTitleNode = m_tree->AddNode(strText, pTitleNode, 0, pHLayout, 3, 0xFF666666, -1, -1, 32);
	m_lay_events = new CVerticalLayoutUI;
	m_lay_events->SetName("12345");
	RECT r = { 1,1,1,1 };
	m_lay_events->SetInset(r);
	m_lay_events->SetManager(&this->m_PaintManager, 0);
	m_lay_events->EnableScrollBar(true);
	m_lay_events->SetSepHeight(4);
	m_tree->AddNode("�������¼�", pSubTitleNode, 0, m_lay_events, 2, 0xFF386382, -1, -1, 99);
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
		pSixGrids->SetNumberOnly(FALSE);
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
		pSevenGrids->SetNumberOnly(FALSE);
		if (i == 4)
			pSevenGrids->SetFont(5);    
		m_tree->AddNode(item_title[i], pSubTitleNode, 0, pSevenGrids, 2, 0xFF386382, 2, 0xFF386382, 30);    
	}
}

void CPatientDataDlg::OnPrintPreview() {
	PatientInfo info;
	PatientData data[7];

	// ��ȡUI��������Ϣ
	GetPatientInfo(&info);
	GetPatientData(data, 7);

	memcpy(&m_preview->m_patient_info, &info, sizeof(PatientInfo));
	memcpy(m_preview->m_patient_data, data, sizeof(PatientData) * 7);
	m_preview->m_tFirstDay = m_tDate - 3600 * 24 * 6;
	for (int i = 0; i < 7; i++) {
		memcpy(m_preview->m_patient_data[i].m_temp, m_patient_data[i].m_temp, sizeof(int) * 6);
	}

	m_switch->SelectItem(1);
	m_preview->Invalidate();
}

void CPatientDataDlg::OnReturn() {
	m_switch->SelectItem(0);      
}

void  CPatientDataDlg::OnPrint() {
	PatientInfo info;
	PatientData data[7];

	// ��ȡUI��������Ϣ
	GetPatientInfo(&info);
	GetPatientData(data, 7);
	time_t tFirstDay = m_tDate - 3600 * 24 * 6;

	for (int i = 0; i < 7; i++) {
		memcpy(data[i].m_temp, m_patient_data[i].m_temp, sizeof(int) * 6);
	}

	CXml2ChartFile   xmlChart;
	LoadXmlChart(xmlChart);


	PRINTDLG printInfo;
	ZeroMemory(&printInfo, sizeof(printInfo));  //��ոýṹ     
	printInfo.lStructSize = sizeof(printInfo);
	printInfo.hwndOwner = 0;
	printInfo.hDevMode = 0;
	printInfo.hDevNames = 0;
	//����ǹؼ���PD_RETURNDC ������������־�����ò���hDC��      
	//            PD_RETURNDEFAULT ������ǵõ�Ĭ�ϴ�ӡ��������Ҫ�����öԻ���     
	//printInfo.Flags = PD_RETURNDC | PD_RETURNDEFAULT;   
	printInfo.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_RETURNDEFAULT;
	printInfo.nCopies = 1;
	printInfo.nFromPage = 0xFFFF;
	printInfo.nToPage = 0xFFFF;
	printInfo.nMinPage = 1;
	printInfo.nMaxPage = 0xFFFF;

	//����API�ó�Ĭ�ϴ�ӡ��     
	//PrintDlg(&printInfo);
	if (PrintDlg(&printInfo) == TRUE)
	{
		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = _T("MyXPS");

		::SetMapMode(printInfo.hDC, MM_ANISOTROPIC);

		SIZE  size = { 760, 1044 };
		::SetWindowExtEx(printInfo.hDC, size.cx, size.cy, 0);
		size.cx = GetDeviceCaps(printInfo.hDC, HORZRES);
		size.cy = GetDeviceCaps(printInfo.hDC, VERTRES);
		::SetViewportExtEx(printInfo.hDC, size.cx, size.cy, 0);

		StartDoc(printInfo.hDC, &di);

		for (int i = 0; i < 1; i++) {
			StartPage(printInfo.hDC);

			//SetBkMode(printInfo.hDC, TRANSPARENT);
			//DrawXml2ChartUI(printInfo.hDC, xmlChart.m_ChartUI);

			PrepareXmlChart(xmlChart, &info, data, 7, tFirstDay);
			PrintXmlChart(printInfo.hDC, xmlChart, 0, 0, data, 7);

			EndPage(printInfo.hDC);
		}

		EndDoc(printInfo.hDC);
		DeleteDC(printInfo.hDC);
	}
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

	//// ����
	//m_tree->GetConfigValue(nRow, cfgValue);
	//if (cfgValue.m_bCheckbox) {
	//	m_tree->GetConfigValue(nRow + 1, cfgValue);
	//	pInfo->m_surgery = cfgValue.m_time;
	//}
	//else {
	//	pInfo->m_surgery = 0;
	//}
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

	//// ����
	//nRow++;
	//for (int i = 0; i < 7; i++) {
	//	m_tree->GetConfigValue(nRow, cfgValue);
	//	for (int j = 0; j < 6; j++) {
	//		sscanf_s(cfgValue.m_Values[j], "%d", &pData[i].m_breath[j]);
	//	}
	//	nRow++;
	//}

	//// ������
	//nRow++;
	//m_tree->GetConfigValue(nRow, cfgValue);
	//for (int i = 0; i < 7; i++) {
	//	sscanf_s(cfgValue.m_Values[i], "%d", &pData[i].m_defecate);
	//}
	//nRow++;

	//// ����
	//m_tree->GetConfigValue(nRow, cfgValue);
	//for (int i = 0; i < 7; i++) {
	//	sscanf_s(cfgValue.m_Values[i], "%d", &pData[i].m_urine);
	//}
	//nRow++;

	//// ������
	//m_tree->GetConfigValue(nRow, cfgValue);
	//for (int i = 0; i < 7; i++) {
	//	sscanf_s(cfgValue.m_Values[i], "%d", &pData[i].m_income);
	//}
	//nRow++;

	//// �ܳ���
	//m_tree->GetConfigValue(nRow, cfgValue);
	//for (int i = 0; i < 7; i++) {
	//	sscanf_s(cfgValue.m_Values[i], "%d", &pData[i].m_output);
	//}
	//nRow++;

	// Ѫѹ
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		STRNCPY(pData[i].m_szBloodPressure, cfgValue.m_Values[i], MAX_BLOOD_PRESSURE_LENGTH);		
	}
	nRow++;

	// ����
	m_tree->GetConfigValue(nRow, cfgValue);
	for (int i = 0; i < 7; i++) {
		STRNCPY(pData[i].m_szWeight, cfgValue.m_Values[i], MAX_WEIGHT_LENGTH);		
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
	//PatientInfo info;
	//PatientData data[7];

	//GetPatientInfo(&info);
	//STRNCPY(info.m_szTagId, m_szTagId, MAX_TAG_ID_LENGTH);

	//// ��������
	//if ( 0 != strcmp(info.m_szPName, m_patient_info.m_szPName) ) {
	//	CBusiness::GetInstance()->SaveTagPNameAsyn(m_szTagId, info.m_szPName);
	//}

	//if ( IsPatientInfoChanged(&info) ) {
	//	// �������ݿ�
	//	CBusiness::GetInstance()->SavePatientInfoAsyn(&info);
	//}

	//GetPatientData(data, 7);
	//time_t  tFirstDay = m_tDate - 3600 * 24 * 6;

	//for (DWORD i = 0; i < 7; i++) {
	//	if (IsPatientInfoChanged( &data[i], i ) ) {
	//		STRNCPY( data[i].m_szTagId, m_szTagId, MAX_TAG_ID_LENGTH ); 
	//		data[i].m_date = tFirstDay + 3600 * 24 * i;
	//		// �������ݿ�
	//		CBusiness::GetInstance()->SavePatientDataAsyn(&data[i]);
	//	}
	//}
	//

	//STRNCPY( m_szUIPName, info.m_szPName, MAX_TAG_PNAME_LENGTH );
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

	//if ( pInfo->m_surgery != m_patient_info.m_surgery )
	//	return TRUE;

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

	if ( 0 != strcmp(pData->m_szBloodPressure, m_patient_data[nIndex].m_szBloodPressure) )
		return TRUE;

	if ( 0 != strcmp(pData->m_szWeight,m_patient_data[nIndex].m_szWeight) )
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
		m_waiting_bar->SetVisible(true);
		m_waiting_bar->Start();
		m_btnPreview->SetEnabled(false);
		m_btnPrint->SetEnabled(false);
		m_btnReturn->SetEnabled(false);
	}
	else {
		m_tree->SetMouseChildEnabled(true);
		m_waiting_bar->Stop();
		m_waiting_bar->SetVisible(false);		
		m_btnPreview->SetEnabled(true);
		m_btnPrint->SetEnabled(true);
		m_btnReturn->SetEnabled(true);

	}
	
	m_bBusy = bBusy;
}

void  CPatientDataDlg::OnPatientInfo(PatientInfo * pInfo, const std::vector<PatientEvent * > & vEvents) {
	PatientInfo * pNewInfo = new PatientInfo;
	memcpy(pNewInfo, pInfo, sizeof(PatientInfo));

	std::vector<PatientEvent * > * pVec = new std::vector<PatientEvent * >;
	std::vector<PatientEvent * >::const_iterator it;
	for (it = vEvents.begin(); it != vEvents.end(); ++it) {
		PatientEvent * pEvent = *it;
		PatientEvent * pNewEvent = new PatientEvent;
		memcpy(pNewEvent, pEvent, sizeof(PatientEvent));
		pVec->push_back(pNewEvent);
	}

	::PostMessage(GetHWND(), UM_PATIENT_INFO, (WPARAM)pNewInfo, (LPARAM)pVec);
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

	std::vector<PatientEvent * > * pVec = (std::vector<PatientEvent * > *)lParam;

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
	cfgValue.m_strEdit = FormatInt(m_patient_info.m_age);
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// �����
	cfgValue.m_strEdit = m_patient_info.m_szOutpatientNo;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// סԺ��
	cfgValue.m_strEdit = m_patient_info.m_szHospitalAdmissionNo;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;
	
	// ��Ժ����
	if ( m_patient_info.m_in_hospital > 0 ) {
		cfgValue.m_bCheckbox = TRUE;
		m_tree->SetConfigValue(nRow+1, cfgValue);

		cfgValue.m_time = m_patient_info.m_in_hospital;
		m_tree->SetConfigValue(nRow+2, cfgValue);
	}
	else {
		cfgValue.m_bCheckbox = FALSE;
		m_tree->SetConfigValue(nRow+1, cfgValue);
	}
	nRow+=3;

	// ��Ժ����
	if (m_patient_info.m_out_hospital > 0) {
		cfgValue.m_bCheckbox = TRUE;
		m_tree->SetConfigValue(nRow + 1, cfgValue);

		cfgValue.m_time = m_patient_info.m_out_hospital;
		m_tree->SetConfigValue(nRow + 2, cfgValue);
	}
	else {
		cfgValue.m_bCheckbox = FALSE;
		m_tree->SetConfigValue(nRow + 1, cfgValue);
	}
	nRow += 3;

	// ����
	cfgValue.m_Values[0] = m_patient_info.m_szMedicalDepartment;
	cfgValue.m_Values[1] = m_patient_info.m_szMedicalDepartment2;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// ����
	cfgValue.m_Values[0] = m_patient_info.m_szWard;
	cfgValue.m_Values[1] = m_patient_info.m_szWard2;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// ����
	cfgValue.m_Values[0] = m_patient_info.m_szBedNo;
	cfgValue.m_Values[1] = m_patient_info.m_szBedNo;
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// �¼�

	time_t tFirstDay = m_tDate - 3600 * 24 * 6;
	CBusiness::GetInstance()->QueryPatientDataAsyn(m_szTagId, tFirstDay);

	if (pVec) {
		ClearVector(*pVec);
		delete pVec;
	}
}

void  CPatientDataDlg::OnPatientDataRet(WPARAM wParam, LPARAM  lParam) {
	PatientData * pData = (PatientData *)wParam;
	memcpy(m_patient_data, pData, sizeof(PatientData) * 7);
	delete[] pData;
	return;

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
		cfgValue.m_Values[i] = m_patient_data[i].m_szBloodPressure;
	}
	m_tree->SetConfigValue(nRow, cfgValue);
	nRow++;

	// ����
	for (int i = 0; i < 7; i++) {
		cfgValue.m_Values[i] = m_patient_data[i].m_szWeight;
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

// ʱ�䷶Χ����ʼʱ��ı�
void  CPatientDataDlg::OnDateStartChanged() {
	//CDuiString  str;
	//SYSTEMTIME s = m_date_start->GetTime();
	//str.Format("%d-%d-%d %d:%d:%d \n", s.wYear, s.wMonth, s.wDay, s.wHour, s.wMinute, s.wSecond);
	//::OutputDebugString(str);
}

// ʱ��ؼ�ʧȥ����
void  CPatientDataDlg::OnDateStartKillFocus() {
	SYSTEMTIME s = m_date_start->GetTime();
	time_t e = SysTime2Time(s) + 3600 * 24 * 6;
	SYSTEMTIME e1 = Time2SysTime(e);
	m_date_end->SetMyTime(&e1);
}

// ѡ�����¼�UI
void  CPatientDataDlg::OnMyEventSelected(CControlUI * pCtl) {
	int cnt = m_lay_events->GetCount();
	for (int i = 0; i < cnt; i++) {
		CMyEventUI * pItem = (CMyEventUI *)m_lay_events->GetItemAt(i);
		if (pItem == pCtl) {
			pItem->SetSelected(TRUE); 
			m_selected_event = i;
		}
		else {
			pItem->SetSelected(FALSE); 
		}
	}
}

// 
void  CPatientDataDlg::OnAddMyEvent() {
	int cnt = m_lay_events->GetCount();
	CMyEventUI * pEvent = new CMyEventUI;
	if (cnt > 0) {
		RECT r = { 1,0,1,1 };
		pEvent->SetBorderSize(r);
	}
	else {
		pEvent->SetBorderSize(1);
		pEvent->SetSelected(TRUE);
		m_selected_event = 0;
	}
	m_lay_events->Add(pEvent);  
}

//
void  CPatientDataDlg::OnDelMyEvent() {
	int cnt = m_lay_events->GetCount();
	if (0 == cnt)
		return;
	if (m_selected_event < 0)
		return;
	if (m_selected_event >= cnt)
		return;

	m_lay_events->RemoveAt(m_selected_event);
	cnt = m_lay_events->GetCount();
	if ( cnt > 0 ) {
		CMyEventUI * pEvent = (CMyEventUI *)m_lay_events->GetItemAt(0);
		pEvent->m_lay_1->SetBorderSize(1);

		if (m_selected_event < cnt) {
			pEvent = (CMyEventUI *)m_lay_events->GetItemAt(m_selected_event);
			pEvent->SetSelected(TRUE);
		}
		else {
			pEvent = (CMyEventUI *)m_lay_events->GetItemAt(cnt-1);
			pEvent->SetSelected(TRUE);
			m_selected_event = cnt - 1;
		}
	}
	else {
		m_selected_event = -1;
	}
}

//
CDuiString  CPatientDataDlg::FormatInt(int nValue) {
	CDuiString str;
	if (nValue > 0) {
		str.Format("%d", nValue);
	}
	return str;
}