#ifdef _DEBUG
#include <vld.h>
#endif

#include "resource.h"
#include "LmnCommon.h"
#include "main.h"

#define MAIN_TIMER_ID                1
#define MAIN_TIMER_INTEVAL           1000

static DWORD   s_dwTemp = 3700;

CMainWnd::CMainWnd() {
	m_pImageUI = 0;
}

void  CMainWnd::Notify(TNotifyUI& msg) {		
	WindowImplBase::Notify(msg);
}

void  CMainWnd::InitWindow() {
	m_pImageUI = (CMyImageUI *)m_PaintManager.FindControl("image0");
	SetTimer(m_hWnd, MAIN_TIMER_ID, MAIN_TIMER_INTEVAL, NULL);
	WindowImplBase::InitWindow();
}

LRESULT CMainWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_TIMER) {
		if (wParam == MAIN_TIMER_ID) {
			DWORD dwTemp   = GetRand(10, 40);
			BOOL bPositive = (BOOL)GetRand(0, 1);
			if (m_pImageUI)
			{
				if (bPositive) {
					s_dwTemp += dwTemp;
				}
				else {
					s_dwTemp -= dwTemp;
				}
				if (s_dwTemp <= 3400) {
					s_dwTemp = 3400;
				}
				else if (s_dwTemp >= 4200) {
					s_dwTemp = 4200;
				}
				m_pImageUI->AddTemp(s_dwTemp);
			}
		}
	}

	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

CControlUI * CMainWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp("MyImage", pstrClass)) {
		return new CMyImageUI(&m_PaintManager);
	}	
	return 0;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CPaintManagerUI::SetInstance(hInstance);

	CMainWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE, 0, 0, 800, 600);
	duiFrame.CenterWindow();
	::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	duiFrame.ShowModal();

	return 0;

}
