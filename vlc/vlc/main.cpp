// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"

CDuiFrameWnd::CDuiFrameWnd() {
	libvlc_inst = 0;
	libvlc_mp = 0;
	libvlc_m = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	WindowImplBase::InitWindow();

	libvlc_inst = libvlc_new(0, NULL);

	HWND screen_hwnd = GetHWND();

	/* Create a new item */
	//m = libvlc_media_new_location (libvlc_inst, "http://mycool.movie.com/test.mov");

	//libvlc_m = libvlc_media_new_path(libvlc_inst, "d:\\test.flv");
	//libvlc_m = libvlc_media_new_path(libvlc_inst, "http://localhost:8080/test/a.flv");
	libvlc_m = libvlc_media_new_location(libvlc_inst, "http://localhost:8080/test/a.flv");

	/* Create a media player playing environement */
	libvlc_mp = libvlc_media_player_new_from_media(libvlc_m);

	/* No need to keep the media now */
	libvlc_media_release(libvlc_m);

	//on windows
	libvlc_media_player_set_hwnd(libvlc_mp, screen_hwnd);

	/* play the media_player */
	int x = libvlc_media_player_play(libvlc_mp);
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}








int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

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

	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}


