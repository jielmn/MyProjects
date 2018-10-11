// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "httpstack.h"

void OnHttp(  int nError, DWORD dwCode, const char * szData, DWORD dwDataLen, 
	          const char * szHeader, DWORD dwHeaderLen, void * context ) {
	int nCmd = (int)context;
	// 如果是获取更新版本
	if ( nCmd == FETCH_STATION ) {
		if ( nError == 0 ) {
			FILE * fp = fopen(STATION_EXE_NAME, "wb");
			// 如果打开文件失败，则更新失败
			if ( 0 == fp ) {

			}
			fwrite(szData, 1, dwDataLen, fp);
			fclose(fp);
			// 更新成功
		}
	}
}




CDuiFrameWnd::CDuiFrameWnd() {
	m_nState = STATE_OK;

	InitHttpStack(OnHttp);
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	dwStyle = dwStyle | WS_EX_TOOLWINDOW;
	::SetWindowLong( m_hWnd, GWL_EXSTYLE, dwStyle);

	LaunchStation();

	SetTimer(m_hWnd, TIMER_UPDATE_CHECK_ID, TIMER_UPDATE_CHECK_INTERVAL, NULL);
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
	else if (uMsg == WM_TIMER) {
		if (wParam == TIMER_UPDATE_CHECK_ID) {
			OnUpdateCheck();
		}
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
	STRNCPY(m_nid.szTip, "433接收器更新程序", sizeof(m_nid.szTip));     // 信息提示
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
		//AppendMenu(hMenu, MF_STRING, UM_OTHER,   _T("Show me"));
		AppendMenu(hMenu, MF_STRING, UM_ONCLOSE, _T("退出"));
		//弹出菜单,并把用户所选菜单项的标识符返回
		int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, m_hWnd, NULL);
		//如果标识符是WM_ONCLOSE则关闭
		if (cmd == UM_ONCLOSE)
		{			
			this->PostMessage(WM_CLOSE);
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

void   CDuiFrameWnd::OnUpdateCheck() {
	// 如果未知服务器地址
	if ( g_data.m_szServerAddr[0] == '\0' ) {
		return;
	}

	// 如果状态不为OK
	if ( m_nState != STATE_OK ) {
		return;
	}

	// 获取本地版本号

	// 获取服务器版本号

	// 比较版本号，是否需要升级

	// 升级处理
	// 1. 查看433进程是否存在，如果存在，则关闭进程	
	HWND hWnd = ::FindWindow(STATION_CLASS_WINDOW_NAME, 0);
	if (0 != hWnd) {
		::PostMessage(hWnd, WM_CLOSE, 0, 0);

		// 查看进程是否关闭
		hWnd = ::FindWindow(STATION_CLASS_WINDOW_NAME, 0);
		while (hWnd) {
			Sleep(1000);
			hWnd = ::FindWindow(STATION_CLASS_WINDOW_NAME, 0);
		}		
	}

	// 2.重命名原433程序为old.exe
	DeleteFile("old.exe");
	MoveFile(STATION_EXE_NAME, "old.exe");

	// 从服务器上下载最新版本
	std::string strUrl = g_data.m_szServerAddr;
	strUrl += "/";
	strUrl += STATION_EXE_NAME;
	CHttp::GetInstance()->Get(strUrl, (void *)FETCH_STATION);

	m_nState = STATE_FETCH;

	// 如果从服务器获取最新版本失败
	//if ( !bFetch ) {
	//	// 名字还原
	//	MoveFile("old.exe", STATION_EXE_NAME);
	//	LaunchStation();
	//	return;
	//}

	//LaunchStation();
}

void   CDuiFrameWnd::LaunchStation() {
	TCHAR szCommandLine[MAX_PATH];
	memset(szCommandLine, 0, sizeof(szCommandLine));
	lstrcpy(szCommandLine, _T(STATION_EXE_NAME));//要启动的进程
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	BOOL bRet = CreateProcess(
		NULL,					// name of executable module
		szCommandLine,			// command line string
		NULL,					// process attributes
		NULL,					// thread attributes
		FALSE,					// handle inheritance option
		0, //CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE,//0,		UNICODE版本下				// creation flags
		NULL,					// new environment block
		NULL,					// current directory name
		&si,					// startup information
		&pi); 				    // process information
	if (!bRet)
	{
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to launch %s \n", STATION_EXE_NAME);
		this->PostMessage(WM_CLOSE);
	}
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

	char szCurDir[MAX_PATH];
	GetModuleFileName(NULL, szCurDir, MAX_PATH);
	char * pos = strrchr(szCurDir, '\\');
	if (pos) {
		*pos = '\0';
		SetCurrentDirectory(szCurDir);
	}
	
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


