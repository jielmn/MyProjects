// win32_1.cpp : 定义应用程序的入口点。
//

#include "Windows.h"
#include "UIlib.h"
using namespace DuiLib;

#pragma comment(lib,"User32.lib")



class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile() { return _T("mainframe_blank.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }

	virtual void    Notify(TNotifyUI& msg) {
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


