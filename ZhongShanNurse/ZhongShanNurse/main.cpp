#include <afx.h>
#include "main.h"
#include "resource.h"
#include "Business.h"
#include "PatientDlg.h"

#define  TEMPERATURE_DATA_INDEX        0
#define  PATIENTS_INDEX                1
#define  NURSES_INDEX                  2
#define  SYNCHRONIZE_INDEX             3
#define  PURE_DATA_INDEX               0
#define  CURVE_INDEX                   1

#define  PATIENTS_INFO_ID_INDEX          0
#define  PATIENTS_INFO_NAME_INDEX        1
#define  PATIENTS_INFO_GENDER_INDEX      2
#define  PATIENTS_INFO_BED_NO_INDEX      3
#define  PATIENTS_INFO_TAGS_INDEX        4
#define  PATIENTS_INFO_STATUS_INDEX      5

DuiLib::CControlUI* CTempDataBuilderCallback::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(PURE_DATA_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(PURE_DATA_FILE), (UINT)0, 0, m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(CURVE_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(CURVE_FILE), (UINT)0, 0, m_PaintManager);
		return pUI;
	}
	return 0;
}





// 数据库连接上
void   CDuiFrameWnd::OnFistDbConnected() {
	// 如果不是第一次
	if (!m_bFisrDbConnect) {
		return;
	}



	m_bFisrDbConnect = FALSE;
}

// 添加病人
void   CDuiFrameWnd::OnAddPatient() {
	CPatientWnd * pPatientDlg = new CPatientWnd;
	pPatientDlg->Create(this->m_hWnd, _T("新增病人信息"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pPatientDlg->CenterWindow();
	int ret = pPatientDlg->ShowModal();

	// 如果添加成功
	if (0 == ret && m_lstPatients) {
		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstPatients->Add(pListElement);

		pListElement->SetText(PATIENTS_INFO_ID_INDEX,     pPatientDlg->m_tPatientInfo.szId );
		pListElement->SetText(PATIENTS_INFO_NAME_INDEX,   pPatientDlg->m_tPatientInfo.szName );
		pListElement->SetText(PATIENTS_INFO_GENDER_INDEX, GetGender(pPatientDlg->m_tPatientInfo.bFemale) );
		pListElement->SetText(PATIENTS_INFO_BED_NO_INDEX, pPatientDlg->m_tPatientInfo.szBedNo );
		pListElement->SetText(PATIENTS_INFO_STATUS_INDEX, GetInHosStatus(pPatientDlg->m_tPatientInfo.bOutHos) );

		pListElement->SetTag( pPatientDlg->m_tPatientInfo.dwId );
	}

	delete pPatientDlg;
}

// 修改病人
void   CDuiFrameWnd::OnModifyPatient() {

}

// 删除病人
void   CDuiFrameWnd::OnDeletePatient() {

}


void CDuiFrameWnd::InitWindow() {
	m_bFisrDbConnect = TRUE;
	g_hWnd = GetHWND();

	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_Callback.m_PaintManager = &m_PaintManager;

	m_lblDbStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl(DATABASE_STATUS_LABEL_ID);
	m_lblBindingReaderStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl(BINDING_READER_STATUS_LABEL_ID);
	m_lblSyncReaderStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl(SYNC_READER_STATUS_LABEL_ID);
	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(_T(TABS_ID)));
	m_sub_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(_T(SUB_TABS_ID)));

	m_lstPatients = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl(_T(PATIENTS_LIST_ID)));


	CZsDatabase::DATABASE_STATUS eDbStatus = CBusiness::GetInstance()->GetDbStatus();
	if (eDbStatus == CZsDatabase::STATUS_OPEN) {
		SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_OK_TEXT);
		OnFistDbConnected();
	}
	else {
		SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_CLOSE_TEXT);
	}

	CZsBindingReader::BINDING_STATUS eBindingReaderStatus = CBusiness::GetInstance()->GetBindingReaderStatus();
	if (eBindingReaderStatus == CZsBindingReader::STATUS_OPEN) {
		SET_CONTROL_TEXT(m_lblBindingReaderStatus, BINDING_READER_STATUS_OK_TEXT);
	}
	else {
		SET_CONTROL_TEXT(m_lblBindingReaderStatus, BINDING_READER_STATUS_CLOSE_TEXT);
	}

	WindowImplBase::InitWindow();
}

