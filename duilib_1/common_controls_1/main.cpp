// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"

CControlUI* CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyControl")) {
		return new CMyControlUI;
	}
	return 0;   
}

CDuiFrameWnd::CDuiFrameWnd() : m_callback(&m_PaintManager) {
	m_dwBrowerProcId = 0;
	m_hWndBrowser = 0;
	memset(&m_BrowserRect, 0, sizeof(m_BrowserRect));
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void CDuiFrameWnd::OnFinalMessage(HWND hWnd) {
	if (m_hWndBrowser) {
		::PostMessage(m_hWndBrowser, WM_CLOSE, 0, 0);   
	}
}
            
void  CDuiFrameWnd::InitWindow() {
	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("switch"));
	m_ip = static_cast<CIPAddressExUI*>(m_PaintManager.FindControl("ipaddr"));
	m_edIpAddr = static_cast<CEditUI *>(m_PaintManager.FindControl("edIpAddr"));
	m_hotkey = static_cast<CHotKeyUI*>(m_PaintManager.FindControl("hotkey"));
	m_edHotKey = static_cast<CEditUI *>(m_PaintManager.FindControl("edHotKey"));
	m_progress = static_cast<CProgressUI *>(m_PaintManager.FindControl("ProgressDemo1"));
	m_edFileName = static_cast<CEditUI *>(m_PaintManager.FindControl("edFileName"));
	m_filebrowse = static_cast<CFileBrowseUI *>(m_PaintManager.FindControl("browse1"));
	m_layBrowser = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl("layBrowser"));
	m_List1 = static_cast<CListUI *>(m_PaintManager.FindControl("list_1"));
	m_Combo1 = static_cast<CComboUI *>(m_PaintManager.FindControl("combo_1"));
	m_dragdrop = static_cast<CDragDropUI *>(m_PaintManager.FindControl("cstDragDrop_1"));
	m_mycontrol = static_cast<CMyControlUI *>(m_PaintManager.FindControl("cstMyControl_1"));

	HCURSOR h = LoadCursor(NULL, IDC_SIZEWE);
	m_dragdrop->SetCursor(h);
	m_dragdrop->SetDragDropType(CDragDropUI::DragDropType_WE);

	m_ip->SetIP("192.168.0.1");           
	m_hotkey->SetHotKey(65, 2);
	m_filebrowse->SetFileName("D:\\test.txt");

	CListTextElementUI* pItem = new CListTextElementUI;	
	m_List1->Add(pItem);
	pItem->SetText(0, "White");
	pItem->SetText(1, "Smith");
	pItem->SetText(2, "1092652");
	pItem->SetText(3, "Smith@hotmail.com");	

	pItem = new CListTextElementUI;
	m_List1->Add(pItem);
	pItem->SetText(0, "Black");
	pItem->SetText(1, "Mike");
	pItem->SetText(2, "43987721");
	pItem->SetText(3, "Mike@hotmail.com");

	pItem = new CListTextElementUI;
	m_List1->Add(pItem);
	pItem->SetText(0, "陈");
	pItem->SetText(1, "雨来");
	pItem->SetText(2, "92888112");
	pItem->SetText(3, "Yulai@163.com");
	
	CDuiString  strText;
	CListLabelElementUI * pComboItem;
	for (int i = 0; i < 5; i++) {
		pComboItem = new CListLabelElementUI;
		strText.Format("%d天", i + 1);
		pComboItem->SetText(strText);
		m_Combo1->Add(pComboItem);
	}
	m_Combo1->SelectItem(0);

	SetTimer(GetHWND(), TIMER_ID_PROGRESS, 100, NULL);
	WindowImplBase::InitWindow();
}

void CDuiFrameWnd::MoveBrowser() {	
	RECT rect = m_layBrowser->GetPos();	
	// 比较rect的变化
	if (rect.left != m_BrowserRect.left || rect.right != m_BrowserRect.right || rect.top != m_BrowserRect.top || rect.bottom != m_BrowserRect.bottom) {
		BOOL bRet = MoveWindow(m_hWndBrowser, rect.left + 1 + 4, rect.top + 1, rect.right - rect.left - 2 - 4, rect.bottom - rect.top - 2, TRUE);
		CDuiString  strText;
		strText.Format("m_hWndBrowser=%p, rect=(%d,%d,%d,%d), move=%s \n", m_hWndBrowser, rect.left, rect.right, rect.top, rect.bottom, bRet ? "true" : "false");
		OutputDebugString((const char *)strText);
		SetParent(m_hWndBrowser, this->GetHWND());
		memcpy(&m_BrowserRect, &rect, sizeof(RECT)); 
	}
}

