// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "AboutDlg.h"
#include "SettingDlg.h"  

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


void  CDuiFrameWnd::OnSetting() {
	CSettingDlg * pSettingDlg = new CSettingDlg;

	pSettingDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pSettingDlg->CenterWindow();
	int ret = pSettingDlg->ShowModal();

	// 如果OK
	if (0 == ret) {
		//g_cfg->SetConfig("low alarm", g_dwLowTempAlarm);
		//g_cfg->SetConfig("high alarm", g_dwHighTempAlarm);
		//g_cfg->SetConfig("collect interval", g_dwCollectInterval);
		//g_cfg->SetConfig("alarm file", g_szAlarmFilePath);
		//g_cfg->SetConfig("min temperature degree", g_dwMinTemp);
		//g_cfg->SetConfig("com port", g_szComPort);
		//g_cfg->Save();
		//m_pImageUI->Invalidate_0();
	}

	delete pSettingDlg;
}

void  CDuiFrameWnd::OnAbout() {
	CAboutDlg * pAboutDlg = new CAboutDlg;

	pAboutDlg->Create(this->m_hWnd, _T("关于"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAboutDlg->CenterWindow();
	int ret = pAboutDlg->ShowModal();

	// 如果OK
	if (0 == ret) {

	}

	delete pAboutDlg;
}

void  CDuiFrameWnd::OnDbClick() {
	POINT point;
	CDuiString strName;

	GetCursorPos(&point);
	::ScreenToClient(m_hWnd, &point);
	CControlUI * pFindControl = m_PaintManager.FindSubControlByPoint(m_layMain, point);

	while ( pFindControl ) {
		strName = pFindControl->GetName();
		if ( 0 == StrNiCmp((const char *)strName, "mychart", 7) ) {
			int nIndex = 0;
			sscanf(strName, "mychart_%d", &nIndex);
			assert( nIndex > 0 && nIndex <= MYCHART_COUNT );
			nIndex--;

			if ( m_chart_state == CHART_STATE_NORMAL ) {
				int nLayIndex = nIndex / MYCHART_PER_LAYER;
				
				for (int i = 0; i < MYCHART_COUNT; i++) {
					if (i != nIndex) {
						m_chart[i]->SetVisible(false);
					}					
				}

				for (int i = 0; i < LAY_LAYER_COUNT; i++) {
					if (i != nLayIndex) {
						m_layLayer[i]->SetVisible(false);
					}
				}       
				m_chart_state = CHART_STATE_EXPAND;
			}
			else {
				for (int i = 0; i < LAY_LAYER_COUNT; i++) {
					m_layLayer[i]->SetVisible(true);
				}
				for (int i = 0; i < MYCHART_COUNT; i++) {
					m_chart[i]->SetVisible(true);
				}
				m_chart_state = CHART_STATE_NORMAL;
			}
		}
		pFindControl = pFindControl->GetParent();
	}
}

void  CDuiFrameWnd::InitWindow() {
	CDuiString strText;

	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_btnMenu = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("menubtn"));

	m_layMain = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl("layMain"));

	for (int i = 0; i < LAY_LAYER_COUNT; i++) {
		strText.Format("lay_%d", i + 1);
		m_layLayer[i] = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(strText));
	}

	for (int i = 0; i < MYCHART_COUNT; i++) {
		strText.Format("mychart_%d", i+1);
		m_chart[i] = m_PaintManager.FindControl(strText);
	}
	m_chart_state = CHART_STATE_NORMAL;


	
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	CDialogBuilder builder;
	DuiLib::CDuiString strText;

	if (0 == strcmp(pstrClass, "MyChart_1") || 0 == strcmp(pstrClass, "MyChart_2") || 0 == strcmp(pstrClass, "MyChart_3") ||
		0 == strcmp(pstrClass, "MyChart_4") || 0 == strcmp(pstrClass, "MyChart_5") || 0 == strcmp(pstrClass, "MyChart_6")) {
		strText.Format("%s.xml", pstrClass);
		CControlUI * pUI = builder.Create((const char *)strText, (UINT)0, 0, &m_PaintManager);
		return pUI;      
	}	 
	
	return WindowImplBase::CreateControl(pstrClass);                                                  
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if (msg.sType == "click") {
		if (name == "menubtn") {
			RECT r = m_btnMenu->GetPos();
			POINT pt = { r.left, r.bottom };
			CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);

			pMenu->Init(*this, pt);
			pMenu->ShowWindow(TRUE);
		}
	}	
	else if (msg.sType == "menu_setting") {
		OnSetting();
	}
	else if (msg.sType == "menu_about") {
		OnAbout();
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_LBUTTONDBLCLK) {
		OnDbClick();
	}
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}
  







int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_log->Output(ILog::LOG_SEVERITY_INFO, "begin.\n");

	CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
	duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame->CenterWindow();  

	// 设置icon
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	duiFrame->ShowModal();
	delete duiFrame;

	g_log->Output(ILog::LOG_SEVERITY_INFO, "close.\n");         

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance(); 

	return 0;
}


