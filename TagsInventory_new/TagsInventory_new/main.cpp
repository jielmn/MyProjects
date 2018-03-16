// win32_1.cpp : ����Ӧ�ó������ڵ㡣
//
#include <afx.h>
#include "login.h"
#include "main.h"
#include "resource.h"
#include "Business.h"


void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	int ret = 0;
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->sigInit.emit(&ret);

	CPaintManagerUI::SetInstance(hInstance);	
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	
	CLoginWnd loginFrame;
	loginFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	loginFrame.CenterWindow();
	g_hWnd = loginFrame.GetHWND();
	::SendMessage(loginFrame.GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(loginFrame.GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	// �������ݿ�
	CBusiness::GetInstance()->ReconnectDatabaseAsyn();
	// ����Reader
	CBusiness::GetInstance()->ReconnectReaderAsyn();

	// ��ʾ��¼����
	loginFrame.ShowModal();

	// �����¼�ɹ�
	if ( CBusiness::GetInstance()->IfLogined() ) {
		// ������	
		CDuiFrameWnd duiFrame;
		duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		duiFrame.CenterWindow();
		g_hWnd = duiFrame.GetHWND();
		::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		duiFrame.ShowModal();
	}

	CBusiness::GetInstance()->sigDeinit.emit(&ret);
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}


