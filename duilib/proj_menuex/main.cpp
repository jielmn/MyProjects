#include "main.h"
#include "UICrack.h"
#include "UIMenu.h"
#include "resource.h"

#pragma comment(lib,"User32.lib")

void   CDuiFrameWnd::Notify(TNotifyUI& msg) {

	if ( strcmp(msg.sType, kClick) == 0 ) {
		DuiLib::CDuiString name = msg.pSender->GetName();
		if ( strcmp(msg.pSender->GetName(), _T("btnFile")) == 0 )
		{
			CMenuWnd* pMenu = new CMenuWnd(m_hWnd, msg.pSender);
			CDuiPoint point = msg.ptMouse;
			ClientToScreen(m_hWnd, &point);
			STRINGorID xml(IDR_XML1);
			pMenu->Init(NULL, xml, "xml", point);                      
		}
	}
	else if (0 == strcmp(msg.sType, "munu_test_3")) {
		::MessageBox(0, "click menu test 3", "≤Àµ•≤‚ ‘", 0);
	}
	else if (0 == strcmp(msg.sType, "munu_test_4")) {
		::MessageBox(0, "click menu test 4", "≤Àµ•≤‚ ‘", 0);  
	}
	WindowImplBase::Notify(msg); 
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}





int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);

	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();
	duiFrame.ShowModal();
	return 0; 
}