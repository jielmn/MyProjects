// win32_1.cpp : ����Ӧ�ó������ڵ㡣
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
 
CDuiFrameWnd::CDuiFrameWnd() {

}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	m_pStepTabLayout = static_cast<CAnimationTabLayoutUI*>(m_PaintManager.FindControl(_T("stepTabLayout")));
	m_pInstallText = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("textProgress")));
	m_pProgressBar = static_cast<CProgressUI*>(m_PaintManager.FindControl(_T("install")));
	m_pAgainBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnAgain")));	           

	WindowImplBase::InitWindow();   
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString  name = msg.pSender->GetName();

	if (msg.sType == _T("click")) {
		if (name == "BtnOneClick") {
			InstallStart();
			return;
		}
		else if (name == "btnAgain") {
			m_pStepTabLayout->SelectItem(0);
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_TIMER) {
		if ( wParam == TIMER_ID_PROGRESS) {
			OnMyTimer();  
		}
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}


void CDuiFrameWnd::InstallStart() {
	m_pAgainBtn->SetVisible(false);
	m_pProgressBar->SetValue(0);
	m_pInstallText->SetText(_T("���ڰ�װ�У����Ժ�..."));

	m_pStepTabLayout->SelectItem(1);	
	SetTimer(GetHWND(), TIMER_ID_PROGRESS, 100, NULL);
}

void  CDuiFrameWnd::InstallFinished() {
	m_pInstallText->SetText(_T("��װ��ɣ�"));
	KillTimer(GetHWND(), TIMER_ID_PROGRESS);
	m_pAgainBtn->SetVisible(true);
}

void CDuiFrameWnd::OnMyTimer() {
	int nVal = m_pProgressBar->GetValue();
	CDuiString strPercent;
	strPercent.Format(_T("���ڰ�װ��%d%%��"), nVal);
	m_pInstallText->SetText(strPercent);
	if (nVal > 95)
	{
		m_pProgressBar->SetValue(100);
		InstallFinished();
	}
	else
	{
		nVal += 5;
		m_pProgressBar->SetValue(nVal);
	}
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

	// ����icon
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


