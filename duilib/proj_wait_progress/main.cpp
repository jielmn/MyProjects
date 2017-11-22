// win32_1.cpp : 定义应用程序的入口点。
//

#include "Windows.h"
#include "UIlib.h"
using namespace DuiLib;

#pragma comment(lib,"User32.lib")


class CMyProgress : public CProgressUI
{
public:
	CMyProgress(CPaintManagerUI *p,CDuiString sForeImage) :m_pManager(p), m_nPos(0),m_sForeImage(sForeImage) {
	}
	~CMyProgress() {}

	void DoEvent(TEventUI& event) {
		if (event.Type == UIEVENT_TIMER && event.wParam == 10)
		{
			const int MARGIN = 3;
			const int STEP = 4;
			CDuiString imageProp;
			const int PROGRESS_WIDTH = 36;
			const int PROGRESS_HEIGHT = 7;
			const int HORIZONTAL_MARGIN = 3;

			int width = this->GetWidth();
			int height = this->GetHeight();

			width -= 2 * HORIZONTAL_MARGIN;

			if (m_nPos < PROGRESS_WIDTH) {
				imageProp.Format("file='%s' source='%d,%d,%d,%d' dest='%d,%d,%d,%d'", (const char *)m_sForeImage, 
					                                 PROGRESS_WIDTH - m_nPos, 0, PROGRESS_WIDTH, PROGRESS_HEIGHT, HORIZONTAL_MARGIN, 
					                                 MARGIN, HORIZONTAL_MARGIN+m_nPos, height - MARGIN);
			}
			else if (m_nPos > width) {
				imageProp.Format("file='%s' source='%d,%d,%d,%d' dest='%d,%d,%d,%d'", (const char *)m_sForeImage, 
													0, 0, PROGRESS_WIDTH + width - m_nPos, PROGRESS_HEIGHT, 
													m_nPos - PROGRESS_WIDTH + HORIZONTAL_MARGIN, MARGIN, width + HORIZONTAL_MARGIN, height - MARGIN);
			}
			else {
				imageProp.Format("file='%s' source='%d,%d,%d,%d' dest='%d,%d,%d,%d'", (const char *)m_sForeImage, 0,0, PROGRESS_WIDTH, PROGRESS_HEIGHT,
					                                 m_nPos - PROGRESS_WIDTH + HORIZONTAL_MARGIN, MARGIN, m_nPos + HORIZONTAL_MARGIN, height - MARGIN);
			}

			this->SetForeImage(imageProp);

			m_nPos += STEP;
			if (m_nPos > width + PROGRESS_WIDTH) {
				m_nPos = 0;
			}
			return;
		}

		CProgressUI::DoEvent(event);
	}

	void Stop() {
		if (m_pManager) {
			m_pManager->KillTimer(this, 10);
		}
	}

	void Start() {
		if (m_pManager) {
			m_pManager->SetTimer(this, 10, 50);
		}
		m_nPos = 0;
	}
private:
	CPaintManagerUI * m_pManager;
	int               m_nPos;
	CDuiString        m_sForeImage;
};


class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile() { return _T("mainframe_wait_progress.xml"); }
	virtual CDuiString GetSkinFolder() { return _T(""); }

	virtual void    Notify(TNotifyUI& msg) {
		if (msg.sType == _T("click"))
		{
			if (msg.pSender->GetName() == _T("btnHello"))
			{
				CMyProgress * pCtrl = (CMyProgress*)m_PaintManager.FindControl("btnHello1");
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
			return new CMyProgress(&m_PaintManager,"progress_fore.png");
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


