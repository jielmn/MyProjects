#include "main.h"
#include "UIMenu.h"
#include "resource.h"

#pragma comment(lib,"User32.lib")

void   CDuiFrameWnd::Notify(TNotifyUI& msg) {

	if ( strcmp(msg.sType, "click") == 0 ) {
		if ( strcmp(msg.pSender->GetName(), _T("btnFile")) == 0 )
		{
			RECT r = m_PaintManager.FindControl(msg.pSender->GetName())->GetPos();
			POINT point = { r.left, r.bottom };

			CMenuWnd* pMenu = new CMenuWnd(m_hWnd, msg.pSender);
			// CDuiPoint point = msg.ptMouse;
			ClientToScreen(m_hWnd, &point);

			//使用资源方式
			//STRINGorID xml(IDR_XML1);
			//pMenu->Init(NULL, xml, "xml", point);

			//使用文件方式
			STRINGorID xml("menutest.xml");
			pMenu->Init(NULL, xml, 0, point);        
		}
		else if ( strcmp(msg.pSender->GetName(), _T("btnEdit")) == 0 ) {
			RECT r = m_PaintManager.FindControl(msg.pSender->GetName())->GetPos();
			POINT point = { r.left, r.bottom };

			CMenuWnd* pMenu = new CMenuWnd(m_hWnd, msg.pSender);
			// CDuiPoint point = msg.ptMouse;
			ClientToScreen(m_hWnd, &point);

			//使用资源方式
			//STRINGorID xml(IDR_XML1);
			//pMenu->Init(NULL, xml, "xml", point);

			//使用文件方式
			STRINGorID xml("menutest_1.xml");
			pMenu->Init(NULL, xml, 0, point); 
		}
	}
	else if (0 == strcmp(msg.sType, "munu_test_3")) {
		::MessageBox(0, "click menu test 3", "菜单测试", 0);
	}
	else if (0 == strcmp(msg.sType, "munu_test_4")) {
		::MessageBox(0, "click menu test 4", "菜单测试", 0);  
	}
	else if (0 == strcmp(msg.sType, "munu_test_e")) {
		::MessageBox(0, "click menu test e", "菜单测试", 0);
	}
	else if (0 == strcmp(msg.sType, "munu_test_f")) {
		::MessageBox(0, "click menu test f", "菜单测试", 0);
	}
	else if (0 == strcmp(msg.sType, "munu_hello")) {
		::MessageBox(0, "click menu hello", "菜单测试", 0);
	}
	else if (0 == strcmp(msg.sType, "munu_world")) {
		::MessageBox(0, "click menu world", "菜单测试", 0);
	}
	else if (0 == strcmp(msg.sType, "munu_test_5")) {
		::MessageBox(0, "click menu 5", "菜单测试", 0);
	}
	else if (0 == strcmp(msg.sType, "munu_test_c")) {
		::MessageBox(0, "click menu c", "菜单测试", 0);
	}
	else if (0 == strcmp(msg.sType, "munu_test_d")) {
		::MessageBox(0, "click menu d", "菜单测试", 0);
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