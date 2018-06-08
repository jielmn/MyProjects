// win32_1.cpp : 定义应用程序的入口点。
//

#include "Windows.h"
#include "main.h"
#include "resource.h"
#include "ImageAll.h"
#include "business.h"


// menu
class CDuiMenu : public DuiLib::WindowImplBase
{
protected:
	virtual ~CDuiMenu() {};        // 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错
	DuiLib::CDuiString  m_strXMLPath;
	DuiLib::CControlUI * m_pOwner;

public:
	explicit CDuiMenu(LPCTSTR pszXMLPath, DuiLib::CControlUI * pOwner) : m_strXMLPath(pszXMLPath), m_pOwner(pOwner) {}
	virtual LPCTSTR    GetWindowClassName()const { return _T("CDuiMenu "); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(""); }
	virtual DuiLib::CDuiString GetSkinFile() { return m_strXMLPath; }
	virtual void       OnFinalMessage(HWND hWnd) { delete this; }

	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Close();
		bHandled = FALSE;
		return 0;
	}

	void Init(HWND hWndParent, POINT ptPos)
	{
		Create(hWndParent, _T("MenuWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		::ClientToScreen(hWndParent, &ptPos);
		::SetWindowPos(*this, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	}

	virtual void  Notify(DuiLib::TNotifyUI& msg) {
		if (msg.sType == "itemclick") {
			if (m_pOwner) {
				m_pOwner->GetManager()->SendNotify(m_pOwner, msg.pSender->GetName(), 0, 0, true);
				PostMessage(WM_CLOSE);
			}
			return;
		}
		WindowImplBase::Notify(msg);
	}

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return __super::HandleMessage(uMsg, wParam, lParam);
	}
};


 

// call back
DuiLib::CControlUI* CTempDataBuilderCallback::CreateControl(LPCTSTR pstrClass) {
	if (0 == _stricmp("MyImage", pstrClass) ) {
		return new CImageAll(m_PaintManager);  
	}
	return 0;
}


CDuiFrameWnd::CDuiFrameWnd() {
	m_callback.m_PaintManager = &m_PaintManager;
}

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

	m_image0 = static_cast<CImageAll*>(m_PaintManager.FindControl("image_0"));
	m_btnMenu = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("menubtn"));

	m_btnPrevius->SetEnabled(false);
	m_btnStop0->SetEnabled(false);
	m_btnStop1->SetEnabled(false);

#if TEST_FLAG
	::SetTimer( m_hWnd, MAIN_TIMER_ID, MAIN_TIMER_INTEVAL, NULL );
#endif

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
		else if (name == "menubtn") {
			RECT r = m_btnMenu->GetPos();
			POINT pt = { r.left, r.bottom };
			CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);

			pMenu->Init(*this, pt);
			pMenu->ShowWindow(TRUE); 
		}
	}
	else if (msg.sType == "menu_setting") {
		OnSetting();
		return;
	}
	else if (msg.sType == "menu_about") {
		OnAbout();
		return;
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
		DuiLib::CControlUI * pUI = builder.Create("StandardTemp.xml", (UINT)0, &m_callback, &m_PaintManager);
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

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_TIMER)
	{
#if TEST_FLAG
		if (wParam == MAIN_TIMER_ID) {
			m_image0->AddTempData(GetRand(2000, 4200));
		}
#endif
	}
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void  CDuiFrameWnd::OnSetting() {

}

void  CDuiFrameWnd::OnAbout() {
	
}









int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	InitRand(TRUE, 1);

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

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}


