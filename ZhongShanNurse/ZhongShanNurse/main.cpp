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

	CBusiness::GetInstance()->GetAllPatientsAsyn();

	m_bFisrDbConnect = FALSE;
}

void  CDuiFrameWnd::AddPatientItem2List(DuiLib::CListTextElementUI* pListElement, const PatientInfo * pPatient, BOOL bSetTag /*= TRUE */) {
	pListElement->SetText(PATIENTS_INFO_ID_INDEX,     pPatient->szId);
	pListElement->SetText(PATIENTS_INFO_NAME_INDEX,   pPatient->szName);
	pListElement->SetText(PATIENTS_INFO_GENDER_INDEX, GetGender(pPatient->bFemale));
	pListElement->SetText(PATIENTS_INFO_BED_NO_INDEX, pPatient->szBedNo);
	pListElement->SetText(PATIENTS_INFO_STATUS_INDEX, GetInHosStatus(pPatient->bOutHos));
	if (bSetTag)
		pListElement->SetTag(pPatient->dwId);
}

void   CDuiFrameWnd::OnGetAllPatientsRet( const std::vector<PatientInfo*> & vRet ) {
	std::vector<PatientInfo*>::const_iterator it;
	for (it = vRet.begin(); it != vRet.end(); it++) {
		PatientInfo*  pPatient = *it;

		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstPatients->Add(pListElement);

		AddPatientItem2List(pListElement, pPatient);
	}
}

// 删除病人ret
void   CDuiFrameWnd::OnDeletePatientRet(int ret, DWORD dwId) {
	if (0 == ret)
	{
		assert(m_lstPatients);
		int nSelIndex = m_lstPatients->GetCurSel();
		DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(nSelIndex);
		assert(pListElement);

		// 如果list焦点没有变化
		if ( pListElement->GetTag() == dwId ) {
			m_lstPatients->RemoveAt(nSelIndex);
		}
		// 遍历整个list
		else {
			int cnt = m_lstPatients->GetCount();
			for (int i = 0; i < cnt; i++) {
				pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(i);
				assert(pListElement);
				if (pListElement->GetTag() == dwId) {
					m_lstPatients->RemoveAt(nSelIndex);
					break;
				}
			}
		}
		
		::MessageBox(this->GetHWND(), "删除病人成功", "删除病人", 0);
	}
	else {
		::MessageBox(this->GetHWND(), GetErrDescription(ret), "删除病人", 0);
	}
}

// 导入病人ret
void  CDuiFrameWnd::OnImportPatientsRet(int ret, const std::vector<PatientInfo*> & vRet) {
	if (0 == m_lstPatients) {
		return;
	}

	std::vector<PatientInfo*>::const_iterator it;
	for (it = vRet.begin(); it != vRet.end(); it++) {
		PatientInfo*  pPatient = *it;

		if (pPatient->bToUpdated) {
			int nCnt = m_lstPatients->GetCount();
			for (int i = 0; i < nCnt; i++) {
				DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(i);
				if (pListElement->GetTag() == pPatient->dwId) {
					AddPatientItem2List(pListElement, pPatient, FALSE);
				}
			}
		}
		else {
			DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
			m_lstPatients->Add(pListElement);

			AddPatientItem2List(pListElement, pPatient);
		}		
	}
}






