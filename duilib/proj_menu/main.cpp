// win32_1.cpp : 定义应用程序的入口点。
//

#include "Windows.h"
#include "UIlib.h"
using namespace DuiLib;
#include "MenuUI.h"

#pragma comment(lib,"User32.lib")



class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile() { return _T("mainframe_menu.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res\\proj_menu_res"); }

	virtual void    Notify(TNotifyUI& msg) {
		if (msg.sType == _T("click"))
		{
			if (msg.pSender->GetName() == _T("btnHello"))
			{
				POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
				CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);

				pMenu->Init(*this, pt);
				pMenu->ShowWindow(TRUE);
				return;
			}
			else {
				CDuiString name = msg.pSender->GetName();
				int a = 100;
				//::MessageBox(0, "click open", "", 0);
			}

		}
		else if (msg.sType == "menu_Open") {
			::MessageBox( 0, "click open", "", 0);
		}
		else if (msg.sType == "menu_Mark") {
			::MessageBox(0, "click MARK", "", 0);
		}
		else if (msg.sType == "menu_Delete") {
			::MessageBox(0, "click DELETE", "", 0);
		}

		WindowImplBase::Notify(msg);
	}

};

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


