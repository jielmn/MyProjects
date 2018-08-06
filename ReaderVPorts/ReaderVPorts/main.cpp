// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"


#ifdef _DEBUG
#pragma comment(lib,"duilibd.lib")
#pragma comment(lib,"LmnCommonXD.lib")
#else
#pragma comment(lib,"duilib.lib")
#pragma comment(lib,"LmnCommonX.lib")
#endif
#pragma comment(lib,"User32.lib")

           

CDuiFrameWnd::CDuiFrameWnd() {

}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_hWnd = this->GetHWND();
	m_btn[0] = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_1"));
	m_btn[1] = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_2"));
	m_btn[2] = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_3"));
	m_btn[3] = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_4"));
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString strName = msg.pSender->GetName();
	if (msg.sType == "click") {
		if (strName == "btn_1") {
			CBusiness::GetInstance()->OpenCloseVPortAsyn(0);
		}
		else if (strName == "btn_2") {
			CBusiness::GetInstance()->OpenCloseVPortAsyn(1);
		}
		else if (strName == "btn_3") {
			CBusiness::GetInstance()->OpenCloseVPortAsyn(2);
		}
		else if (strName == "btn_4") {
			CBusiness::GetInstance()->OpenCloseVPortAsyn(3);
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == UM_NOTIFY_OPEN_VPORT_RET) {
		OnNotifyOpenRet(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void   CDuiFrameWnd::OnNotifyOpenRet(WPARAM wParam, LPARAM lParam) {
	int nIndex = wParam >> 8;
	BOOL bRet = (BOOL)(wParam & 0xFF);
	CLmnSerialPort::PortStatus s = (CLmnSerialPort::PortStatus)lParam;

	CDuiString  strText;
	if ( s == CLmnSerialPort::CLOSE ) {
		strText.Format("关闭虚拟串口com%d", nIndex * 2 + 2);
		m_btn[nIndex]->SetText(strText);
	}
	else {
		strText.Format("打开虚拟串口com%d", nIndex * 2 + 2);
		m_btn[nIndex]->SetText(strText);
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


