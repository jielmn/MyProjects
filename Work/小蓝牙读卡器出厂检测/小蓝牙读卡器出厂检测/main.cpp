// win32_1.cpp : ����Ӧ�ó������ڵ㡣
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "UIMenu.h"
#include "LmnSerialPort.h"

#include "business.h"
#include "resource.h"

                                                             
CDuiFrameWnd::CDuiFrameWnd() {

}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = GetHWND();

	m_lblStatus = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblStatus"));

	CheckDevices();
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString  strName;
	strName = msg.pSender->GetName();

	if ( msg.sType == "click" ) {
		if (strName == "btnSetting") {
			RECT r = msg.pSender->GetPos();
			POINT point = { r.left, r.bottom };

			CMenuWnd* pMenu = new CMenuWnd(m_hWnd, msg.pSender);
			// CDuiPoint point = msg.ptMouse;
			ClientToScreen(m_hWnd, &point); 

			//ʹ����Դ��ʽ
			//STRINGorID xml(IDR_XML1);
			//pMenu->Init(NULL, xml, "xml", point);  

			//ʹ���ļ���ʽ
			STRINGorID xml("menusetting.xml"); 
			pMenu->Init(NULL, xml, 0, point);                          
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_COM_STATUS ) {
		int nCom  = (int)wParam;
		BOOL bRet = (BOOL)lParam;
		CDuiString strText;

		if (nCom > 0) {
			if (bRet) {
				strText.Format("�򿪴���com%d�ɹ�", nCom);
				
			}
			else {
				strText.Format("�򿪴���com%dʧ�ܣ�", nCom);
			}
		}
		else if (nCom == 0) {
			strText.Format("û�д�����ģ�鴮��");
		}
		else {
			strText.Format("�ҵ��������ģ�飬��ȷ��ֻ��һ��");
		}

		m_lblStatus->SetText(strText);
	}
	else if (WM_DEVICECHANGE == uMsg) {
		CheckDevices();
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void CDuiFrameWnd::CheckDevices() {	
	CBusiness::GetInstance()->CheckDevicesAsyn();
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

	// ����icon
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


