// win32_1.cpp : 定义应用程序的入口点。
//

#include "Windows.h"
#include "main.h"
#include "resource.h"

#pragma comment(lib,"User32.lib")

void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_tabs = static_cast<DuiLib::CTabLayoutUI*>( m_PaintManager.FindControl("switch") );
	m_btnNext = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnNext"));
	m_btnPrevius = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnPrevius"));
	m_navigator = static_cast<DuiLib::CHorizontalLayoutUI*>(m_PaintManager.FindControl("navigator"));

	m_btnStart0 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnStart_0"));
	m_btnStop0 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnStop_0"));

	m_btnStart1 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnStart_1"));
	m_btnStop1 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnStop_1"));

	m_btnPrevius->SetEnabled(false);
	m_btnStop0->SetEnabled(false);
	m_btnStop1->SetEnabled(false);
	WindowImplBase::InitWindow();            
}

void CDuiFrameWnd::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	if ( msg.sType == _T("click") ) {
		if (name == "btnNext") {
			int nCursel = m_tabs->GetCurSel();
			if ( nCursel < 3 ) {
				nCursel++;
				m_tabs->SelectItem(nCursel);
				if (nCursel == 3) {					
					m_btnNext->SetEnabled(false);
					m_navigator->SetVisible(false);               
				}
				m_btnPrevius->SetEnabled(true);     
			}			
		}
		else if (name == "btnPrevius") {
			int nCursel = m_tabs->GetCurSel();
			if (nCursel > 0) {
				nCursel--;
				m_tabs->SelectItem(nCursel);
				if (nCursel == 0) {
					m_btnPrevius->SetEnabled(false);
				}
				m_btnNext->SetEnabled(true);
			}
			
		}
	}

	WindowImplBase::Notify(msg);
}

DuiLib::CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {

	DuiLib::CDialogBuilder  builder;
	if (0 == strcmp("PatientInfo", pstrClass)) {
		DuiLib::CControlUI * pUI = builder.Create( "PatientInfo.xml", (UINT)0, 0, &m_PaintManager ); 
		return pUI;
	}
	else if (0 == strcmp("StandardTemp", pstrClass)) {
		DuiLib::CControlUI * pUI = builder.Create("StandardTemp.xml", (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("CompareTemp", pstrClass)) {
		DuiLib::CControlUI * pUI = builder.Create("CompareTemp.xml", (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("Surgery", pstrClass)) {
		DuiLib::CControlUI * pUI = builder.Create("Surgery.xml", (UINT)0, 0, &m_PaintManager);
		return pUI;
	}

	return WindowImplBase::CreateControl(pstrClass);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
	duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame->CenterWindow();

	// 设置icon
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	duiFrame->ShowModal();
	delete duiFrame;

	return 0;
}


