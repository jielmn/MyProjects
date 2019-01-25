// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "flash10a.tlh"
 
CDuiFrameWnd::CDuiFrameWnd() {

}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	 
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
	else if (msg.sType == _T("showactivex"))
	{
		if (name.CompareNoCase(_T("ani_flash")) == 0) {
			CActiveXUI* pActiveX = static_cast<CActiveXUI*>(m_PaintManager.FindControl(_T("ani_flash")));		
			DuiLib::CDuiString path = CPaintManagerUI::GetInstancePath() + GetSkinFolder() + "\\waterdrop.swf";
			IShockwaveFlash* pFlash = NULL;
			pActiveX->GetControl(__uuidof(IShockwaveFlash), (void**)&pFlash);
			if (pFlash != NULL) {
				pFlash->put_WMode(_bstr_t(_T("Transparent"))); 
				pFlash->put_Movie(_bstr_t(path));
				pFlash->DisableLocalSecurity();
				pFlash->put_AllowScriptAccess(L"always");         
				BSTR response;
				pFlash->CallFunction(L"<invoke name=\"setButtonText\" returntype=\"xml\"><arguments><string>Click me!</string></arguments></invoke>", &response);
				pFlash->Release(); 
			}
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
	m_pInstallText->SetText(_T("正在安装中，请稍候..."));

	m_pStepTabLayout->SelectItem(1);	
	SetTimer(GetHWND(), TIMER_ID_PROGRESS, 100, NULL);
}

void  CDuiFrameWnd::InstallFinished() {
	m_pInstallText->SetText(_T("安装完成！"));
	KillTimer(GetHWND(), TIMER_ID_PROGRESS);
	m_pAgainBtn->SetVisible(true);
}

void CDuiFrameWnd::OnMyTimer() {
	int nVal = m_pProgressBar->GetValue();
	CDuiString strPercent;
	strPercent.Format(_T("正在安装（%d%%）"), nVal);
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
	CoInitialize(NULL);

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


