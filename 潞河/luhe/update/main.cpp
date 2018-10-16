// win32_1.cpp : ����Ӧ�ó������ڵ㡣
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
	// ����ǻ�ȡ���°汾
	if ( nCmd == FETCH_STATION ) {
		// ��ȡ���°汾�ɹ�
		if ( nError == 0 ) {
			// ��ԭ433�������
			DeleteFile("old.exe");
			MoveFile(STATION_EXE_NAME, "old.exe");

			FILE * fp = fopen(STATION_EXE_NAME, "wb");
			// ������ļ�ʧ�ܣ������ʧ��
			if ( 0 == fp ) {
				MoveFile("old.exe", STATION_EXE_NAME);
				::PostMessage(g_data.m_hWnd, UM_UPDATE_RET, 1, 0);								
				return;
			}
			fwrite(szData, 1, dwDataLen, fp);
			fclose(fp);
			// ���³ɹ�
			::PostMessage(g_data.m_hWnd, UM_UPDATE_RET, 0, 0);
		}
		// ��ȡ���°汾ʧ��
		else {
			::PostMessage(g_data.m_hWnd, UM_UPDATE_RET, 2, 0);
		}
	}
	else if (nCmd == FETCH_VERSION) {
		// ��ȡ�汾�ųɹ�
		if (nError == 0) {
			char * pVersion = new char[dwDataLen + 1];
			memcpy(pVersion, szData, dwDataLen);
			pVersion[dwDataLen] = '\0';
			::PostMessage(g_data.m_hWnd, UM_VERSION_RET, 0, (LPARAM)pVersion);
		}
		// ��ȡ�汾��ʧ��
		else {
			::PostMessage(g_data.m_hWnd, UM_VERSION_RET, -1, 0);
		}
	}
}




CDuiFrameWnd::CDuiFrameWnd() {
	m_nState = STATE_OK;
	memset(m_szRemoteVersion, 0, sizeof(m_szRemoteVersion));
	InitHttpStack(OnHttp);
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = this->GetHWND();

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
	else if (uMsg == UM_UPDATE_RET) {
		OnUpdateRet(wParam, lParam);
	}
	else if (uMsg == UM_VERSION_RET) {
		OnVersionRet(wParam, lParam);
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
	m_nid.uCallbackMessage = UM_TRAY;                         // �Զ������Ϣ����
	m_nid.hIcon = hIcon;
	STRNCPY(m_nid.szTip, "433���������³���", sizeof(m_nid.szTip));     // ��Ϣ��ʾ
	Shell_NotifyIcon(NIM_ADD, &m_nid);                        // �����������ͼ��
	ShowWindow(false);
}

void  CDuiFrameWnd::OnTrayMsg(WPARAM wParam, LPARAM  lParam) {
	switch (lParam)
	{

	// �Ҽ�����ʱ�����˵�
	case WM_RBUTTONUP:
	{
		//��ȡ�������
		POINT pt; 
		GetCursorPos(&pt);
		//�һ�����ؿ���������һ������Ĳ˵���
		SetForegroundWindow(m_hWnd);
		//���̲˵�    win32����ʹ�õ���HMENU�������MFC�������ʹ��CMenu
		HMENU hMenu;
		//�������̲˵�
		hMenu = CreatePopupMenu();
		//��Ӳ˵�,�ؼ��������õ�һ����ʶ��  WM_ONCLOSE �������õ�		
		//AppendMenu(hMenu, MF_STRING, UM_OTHER,   _T("Show me"));
		AppendMenu(hMenu, MF_STRING, UM_ONCLOSE, _T("�˳�"));
		//�����˵�,�����û���ѡ�˵���ı�ʶ������
		int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, m_hWnd, NULL);
		//�����ʶ����WM_ONCLOSE��ر�
		if (cmd == UM_ONCLOSE)
		{			
			this->PostMessage(WM_CLOSE);
		}
		DestroyMenu(hMenu);
	}
	break;

	// ˫������Ĵ���
	case WM_LBUTTONDBLCLK:
	{
		// // do something
	}
	break;

	}
}

