#ifdef _WIN32
#include "vld.h"
#endif

#include "main.h"
#include "LmnThread.h"
#include "Business.h"
#include "resource.h"
#include "LoginDlg.h"

void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	DuiLib::WindowImplBase::InitWindow();
}

void  CDuiFrameWnd::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::WindowImplBase::Notify(msg);
}

DuiLib::CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return DuiLib::WindowImplBase::CreateControl( pstrClass );
}

// 处理自定义信息
LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	int ret = 0;
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();

	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));


	CLoginWnd * loginFrame = new CLoginWnd;
	loginFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	loginFrame->CenterWindow();
	::SendMessage(loginFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(loginFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	// 显示登录界面
	ret = loginFrame->ShowModal();
	delete loginFrame;

	// 如果登录成功
	if (0 == ret) {
		// 主界面	
		CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
		duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		duiFrame->CenterWindow();

		// 设置icon
		::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

		// show model
		duiFrame->ShowModal();
		delete duiFrame;
		duiFrame = 0;
	}

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}