// 添加病人
void   CDuiFrameWnd::OnAddPatient() {
	CPatientWnd * pPatientDlg = new CPatientWnd;
	if (0 == pPatientDlg) {
		return;
	}

	pPatientDlg->Create(this->m_hWnd, _T("新增病人信息"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pPatientDlg->CenterWindow();
	int ret = pPatientDlg->ShowModal();

	// 如果添加成功
	if (0 == ret && m_lstPatients) {
		DuiLib::CListTextElementUI* pListElement = new DuiLib::CListTextElementUI;
		m_lstPatients->Add(pListElement);

		AddPatientItem2List(pListElement, &pPatientDlg->m_tPatientInfo);
	}

	delete pPatientDlg;
}

// 修改病人
void   CDuiFrameWnd::OnModifyPatient() {
	if (0 == m_lstPatients) {
		return;
	}

	int nSelIndex = m_lstPatients->GetCurSel();
	DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(nSelIndex);
	if (0 == pListElement) {
		return;
	}

	CPatientWnd * pPatientDlg = new CPatientWnd(FALSE);
	if (0 == pPatientDlg) {
		return;
	}
	memset(&pPatientDlg->m_tPatientInfo, 0, sizeof(PatientInfo));

	strncpy_s(pPatientDlg->m_tPatientInfo.szId,    pListElement->GetText(PATIENTS_INFO_ID_INDEX),     sizeof(pPatientDlg->m_tPatientInfo.szId));
	strncpy_s(pPatientDlg->m_tPatientInfo.szName,  pListElement->GetText(PATIENTS_INFO_NAME_INDEX),   sizeof(pPatientDlg->m_tPatientInfo.szName));
	strncpy_s(pPatientDlg->m_tPatientInfo.szBedNo, pListElement->GetText(PATIENTS_INFO_BED_NO_INDEX), sizeof(pPatientDlg->m_tPatientInfo.szBedNo));
	pPatientDlg->m_tPatientInfo.bFemale = GetGender(pListElement->GetText(PATIENTS_INFO_GENDER_INDEX));
	pPatientDlg->m_tPatientInfo.bOutHos = GetInHosStatus(pListElement->GetText(PATIENTS_INFO_STATUS_INDEX));
	pPatientDlg->m_tPatientInfo.dwId = (DWORD)pListElement->GetTag();
	
	pPatientDlg->Create(this->m_hWnd, _T("新增病人信息"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pPatientDlg->CenterWindow();
	int ret = pPatientDlg->ShowModal();

	// 如果修改成功
	if (0 == ret && m_lstPatients) {
		AddPatientItem2List(pListElement, &pPatientDlg->m_tPatientInfo, FALSE);
	}

	delete pPatientDlg;
}

// 删除病人
void   CDuiFrameWnd::OnDeletePatient() {
	if (0 == m_lstPatients) {
		return;
	}

	int nSelIndex = m_lstPatients->GetCurSel();
	DuiLib::CListTextElementUI* pListElement = (DuiLib::CListTextElementUI*)m_lstPatients->GetItemAt(nSelIndex);
	if (0 == pListElement) {
		return;
	}

	if ( IDYES == ::MessageBox(GetHWND(), "你确定要删除该病人信息吗？", "删除病人", MB_YESNO | MB_DEFBUTTON2) ) {
		CBusiness::GetInstance()->DeletePatientAsyn( (DWORD)pListElement->GetTag() );
	}
}

// 导入病人
void   CDuiFrameWnd::OnImportPatients() {
	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名  
	ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
	ofn.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
	ofn.lpstrFilter = TEXT("Excel Flie(*.xls)\0*.xls\0所有文件\0*.*\0\0");//设置过滤  
	ofn.nFilterIndex = 1;//过滤器索引  
	ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL  
	ofn.nMaxFile = sizeof(strFilename);//缓冲区长度  
									   //ofn.lpstrInitialDir = NULL;//初始目录为默认  
	ofn.lpstrInitialDir = CPaintManagerUI::GetInstancePath();
	ofn.lpstrTitle = TEXT("请选择一个文件");//使用系统默认标题留空即可  
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项  
	if (GetOpenFileName(&ofn))
	{
		CBusiness::GetInstance()->ImportPatientsAsyn( strFilename );
	}
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
			OnImportPatients();
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
	else if (uMsg == UM_GET_ALL_PATIENTS_RET) {
		nError = wParam;
		std::vector<PatientInfo * > * pvRet = (std::vector<PatientInfo *> *)lParam;

		if ( (0 == nError) && pvRet )
			OnGetAllPatientsRet(*pvRet);

		if (pvRet) {
			ClearVector(*pvRet);
		}
		return 0;
	}
	else if (uMsg == UM_DELETE_PATIENT_RET) {
		nError = wParam;
		DWORD dwId = (DWORD)lParam;

		OnDeletePatientRet( nError, dwId );
		return 0;
	}
	else if (uMsg == UM_NOTIFY_IMPORT_PATIENTS_RET) {
		nError = wParam;
		std::vector<PatientInfo*> * pvRet = (std::vector<PatientInfo*> *)lParam;

		if ( 0 == nError ) {
			if (pvRet) {
				OnImportPatientsRet(nError, *pvRet);
			}			
		}
		// 部分失败
		else if ( ZS_ERR_PARTIALLY_FAILED_TO_IMPORT_EXCEL == nError) {
			if (pvRet) {
				OnImportPatientsRet(nError, *pvRet);
			}
			::MessageBox(this->GetHWND(), GetErrDescription(nError), "导入病人信息", 0);
		}
		// 完全失败
		else {
			::MessageBox(this->GetHWND(), GetErrDescription(nError), "导入病人信息", 0);
		}

		if (pvRet) {
			ClearVector(*pvRet);
			delete pvRet;
		}
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