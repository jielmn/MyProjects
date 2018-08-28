// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include <time.h>

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "SettingDlg.h"
#include "LmnTelSvr.h"

CDuiFrameWnd::CDuiFrameWnd() :m_callback(&m_PaintManager) {
	m_layReaders = 0;
	memset(m_pUiReaders, 0, sizeof(m_pUiReaders));
	memset(m_pUiReaderSwitch, 0, sizeof(m_pUiReaderSwitch));
	memset(m_pUiAlarms, 0, sizeof(m_pUiAlarms));
	memset(m_pUiReaderTemp, 0, sizeof(m_pUiReaderTemp));
	memset(m_pUiBtnReaderNames, 0, sizeof(m_pUiBtnReaderNames));
	memset(m_pUiEdtReaderNames, 0, sizeof(m_pUiEdtReaderNames));
	memset(m_pUiLayReader, 0, sizeof(m_pUiLayReader));
	m_pUiMyImage = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_hWnd = GetHWND();
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_pUiBtnPatientName = static_cast<CButtonUI*>(m_PaintManager.FindControl(BTN_PATIENT_NAME));
	m_pUiEdtPatientName = static_cast<CEditUI*>(m_PaintManager.FindControl(EDT_PATIENT_NAME));
	m_pUiBtnSex = static_cast<CButtonUI*>(m_PaintManager.FindControl(BTN_PATIENT_SEX));
	m_pUiEdtSex = static_cast<CEditUI*>(m_PaintManager.FindControl(EDT_PATIENT_SEX));
	m_pUiBtnAge = static_cast<CButtonUI*>(m_PaintManager.FindControl(BTN_PATIENT_AGE));
	m_pUiEdtAge = static_cast<CEditUI*>(m_PaintManager.FindControl(EDT_PATIENT_AGE));

	m_pUiBtnPatientName->SetText(g_data.m_szPatietName);
	m_pUiBtnSex->SetText(g_data.m_szPatietSex);
	m_pUiBtnAge->SetText(g_data.m_szPatietAge);
	m_pUiMyImage = static_cast<CMyImageUI*>(m_PaintManager.FindControl(MYIMAGE_NAME));

	m_btnMenu = static_cast<CButtonUI*>(m_PaintManager.FindControl(BTN_MENU_NAME));

	m_layReaders = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(LAYOUT_READERS));
	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		CDialogBuilder builder;
		m_pUiReaders[i] = builder.Create(READER_FILE_NAME, (UINT)0, &m_callback, &m_PaintManager);
		m_pUiReaders[i]->SetTag(i);

		m_pUiIndicator[i] = static_cast<CHorizontalLayoutUI*>(m_pUiReaders[i]->FindControl(MY_FINDCONTROLPROC, CTL_INDICATOR_NAME, 0));
		m_pUiIndicator[i]->SetTag(i);

		m_pUiLayReader[i] = static_cast<CHorizontalLayoutUI*>(m_pUiReaders[i]->FindControl(MY_FINDCONTROLPROC, LAY_READER_NAME, 0));
		m_pUiLayReader[i]->SetTag(i);

		m_pUiReaderSwitch[i] = static_cast<COptionUI*>(m_pUiReaders[i]->FindControl(MY_FINDCONTROLPROC, READER_SWITCH_NAME, 0));
		m_pUiReaderSwitch[i]->SetTag(i);

		m_pUiReaderTemp[i] = static_cast<CLabelUI *>(m_pUiReaders[i]->FindControl(MY_FINDCONTROLPROC, READER_TEMP_NAME, 0));
		m_pUiReaderTemp[i]->SetTag(i);

		m_pUiBtnReaderNames[i] = static_cast<CButtonUI*>(m_pUiReaders[i]->FindControl(MY_FINDCONTROLPROC, BTN_READER_NAME, 0));
		m_pUiBtnReaderNames[i]->SetTag(i);

		m_pUiEdtReaderNames[i] = static_cast<CEditUI*>(m_pUiReaders[i]->FindControl(MY_FINDCONTROLPROC, EDT_READER_NAME, 0));
		m_pUiEdtReaderNames[i]->SetTag(i);

		m_pUiAlarms[i] = static_cast<CAlarmImageUI*>(m_pUiReaders[i]->FindControl(MY_FINDCONTROLPROC, ALARM_IMAGE_NAME, 0));
		m_pUiAlarms[i]->SetTag(i);

		m_pUiIndicator[i]->SetBkColor(g_data.m_argb[i]);
		m_pUiReaderTemp[i]->SetText("--"); 
		m_pUiBtnReaderNames[i]->SetText(g_data.m_szReaderName[i]);
		m_pUiAlarms[i]->FailureAlarm();
		
		m_layReaders->Add(m_pUiReaders[i]); 
	}

	g_edRemark = static_cast<CEditUI*>(m_PaintManager.FindControl(EDIT_REMARK_NAME));
	g_edRemark->SetText("");
	g_edRemark->SetVisible(false);

	OnLayReaderSelected(0);

	InitRand(TRUE, 1);
