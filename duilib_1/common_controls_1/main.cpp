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
	return 0;   
}

CDuiFrameWnd::CDuiFrameWnd() : m_callback(&m_PaintManager) {

}

CDuiFrameWnd::~CDuiFrameWnd() {

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

	m_ip->SetIP("192.168.0.1");           
	m_hotkey->SetHotKey(65, 2);
	m_filebrowse->SetFileName("D:\\test.txt");

	SetTimer(GetHWND(), TIMER_ID_PROGRESS, 100, NULL);
	WindowImplBase::InitWindow();
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