DuiLib::CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(TEMPERATURE_DATA_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(TEMPERATURE_DATA_FILE), (UINT)0, &m_Callback, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(PATIENTS_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(PATIENTS_FILE), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(NURSES_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(NURSES_FILE), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(SYNCHRONIZE_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(SYNCHRONIZE_FILE), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	return 0;
}

void CDuiFrameWnd::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if (msg.sType == _T("selectchanged"))
	{
		if (name == _T(TEMPERATURE_DATA_OPTION_ID)) {
			if (m_tabs)
				m_tabs->SelectItem(TEMPERATURE_DATA_INDEX);
		}
		else if (name == _T(PATIENTS_OPTION_ID)) {
			if (m_tabs)
				if (m_tabs)m_tabs->SelectItem(PATIENTS_INDEX);
		}
		else if (name == _T(NURSES_OPTION_ID)) {
			if (m_tabs)
				m_tabs->SelectItem(NURSES_INDEX);
		}
		else if (name == SYNCHRONIZE_OPTION_ID) {
			if (m_tabs)
				m_tabs->SelectItem(SYNCHRONIZE_INDEX);
		}
		else if (name == PURE_DATA_OPTION_ID) {
			if (m_sub_tabs)
				m_sub_tabs->SelectItem(PURE_DATA_INDEX);
		}
		else if (name == CURVE_OPTION_ID) {
			if (m_sub_tabs)
				m_sub_tabs->SelectItem(CURVE_INDEX);
		}
	}
	else if (msg.sType == "click") {
		if (name == ADD_PATIENT_BUTTON_ID) {
			OnAddPatient();
		}
		else if (name == MOD_PATIENT_BUTTON_ID) {
			OnModifyPatient();
		}
		else if (name == DEL_PATIENT_BUTTON_ID) {
			OnDeletePatient();
		}
		else if (name == IMPORT_PATIENT_BUTTON_ID) {

		}
		else if (name == BINDING_PATIENT_BUTTON_ID) {

		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int nError = 0;
	//char buf[8192];

	if (uMsg == UM_SHOW_DB_STATUS) {
		CZsDatabase::DATABASE_STATUS eStatus = (CZsDatabase::DATABASE_STATUS)wParam;
		if (eStatus == CZsDatabase::STATUS_OPEN) {
			SET_CONTROL_COLOR(m_lblDbStatus, COLOR_OK);
			SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_OK_TEXT);
			OnFistDbConnected();
		}
		else {
			SET_CONTROL_COLOR(m_lblDbStatus, COLOR_ERROR);
			SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_CLOSE_TEXT);
		}
		return 0;
	}
	else if (uMsg == UM_SHOW_BINDING_READER_STATUS) {
		CZsBindingReader::BINDING_STATUS eStatus = (CZsBindingReader::BINDING_STATUS)wParam;
		if (eStatus == CZsBindingReader::STATUS_OPEN) {
			SET_CONTROL_COLOR(m_lblBindingReaderStatus, COLOR_OK);
			SET_CONTROL_TEXT(m_lblBindingReaderStatus, BINDING_READER_STATUS_OK_TEXT);
		}
		else {
			SET_CONTROL_COLOR(m_lblBindingReaderStatus, COLOR_ERROR);
			SET_CONTROL_TEXT(m_lblBindingReaderStatus, BINDING_READER_STATUS_CLOSE_TEXT);
		}
		return 0;
	}

	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	int ret = 0;
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->sigInit.emit(&ret);

	DuiLib::CPaintManagerUI::SetInstance(hInstance);

	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	// 主界面	
	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();

	// 设置icon
	::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);	

	// show model
	duiFrame.ShowModal();

	CBusiness::GetInstance()->sigDeinit.emit(&ret);
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}