#if TEST_FLAG
	SetTimer(m_hWnd, TIMER_TEST_ID_1, TIMER_TEST_INTERVAL_1, NULL);
	SetTimer(m_hWnd, TIMER_TEST_ID_2, TIMER_TEST_INTERVAL_2, NULL);
	SetTimer(m_hWnd, TIMER_TEST_ID_3, TIMER_TEST_INTERVAL_3, NULL);
#endif

	WindowImplBase::InitWindow();
}
                 
CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if ( 0 == strcmp(pstrClass, MYIMAGE_CLASS_NAME) ) {
		return new CMyImageUI();
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	if ( msg.sType == "click" ) {
		if (name == BTN_PATIENT_NAME) {
			OnBtnPatientName(msg);
		}
		else if (name == BTN_PATIENT_SEX ) {
			OnBtnPatientSex(msg);
		}
		else if (name == BTN_PATIENT_AGE ) {
			OnBtnPatientAge(msg);
		}
		else if (name == BTN_READER_NAME) {
			OnBtnReaderName(msg);
		}
		else if (name == BTN_MENU_NAME ) {
			OnBtnMenu(msg);
		}
		else if ( name == MYIMAGE_NAME ) {
			POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
			OnMyImageClick(&pt);
		}
	}
	else if (msg.sType == "killfocus") {
		if (name == EDT_PATIENT_NAME) {
			OnKillFocusPatientName(msg);
		}
		else if (name == EDT_PATIENT_SEX) {
			OnKillFocusPatientSex(msg);
		}
		else if (name == EDT_PATIENT_AGE) {
			OnKillFocusPatientAge(msg);
		}
		else if (name == EDT_READER_NAME) {
			OnKillFocusReaderName(msg);
		}
		else if (name == EDIT_REMARK_NAME){
			OnEdtRemarkKillFocus(m_pUiMyImage);
		}
	}
	else if (msg.sType == "menu_setting") {
		OnSetting();
	}
	else if (msg.sType == "menu") {
		if (name == MYIMAGE_NAME) {
			OnMyImageMenu(msg);
		}
	}
	else if (msg.sType == "menu_export_excel") {
		OnExportExcel();
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == WM_LBUTTONDOWN ) {
		OnMyLButtonDown(wParam, lParam);
	}
	else if (uMsg == UM_UPDATE_SCROLL) {
		OnUpdateImageScroll();
	}
	else if (uMsg == WM_TIMER) {
		OnMyTimer(wParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}
   
void  CDuiFrameWnd::OnMyLButtonDown(WPARAM wParam, LPARAM lParam) {
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CControlUI* pCtl = m_PaintManager.FindSubControlByPoint(0,pt);
	if ( 0 == pCtl ) {
		return;
	}

	//// 如果单击按钮
	//if (0 == strcmp(pCtl->GetClass(), DUI_CTR_BUTTON)) {
	//	return;
	//}
	//// 如果单击checkbox
	//else if (0 == strcmp(pCtl->GetClass(), DUI_CTR_OPTION)) {
	//	return;
	//}

	while (pCtl) {
		if (pCtl->GetName() == LAY_READER_NAME) {
			OnLayReaderSelected(pCtl->GetTag());
			break;
		}
		pCtl = pCtl->GetParent();
	}
}
                    
void   CDuiFrameWnd::OnLayReaderSelected(DWORD dwIndex) {
	assert(dwIndex < MAX_READERS_COUNT);
	for ( DWORD i = 0; i < MAX_READERS_COUNT; i++ ) {
		if ( i == dwIndex ) {
			m_pUiLayReader[i]->SetBkColor( 0xFF444444 /*0xFF007ACC*/);
		}
		else {
			m_pUiLayReader[i]->SetBkColor(0);
		}
	}
	m_pUiMyImage->SelectedReader(dwIndex);
	m_pUiMyImage->MyInvalidate();
}
  
void  CDuiFrameWnd::OnBtnPatientName(TNotifyUI& msg) {
	msg.pSender->SetVisible(false);
	m_pUiEdtPatientName->SetText(msg.pSender->GetText());
	m_pUiEdtPatientName->SetVisible(true);
	m_pUiEdtPatientName->SetFocus();
}

void   CDuiFrameWnd::OnBtnPatientSex(TNotifyUI& msg) {
	msg.pSender->SetVisible(false);
	m_pUiEdtSex->SetText(msg.pSender->GetText());
	m_pUiEdtSex->SetVisible(true);
	m_pUiEdtSex->SetFocus();
}

void   CDuiFrameWnd::OnBtnPatientAge(TNotifyUI& msg) {
	msg.pSender->SetVisible(false);
	m_pUiEdtAge->SetText(msg.pSender->GetText());
	m_pUiEdtAge->SetVisible(true);
	m_pUiEdtAge->SetFocus();
}

void   CDuiFrameWnd::OnKillFocusPatientName(TNotifyUI& msg) {
	g_cfg->SetConfig(CFG_PATIENT_NAME, msg.pSender->GetText(),"--");
	g_cfg->Save();

	msg.pSender->SetVisible(false);
	m_pUiBtnPatientName->SetText(msg.pSender->GetText());
	m_pUiBtnPatientName->SetVisible(true);
}

void   CDuiFrameWnd::OnKillFocusPatientSex(TNotifyUI& msg) {
	g_cfg->SetConfig(CFG_PATIENT_SEX, msg.pSender->GetText(), "--");
	g_cfg->Save();

	msg.pSender->SetVisible(false);
	m_pUiBtnSex->SetText(msg.pSender->GetText());
	m_pUiBtnSex->SetVisible(true);
}

void   CDuiFrameWnd::OnKillFocusPatientAge(TNotifyUI& msg) {
	g_cfg->SetConfig(CFG_PATIENT_AGE, msg.pSender->GetText(), "--");
	g_cfg->Save();

	msg.pSender->SetVisible(false);
	m_pUiBtnAge->SetText(msg.pSender->GetText());
	m_pUiBtnAge->SetVisible(true);
}

void   CDuiFrameWnd::OnBtnReaderName(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	msg.pSender->SetVisible(false);
	m_pUiEdtReaderNames[nIndex]->SetText(msg.pSender->GetText());
	m_pUiEdtReaderNames[nIndex]->SetVisible(true);
	m_pUiEdtReaderNames[nIndex]->SetFocus();
}

void  CDuiFrameWnd::OnKillFocusReaderName(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();

	DuiLib::CDuiString  strText;
	strText.Format("%s %d", CFG_READER_NAME, nIndex + 1);
	g_cfg->SetConfig( strText, msg.pSender->GetText(), "--" );
	g_cfg->Save();

	msg.pSender->SetVisible(false);
	m_pUiBtnReaderNames[nIndex]->SetText(msg.pSender->GetText());
	m_pUiBtnReaderNames[nIndex]->SetVisible(true); 
}

void  CDuiFrameWnd::OnBtnMenu(TNotifyUI& msg) {
	RECT r = m_btnMenu->GetPos();
	POINT pt = { r.left, r.bottom };
	CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);

	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

void  CDuiFrameWnd::OnSetting() {
	CDuiString  strText;
	DWORD  dwValue = 0;

	CGlobalData  oldData = g_data;

	CSettingDlg * pSettingDlg = new CSettingDlg;
	pSettingDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pSettingDlg->CenterWindow();
	int ret = pSettingDlg->ShowModal();
	// 如果OK
	if (0 == ret) {
		dwValue = 0;
		g_cfg->SetConfig(CFG_AREA_ID_NAME, g_data.m_dwAreaNo, &dwValue);

		dwValue = DEFAULT_LOWEST_TEMP;
		g_cfg->SetConfig(CFG_SHOWING_LOWEST_TEMP, g_data.m_dwMyImageMinTemp, &dwValue);

		dwValue = DEFAULT_COLLECT_INTERVAL;
		g_cfg->SetConfig(CFG_COLLECT_INTERVAL, g_data.m_dwCollectInterval, &dwValue);

		dwValue = DEFAULT_ALARM_VOICE_SWITCH;
		g_cfg->SetConfig(CFG_ALARM_VOICE_SWITCH, g_data.m_bAlarmVoiceOff, &dwValue);

		for (int i = 0; i < MAX_READERS_COUNT; i++) {
			strText.Format("%s %lu", CFG_LOW_TEMP_ALARM, i + 1);
			dwValue = DEFAULT_LOW_TEMP_ALARM;
			g_cfg->SetConfig(strText, g_data.m_dwLowTempAlarm[i], &dwValue);

			strText.Format("%s %lu", CFG_HIGH_TEMP_ALARM, i + 1);
			dwValue = DEFAULT_HIGH_TEMP_ALARM;
			g_cfg->SetConfig(strText, g_data.m_dwHighTempAlarm[i], &dwValue);

			strText.Format(CFG_BED_NO " %d", i + 1);
			dwValue = i + 1;
			g_cfg->SetConfig(strText, g_data.m_dwBedNo[i], &dwValue);
		}
		g_cfg->Save();
		m_pUiMyImage->MyInvalidate();
	}

	delete pSettingDlg;
}

// 新温度数据达到
void   CDuiFrameWnd::OnNewTempData(int nRederIndex, DWORD dwTemp) {
	CDuiString  strText;

	// 如果开关没有打开，或者病区号为0， 或者床位号为0。丢掉数据
	if ( !g_data.m_bReaderSwitch[nRederIndex] || g_data.m_dwAreaNo == 0 || g_data.m_dwBedNo[nRederIndex] == 0) {
		return;
	}

	double dTemperature = dwTemp / 100.0;
	strText.Format("%.2f", dTemperature);
	m_pUiReaderTemp[nRederIndex]->SetText(strText);

	if (dwTemp >= g_data.m_dwHighTempAlarm[nRederIndex]) {
		m_pUiReaderTemp[nRederIndex]->SetTextColor(0xFFFC235C);
		m_pUiAlarms[nRederIndex]->HighTempAlarm();
	}
	else if (dwTemp <= g_data.m_dwLowTempAlarm[nRederIndex]) {
		m_pUiReaderTemp[nRederIndex]->SetTextColor(0xFF02A5F1);
		m_pUiAlarms[nRederIndex]->LowTempAlarm();
	}
	else {
		m_pUiReaderTemp[nRederIndex]->SetTextColor(0xFFFFFFFF);
		m_pUiAlarms[nRederIndex]->StopAlarm();
	}

	m_pUiMyImage->AddTemp(nRederIndex, dwTemp);

	// 如果报警开关打开
	if (!g_data.m_bAlarmVoiceOff ) {
		if (dwTemp < g_data.m_dwLowTempAlarm[nRederIndex] || dwTemp > g_data.m_dwHighTempAlarm[nRederIndex]) {
			CBusiness::GetInstance()->AlarmAsyn( g_data.m_szAlarmFilePath );
		}
	}
}

void   CDuiFrameWnd::OnUpdateImageScroll() {
	if (!g_data.m_bAutoScroll) {
		return;
	}

	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)m_pUiMyImage->GetParent();
	SIZE tParentScrollPos = pParent->GetScrollPos();
	SIZE tParentScrollRange = pParent->GetScrollRange();
	if (tParentScrollPos.cx != tParentScrollRange.cx) {
		pParent->SetScrollPos(tParentScrollRange);
	}
}

// 定时器
void   CDuiFrameWnd::OnMyTimer(DWORD dwTimerId) {
#if TEST_FLAG
	if (dwTimerId == TIMER_TEST_ID_1) {
		OnNewTempData(0, GetRand(3200, 3500));
	}
	else if (dwTimerId == TIMER_TEST_ID_2) {
		OnNewTempData(1, GetRand(3400, 3700));
	}
	else if (dwTimerId == TIMER_TEST_ID_3) {
		OnNewTempData(2, GetRand(3600, 3900));
	}
#endif
}

//
void   CDuiFrameWnd::OnMyImageMenu(TNotifyUI& msg) {
	POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
	CDuiMenu *pMenu = new CDuiMenu(_T("menu_image.xml"), msg.pSender);
	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

//
void   CDuiFrameWnd::OnExportExcel() {
	m_pUiMyImage->ExportExcel(m_pUiBtnPatientName->GetText());
}

void   CDuiFrameWnd::OnMyImageClick(const POINT * pPoint) {
	m_pUiMyImage->OnMyClick(pPoint);
}
  


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

	DWORD  dwPort = 0;
	g_cfg->GetConfig(CFG_TELNET_PORT, dwPort, 1235);
	JTelSvrStart((unsigned short)dwPort, 10);

	CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
	duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame->CenterWindow();

	// 设置icon
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	duiFrame->ShowModal();
	delete duiFrame;

	g_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}


