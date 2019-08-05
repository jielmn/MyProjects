// win32_1.cpp : 定义应用程序的入口点。
//

#include "Windows.h"
#include "WaitingBarUI.h"

#pragma comment(lib,"User32.lib")





class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile() { return _T("mainframe_wait_progress.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res\\proj_wait_progress_res"); }

	virtual void    Notify(TNotifyUI& msg) {
		if (msg.sType == _T("click"))
		{
			if (msg.pSender->GetName() == _T("btnHello"))
			{
				CWaitingBarUI * pCtrl = (CWaitingBarUI*)m_PaintManager.FindControl("btnHello1");
				if (0 != pCtrl) {
					pCtrl->SetVisible(!pCtrl->IsVisible());
					if (pCtrl->IsVisible()) {
						pCtrl->Start();
					}
					else {
						pCtrl->Stop();
					}
				}
				return;
			}
		}

		WindowImplBase::Notify(msg);
	}

	virtual CControlUI * CreateControl(LPCTSTR pstrClass) {
		if (0 == _stricmp("MyProgress", pstrClass)) {
			return new CWaitingBarUI();
		}
		return 0;
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


