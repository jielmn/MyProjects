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

	m_layReaders = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(LAYOUT_READERS));
	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		CDialogBuilder builder;
		m_pUiReaders[i] = builder.Create(READER_FILE_NAME, (UINT)0, &m_callback, &m_PaintManager);
		m_pUiReaders[i]->SetTag(i);

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

		m_pUiReaderTemp[i]->SetText("--"); 
		m_pUiBtnReaderNames[i]->SetText(g_data.m_szReaderName[i]);
		m_pUiAlarms[i]->FailureAlarm();
		
		m_layReaders->Add(m_pUiReaders[i]); 
	}

	OnLayReaderSelected(0);

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

	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == WM_LBUTTONDOWN ) {
		OnMyLButtonDown(wParam, lParam);
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
			m_pUiLayReader[i]->SetBkColor(0xFF007ACC);
		}
		else {
			m_pUiLayReader[i]->SetBkColor(0);
		}
	}
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




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

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