void   CDuiFrameWnd::OnUpdateCheck() {
	// ���δ֪��������ַ
	if ( g_data.m_szServerAddr[0] == '\0' ) {
		return;
	}

	// ���״̬��ΪOK
	if ( m_nState != STATE_OK ) {
		return;
	}

	// ��ȡ�������汾��
	std::string strUrl = g_data.m_szServerAddr;
	strUrl += "/update/version.txt";
	CHttp::GetInstance()->Get(strUrl, (void *)FETCH_VERSION);
	m_nState = STATE_GET_VERSION;
}

void   CDuiFrameWnd::LaunchStation() {
	TCHAR szCommandLine[MAX_PATH];
	memset(szCommandLine, 0, sizeof(szCommandLine));
	lstrcpy(szCommandLine, _T(STATION_EXE_NAME));//Ҫ�����Ľ���
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	BOOL bRet = CreateProcess(
		NULL,					// name of executable module
		szCommandLine,			// command line string
		NULL,					// process attributes
		NULL,					// thread attributes
		FALSE,					// handle inheritance option
		0, //CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE,//0,		UNICODE�汾��				// creation flags
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

void   CDuiFrameWnd::OnUpdateRet(WPARAM wParam, LPARAM lParam) {
	int nRet = wParam;
	if ( 0 == nRet ) {
		g_data.m_cfg->SetConfig("station version", m_szRemoteVersion);
		g_data.m_cfg->Save();
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "update OK \n");
	}

	LaunchStation();

	m_nState = STATE_OK;
}

void  CDuiFrameWnd::OnVersionRet(WPARAM wParam, LPARAM lParam) {
	int nRet = wParam;
	char * pVersion = (char *)lParam;

	// ��ȡ�汾�ųɹ�
	if (0 == nRet) {
		assert(pVersion);

		char szVersion[256];
		g_data.m_cfg->GetConfig("station version", szVersion, sizeof(szVersion), "");
		// ����汾����ͬ
		if ( 0 == StrICmp(szVersion, pVersion) ) {
			m_nState = STATE_OK;
		}
		// �汾�Ų�ͬ
		else {
			g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "remote version is different from local. start updating \n");
			STRNCPY(m_szRemoteVersion, pVersion, sizeof(m_szRemoteVersion));

			// 1. �鿴433�����Ƿ���ڣ�������ڣ���رս���	
			HWND hWnd = ::FindWindow(STATION_CLASS_WINDOW_NAME, 0);
			if (0 != hWnd) {
				::PostMessage(hWnd, WM_CLOSE, 0, 0);

				// �鿴�����Ƿ�ر�
				hWnd = ::FindWindow(STATION_CLASS_WINDOW_NAME, 0);
				while (hWnd) {
					Sleep(1000);
					hWnd = ::FindWindow(STATION_CLASS_WINDOW_NAME, 0);
				}
			}

			// 2. �ӷ��������������°汾
			std::string strUrl = g_data.m_szServerAddr;
			strUrl += "/update/";
			strUrl += STATION_EXE_NAME;
			CHttp::GetInstance()->Get(strUrl, (void *)FETCH_STATION);
			m_nState = STATE_FETCH;
		}
	}
	// ��ȡ�汾��ʧ��
	else {
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to get remote version \n");
		m_nState = STATE_OK;
	}

	if (pVersion) {
		delete[] pVersion;
	}
}






int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HANDLE handle = ::CreateMutex(NULL, FALSE, GLOBAL_LOCK_NAME);//handleΪ������HANDLE���͵�ȫ�ֱ��� 
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

	// ����icon
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	// �������
	duiFrame->AddTray(hIcon);
	
	duiFrame->ShowModal(false);
	delete duiFrame;

	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}


