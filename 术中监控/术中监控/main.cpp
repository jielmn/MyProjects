#include <windows.h>
#include "main.h"
#include "resource.h"
#include "Business.h"
#include "SettingDlg.h"
#include "MyImage.h"
#include "AboutDlg.h"
#include "exhDatabase.h"
#include "AllDataImageDlg.h"

#include<gdiplus.h>//gdi+头文件
using namespace Gdiplus;

//#include <mmsystem.h> //导入声音头文件库   
//#pragma comment(lib,"winmm.lib")//导入声音的链接库  



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
		g_cfg->SetConfig("low alarm", g_dwLowTempAlarm);
		g_cfg->SetConfig("high alarm", g_dwHighTempAlarm);
		g_cfg->SetConfig("collect interval", g_dwCollectInterval);
		g_cfg->SetConfig("alarm file",g_szAlarmFilePath);
		g_cfg->SetConfig("min temperature degree", g_dwMinTemp);
		g_cfg->Save();
		m_pImageUI->Invalidate();
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

void  CDuiFrameWnd::OnImageAll() {
	CAllDataImageDlg * pDlg = new CAllDataImageDlg;

	pDlg->Create(this->m_hWnd, _T("温度全图"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// 如果OK
	if (0 == ret) {

	}
	   
	delete pDlg;
}

void  CDuiFrameWnd::OnTemperatureResult(int nRet, DWORD dwTemp /*= 0*/) {
	if (0 != nRet) {
		m_lblTemperature->SetTextColor(0xFF447AA1);
		m_lblTemperature->SetText("--");
		m_pAlarmUI->FailureAlarm();
		return;
	}

	m_pImageUI->AddTemp(dwTemp);

	if (dwTemp < g_dwLowTempAlarm) {
		m_lblTemperature->SetTextColor(m_dwLowTempColor);
		m_pAlarmUI->LowTempAlarm();
	}
	else if (dwTemp > g_dwHighTempAlarm) {
		m_lblTemperature->SetTextColor(m_dwHighTempColor);
		m_pAlarmUI->HighTempAlarm();
	}
	else {
		m_lblTemperature->SetTextColor(m_dwNormalColor);
		m_pAlarmUI->StopAlarm();
	}

	DuiLib::CDuiString  strTemp;
	strTemp.Format("%.2f", (dwTemp / 100.0));
	m_lblTemperature->SetText(strTemp);

	if ( (int)dwTemp > m_nHighestTemp ) {
		m_nHighestTemp = dwTemp;

		strTemp.Format("%.2f", (dwTemp / 100.0));
		m_lblHighTemperature->SetText(strTemp);
	}

	if ((int)dwTemp < m_nLowestTemp ) {
		m_nLowestTemp = dwTemp;

		strTemp.Format("%.2f", (dwTemp / 100.0));
		m_lblLowTemperature->SetText(strTemp);    
	}
}

void  CDuiFrameWnd::ShowTimeLeft() {
	CDuiString  strTimeLeft;
	strTimeLeft.Format("%ld", m_nTimeLeft);
	m_lblTimeLeft->SetText(strTimeLeft);
}

void  CDuiFrameWnd::ShowTimeLeftError() { 
	m_lblTimeLeft->SetText("--");
}







CDuiFrameWnd::CDuiFrameWnd() {
	m_btnMenu = 0;
	m_lblReaderStatus = 0;
	m_pImageUI = 0;
	m_lblTemperature = 0;
	m_pAlarmUI = 0;
	m_nWidth = 0;
	m_nHeight = 0;

	char buf[8192];
	g_cfg->GetConfig("low temperature color", buf, sizeof(buf), COLOR_LOW_TEMPERATURE);
	sscanf( buf, "0x%x", &m_dwLowTempColor);

	g_cfg->GetConfig("normal temperature color", buf, sizeof(buf), COLOR_NORMAL_TEMPERATURE);
	sscanf(buf, "0x%x", &m_dwNormalColor);	

	g_cfg->GetConfig("high temperature color", buf, sizeof(buf), COLOR_HIGH_TEMPERATURE);
	sscanf(buf, "0x%x", &m_dwHighTempColor);

	m_nLowestTemp  = 10000;
	m_nHighestTemp = 0;
	m_nTimeLeft = 0;
}

void CDuiFrameWnd::InitWindow() {
	g_hWnd = GetHWND();

	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_btnMenu = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("menubtn"));
	m_lblReaderStatus = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblReaderStatus"));
	m_lblTemperature = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblTemperature"));

	m_lblLowTemperature = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblLowestTemperature"));
	m_lblHighTemperature = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblHighestTemperature"));
	m_lblTimeLeft = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblTileLeft"));

	m_pImageUI = (CMyImageUI *)m_PaintManager.FindControl("image0");	
	m_pAlarmUI = (CAlarmImageUI *)m_PaintManager.FindControl("alarm_image");

	m_lblTemperature->SetText("--");
	m_lblLowTemperature->SetText("--");
	m_lblHighTemperature->SetText("--");
	m_lblTimeLeft->SetText("--");

	m_txtTitle = static_cast<DuiLib::CTextUI*>(m_PaintManager.FindControl("txtTitle")); 

#if TEST_FLAG
	SetTimer(m_hWnd, MAIN_TIMER_ID, MAIN_TIMER_INTEVAL, NULL);
	m_lblReaderStatus->SetText("读卡器连接OK");
#endif
	 
#ifdef TELEMED_READER_TYPE_1
	m_txtTitle->SetText("术中监控");
#else
	m_txtTitle->SetText("术中监控");
#endif
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
	else if (msg.sType == "menu_data_image") {
		OnImageAll();
		return;
	}
	DuiLib::WindowImplBase::Notify(msg);
}

DuiLib::CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp("MyImage", pstrClass)) {
		return new CMyImageUI(&m_PaintManager);
	}
	else if (0 == strcmp("AlarmImage", pstrClass)) {
		return new CAlarmImageUI(&m_PaintManager);
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
	else if (uMsg == WM_TIMER ) {

#if  TEST_FLAG
		if (wParam == MAIN_TIMER_ID) {
			static  DWORD  dwTemp = 3350;

			OnTemperatureResult(0, dwTemp);
			 
			if (dwTemp < 3400)  
				dwTemp += 80;
			else if (dwTemp < 3450)
				dwTemp += 60;
			else if (dwTemp < 3500)
				dwTemp += 50;
			else if (dwTemp < 3550)
				dwTemp += 40;
			else if (dwTemp < 3580)
				dwTemp += 30;   
			else if (dwTemp < 3620)
				dwTemp += 20;
			else if (dwTemp < 3640)
				dwTemp += 10;
			else if (dwTemp < 3650)
				dwTemp += 5;
			else if (dwTemp < 3655)
				dwTemp += 3;
			else if (dwTemp < 3658)
				dwTemp += 2;
			else if (dwTemp < 3663)
				dwTemp += 1;
			else
				dwTemp += 0;
		}
#else
		if (TIME_LEFT_TIMER_ID == wParam) {
			if ( m_nTimeLeft > 0 ) {
				m_nTimeLeft--;
			}			

			if (m_nTimeLeft <= 0) {
				ShowTimeLeft(); 
				KillTimer(m_hWnd, TIME_LEFT_TIMER_ID);
			}
			else {
				ShowTimeLeft();
			}
		}
#endif

	}
	else if (uMsg == UM_SHOW_READ_TAG_TEMP_RET) {
		ret = wParam;   
		DWORD dwTemp = lParam;

		if (0 == ret) {
			OnTemperatureResult( 0, dwTemp);
			// 获取温度
			CBusiness::GetInstance()->ReadTagTempAsyn(g_dwCollectInterval * 1000);
			m_nTimeLeft = g_dwCollectInterval;
			SetTimer(m_hWnd, TIME_LEFT_TIMER_ID, TIME_LEFT_TIMER_INTEVAL, 0);

			ShowTimeLeft();			
		}
		// 获取温度失败
		else {
			OnTemperatureResult(ret);
			ShowTimeLeftError();
		}
	}
	else if (uMsg == WM_SIZE) {
		m_nWidth  = LOWORD(lParam);
		m_nHeight = HIWORD(lParam);

		if (0 != m_pImageUI) {
			m_pImageUI->SetWndRect(m_nWidth, m_nHeight);
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

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR pGdiToken;
	GdiplusStartup(&pGdiToken, &gdiplusStartupInput, NULL);//初始化GDI+

	int ret = 0;
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	CMyDatabase::GetInstance()->OnInitDb();

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

	CMyDatabase::GetInstance()->OnDeinitDb();
	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();


	//死循环，下面这句不会调用，只是想把那个意思表明
	GdiplusShutdown(pGdiToken);//关闭GDI+

	return 0;
}