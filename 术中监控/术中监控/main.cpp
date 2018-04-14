#include <windows.h>
#include "main.h"
#include "resource.h"
#include "Business.h"
#include "SettingDlg.h"
#include "MyImage.h"
#include "AboutDlg.h"

//#include <mmsystem.h> //导入声音头文件库   
//#pragma comment(lib,"winmm.lib")//导入声音的链接库  
//#define MAIN_TIMER_ID                1
//#define MAIN_TIMER_INTEVAL           5000

#define COLLECT_TIMER_ID             2

static DWORD   s_dwTemp = 3700;

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
	if (0 == ret ) {

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







void CDuiFrameWnd::InitWindow() {
	g_hWnd = GetHWND();

	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_btnMenu = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("menubtn"));
	m_lblReaderStatus = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblReaderStatus"));

	m_pImageUI = (CMyImageUI *)m_PaintManager.FindControl("image0");
	//SetTimer(m_hWnd, MAIN_TIMER_ID, MAIN_TIMER_INTEVAL, NULL);
	DuiLib::WindowImplBase::InitWindow();
}

void  CDuiFrameWnd::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if (msg.sType == "click") {
		if (name == "menubtn") {
			RECT r = m_btnMenu->GetPos();
			//POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
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
	DuiLib::WindowImplBase::Notify(msg);
}

DuiLib::CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp("MyImage", pstrClass)) {
		return new CMyImageUI(&m_PaintManager);
	}
	return DuiLib::WindowImplBase::CreateControl(pstrClass);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int ret = 0;
	if (uMsg == UM_SHOW_READER_STATUS) {
		CTelemedReader::READER_STATUS eStatus = (CTelemedReader::READER_STATUS)wParam;
		if (eStatus == CTelemedReader::STATUS_OPEN)
		{
			m_lblReaderStatus->SetText("读卡器连接OK");

			// 获取温度
			CBusiness::GetInstance()->ReadTagTempAsyn();
		}			
		else
		{
			m_lblReaderStatus->SetText("读卡器连接失败");
		}
	}
	else if (uMsg == WM_TIMER) {
		if (wParam == COLLECT_TIMER_ID) {
			
		}
	}
	else if (uMsg == UM_SHOW_READ_TAG_TEMP_RET) {
		ret = wParam;
		DWORD dwTemp = lParam;

		if (0 == ret) {
			m_pImageUI->AddTemp(dwTemp);

			// 获取温度
			CBusiness::GetInstance()->ReadTagTempAsyn(g_dwCollectInterval * 1000);
		}		
	}
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam );
}



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//mciSendString("open d:\\1.mp3", NULL, 0, 0);
	//mciSendString("play d:\\1.mp3 ", NULL, 0, 0);
	//mciSendString("close d:\\1.mp3 ", NULL, 0, 0);	

	int ret = 0;
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();

	DuiLib::CPaintManagerUI::SetInstance(hInstance);

	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	// 主界面	
	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();

	// 设置icon
	::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	// show model
	duiFrame.ShowModal();

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}