void CDuiFrameWnd::OnWndInit() {	
	PROCESS_INFORMATION pi;
	STARTUPINFO si = { sizeof(si) };

	char szCommandLine[300];
	SNPRINTF(szCommandLine, sizeof(szCommandLine), "%s\\cefclient.exe --hide-controls --hide-caption --url=www.baidu.com", g_data.m_szBrowserDir);
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

	// 如果创建成功
	if (bRet) {
		m_dwBrowerProcId = pi.dwProcessId;
		m_hWndBrowser = GetProcessMainWnd(pi.dwProcessId);
		DWORD dwSleep = 0;

		while (0 == m_hWndBrowser) {
			if (dwSleep >= 10000) {
				break;
			}
			Sleep(100);
			dwSleep += 100;
			m_hWndBrowser = GetProcessMainWnd(pi.dwProcessId);
		}
		if (m_hWndBrowser) {
			MoveBrowser();
		}
	}	
}

LRESULT CDuiFrameWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	return WindowImplBase::OnSize(uMsg, wParam, lParam, bHandled);
}
                
CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	DuiLib::CDialogBuilder builder;
	if (0 == strcmp("Controls_1", pstrClass)) {		
		DuiLib::CControlUI * pUI = builder.Create(_T("Controls_1.xml"), (UINT)0, &m_callback, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("Controls_2", pstrClass)) {
		DuiLib::CControlUI * pUI = builder.Create(_T("Controls_2.xml"), (UINT)0, &m_callback, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("Controls_3", pstrClass)) {
		DuiLib::CControlUI * pUI = builder.Create(_T("Controls_3.xml"), (UINT)0, &m_callback, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("Controls_4", pstrClass)) {
		DuiLib::CControlUI * pUI = builder.Create(_T("Controls_4.xml"), (UINT)0, &m_callback, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("DragDrop", pstrClass)) {
		return new CDragDropUI;
	}
	return WindowImplBase::CreateControl(pstrClass); 
}
    
void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if ( msg.sType == _T("selectchanged") ) {
		if (name == _T("option_1")) {
			m_tabs->SelectItem(0);
		}
		else if (name == _T("option_2")) {
			m_tabs->SelectItem(1);
		}
		else if (name == _T("option_3")) {
			m_tabs->SelectItem(2);
		}
		else if (name == "option_4") {
			m_tabs->SelectItem(3);
		}
	}
	else if (msg.sType == "click") {
		if (name == "btnNonMode") {
			CDuiPopupWnd* pPopWnd = new CDuiPopupWnd();
			pPopWnd->Create(m_hWnd, NULL, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 800, 572);
			pPopWnd->CenterWindow();       
		}
		else if (name == "btnMode") {
			CDuiModeWnd * pModeWnd = new CDuiModeWnd();
			pModeWnd->Create(m_hWnd, NULL, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 800, 572);
			pModeWnd->CenterWindow();
			pModeWnd->ShowModal();       
		}
		else if (name == "btnShowIp") {
			m_edIpAddr->SetText(m_ip->GetIP());  
		}
		else if (name == "btnShowHotKey") {
			WORD  wVirtualKey;
			WORD  wModifiers;
			m_hotkey->GetHotKey(wVirtualKey, wModifiers);

			strText.Format("%s,virtual key=0x%X, modifiers=0x%X",
				(const char *)m_hotkey->GetText(), (DWORD)wVirtualKey, (DWORD)wModifiers);
			m_edHotKey->SetText(strText); 
		}
		else if (name == "btnShowFileName") {
			m_edFileName->SetText(m_filebrowse->GetFileName());
		}
	}  
	else if (msg.sType == "windowinit") {
		OnWndInit();
	}
	else if (msg.sType == "init_splits") {
		RECT rect = m_mycontrol->GetPos();

		RECT r;
		r = rect;
		r.left = m_mycontrol->m_nSplitX - 2;
		r.right = r.left + 2 * 2;

		m_dragdrop->SetVisible(true); 
		m_dragdrop->SetPos(r);		
	}
	WindowImplBase::Notify(msg);
}                                            
         
LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	DuiLib::CDuiString  strText;
	if (uMsg == WM_TIMER) {
		if (wParam == TIMER_ID_PROGRESS) {			
			int v = m_progress->GetValue();
			if (v < 100) {
				strText.Format("%d%%", v+1);
				m_progress->SetValue(v + 1);
				m_progress->SetText(strText);
			}				 
			else {
				strText.Format("%d%%", 0); 
				m_progress->SetValue(0);
				m_progress->SetText(strText);
			}
		}
	}
	else if (uMsg == WM_PAINT) {
		this->PostMessage(UM_AFTER_PAINT);
	}
	else if (uMsg == UM_AFTER_PAINT) {
		MoveBrowser();
	}
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


