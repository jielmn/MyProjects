// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "LmnTelSvr.h"


CDuiFrameWnd::CDuiFrameWnd() {

}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	dwStyle = dwStyle | WS_EX_TOOLWINDOW;
	::SetWindowLong( m_hWnd, GWL_EXSTYLE, dwStyle);

	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	if ( msg.sType == "click" ) {
		if (msg.pSender->GetName() == "myclosebtn") {
			ShowWindow(false);
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_TRAY ) {
		OnTrayMsg(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void CDuiFrameWnd::OnFinalMessage(HWND hWnd) {
	m_nid.hIcon = NULL;
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
}

void  CDuiFrameWnd::AddTray(HICON hIcon) {
	m_nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_nid.hWnd = GetHWND();
	m_nid.uID  = IDI_ICON1;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.uCallbackMessage = UM_TRAY;                         // 自定义的消息名称
	m_nid.hIcon = hIcon;
	STRNCPY(m_nid.szTip, "433接收器", sizeof(m_nid.szTip));     // 信息提示
	Shell_NotifyIcon(NIM_ADD, &m_nid);                        // 在托盘区添加图标
	ShowWindow(false);
}

void  CDuiFrameWnd::OnTrayMsg(WPARAM wParam, LPARAM  lParam) {
	switch (lParam)
	{

	// 右键起来时弹出菜单
	case WM_RBUTTONUP:
	{
		//获取鼠标坐标
		POINT pt; 
		GetCursorPos(&pt);
		//右击后点别地可以清除“右击出来的菜单”
		SetForegroundWindow(m_hWnd);
		//托盘菜单    win32程序使用的是HMENU，如果是MFC程序可以使用CMenu
		HMENU hMenu;
		//生成托盘菜单
		hMenu = CreatePopupMenu();
		//添加菜单,关键在于设置的一个标识符  WM_ONCLOSE 点击后会用到		
		AppendMenu(hMenu, MF_STRING, UM_OTHER,   _T("Show me"));
		AppendMenu(hMenu, MF_STRING, UM_ONCLOSE, _T("Exit"));
		//弹出菜单,并把用户所选菜单项的标识符返回
		int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, m_hWnd, NULL);
		//如果标识符是WM_ONCLOSE则关闭
		if (cmd == UM_ONCLOSE)
		{			
			this->PostMessage(WM_CLOSE);
		}
		else if (cmd == UM_OTHER) {
			this->ShowWindow();
		}
		DestroyMenu(hMenu);
	}
	break;

	// 双击左键的处理
	case WM_LBUTTONDBLCLK:
	{
		// // do something
	}
	break;

	}
}




// integer, text arguments
void PrintStatus(int nCnt, void * args[]) {
	CBusiness::GetInstance()->PrintStatusAsyn();
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HANDLE handle = ::CreateMutex(NULL, FALSE, GLOBAL_LOCK_NAME);//handle为声明的HANDLE类型的全局变量 
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		return 0;
	}

	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

	JTelSvrRegCommand("status", "check status", PrintStatus, 0);

	DWORD  dwPort = 0;
	g_data.m_cfg->GetConfig("telnet port", dwPort, 1207);
	JTelSvrStart((unsigned short)dwPort, 10);


	CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
	duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame->CenterWindow();

	// 设置icon
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	// 添加托盘
	duiFrame->AddTray(hIcon);
	
	duiFrame->ShowModal(false);
	delete duiFrame;

	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}


