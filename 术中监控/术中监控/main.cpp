#include <windows.h>
#include "main.h"
#include "resource.h"
#include "Business.h"
#include "SettingDlg.h"
#include "MyImage.h"
#include "AboutDlg.h"
#include "exhDatabase.h"
#include "AllDataImageDlg.h"

//#include <mmsystem.h> //��������ͷ�ļ���   
//#pragma comment(lib,"winmm.lib")//�������������ӿ�  



static DWORD   s_dwTemp = 3700;

class CDuiMenu : public DuiLib::WindowImplBase
{
protected:
	virtual ~CDuiMenu() {};        // ˽�л����������������˶���ֻ��ͨ��new�����ɣ�������ֱ�Ӷ���������ͱ�֤��delete this�������
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

	pSettingDlg->Create(this->m_hWnd, _T("����"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pSettingDlg->CenterWindow();
	int ret = pSettingDlg->ShowModal();

	// ���OK
	if (0 == ret ) {
		g_cfg->SetConfig("low alarm", g_dwLowTempAlarm);
		g_cfg->SetConfig("high alarm", g_dwHighTempAlarm);
		g_cfg->SetConfig("collect interval", g_dwCollectInterval);
		g_cfg->SetConfig("alarm file",g_szAlarmFilePath);
		g_cfg->Save();
		m_pImageUI->Invalidate();
	}

	delete pSettingDlg;
}

void  CDuiFrameWnd::OnAbout() {
	CAboutDlg * pAboutDlg = new CAboutDlg;

	pAboutDlg->Create(this->m_hWnd, _T("����"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAboutDlg->CenterWindow();
	int ret = pAboutDlg->ShowModal();

	// ���OK
	if (0 == ret) {

	}

	delete pAboutDlg;
}

void  CDuiFrameWnd::OnImageAll() {
	CAllDataImageDlg * pDlg = new CAllDataImageDlg;

	pDlg->Create(this->m_hWnd, _T("�¶�ȫͼ"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();

	// ���OK
	if (0 == ret) {

	}
	   
	delete pDlg;
}





CDuiFrameWnd::CDuiFrameWnd() {
	m_btnMenu = 0;
	m_lblReaderStatus = 0;
	m_pImageUI = 0;
	m_lblTemperature = 0;
	m_pAlarmUI = 0;
	m_nWidth = 0;
	m_nHeight = 0;
}

void CDuiFrameWnd::InitWindow() {
	g_hWnd = GetHWND();

	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_btnMenu = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("menubtn"));
	m_lblReaderStatus = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblReaderStatus"));
	m_lblTemperature = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblTemperature"));

	m_pImageUI = (CMyImageUI *)m_PaintManager.FindControl("image0");	
	m_pAlarmUI = (CAlarmImageUI *)m_PaintManager.FindControl("alarm_image");

	m_lblTemperature->SetText("--");

#if TEST_FLAG
	SetTimer(m_hWnd, MAIN_TIMER_ID, MAIN_TIMER_INTEVAL, NULL);
	m_lblReaderStatus->SetText("����������OK");
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
			m_lblReaderStatus->SetText("����������OK");

			// ��ȡ�¶�
			CBusiness::GetInstance()->ReadTagTempAsyn();
		}			
		else
		{
			m_lblReaderStatus->SetText("����������ʧ��");
		}
	}
	else if (uMsg == WM_TIMER) {
#if  TEST_FLAG
		if (wParam == MAIN_TIMER_ID) {
			static  DWORD  dwTemp = 3350;

			m_pImageUI->AddTemp(dwTemp);

			if (dwTemp < g_dwLowTempAlarm) {
				m_lblTemperature->SetTextColor(0xFF0000FF);
				m_pAlarmUI->LowTempAlarm();
			}
			else if (dwTemp > g_dwHighTempAlarm) {
				m_lblTemperature->SetTextColor(0xFFFF0000);
				m_pAlarmUI->HighTempAlarm();
			}
			else {
				m_lblTemperature->SetTextColor(0xFF447AA1);
				m_pAlarmUI->StopAlarm();
			}

			DuiLib::CDuiString  strTemp;
			strTemp.Format("%.2f", (dwTemp / 100.0));
			m_lblTemperature->SetText(strTemp);

			if ( dwTemp < 3400 )
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
#endif
	}
	else if (uMsg == UM_SHOW_READ_TAG_TEMP_RET) {
		ret = wParam;
		DWORD dwTemp = lParam;

		if (0 == ret) {
			m_pImageUI->AddTemp(dwTemp);

			if ( dwTemp < g_dwLowTempAlarm ) {
				m_lblTemperature->SetTextColor(0xFF0000FF);
				m_pAlarmUI->LowTempAlarm();
			}
			else if (dwTemp > g_dwHighTempAlarm) {
				m_lblTemperature->SetTextColor(0xFFFF0000);
				m_pAlarmUI->HighTempAlarm();
			}
			else {
				m_lblTemperature->SetTextColor(0xFF447AA1);
				m_pAlarmUI->StopAlarm(); 
			}

			DuiLib::CDuiString  strTemp;
			strTemp.Format("%.2f", (dwTemp / 100.0) );
			m_lblTemperature->SetText(strTemp); 

			// ��ȡ�¶�
			CBusiness::GetInstance()->ReadTagTempAsyn(g_dwCollectInterval * 1000);
		}
		// ��ȡ�¶�ʧ��
		else {
			m_lblTemperature->SetTextColor(0xFF447AA1);
			m_lblTemperature->SetText("--");
			m_pAlarmUI->FailureAlarm();
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

	int ret = 0;
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	CMyDatabase::GetInstance()->OnInitDb();

	DuiLib::CPaintManagerUI::SetInstance(hInstance);

	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	// ������	
	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();

	// ����icon
	::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	// show model
	duiFrame.ShowModal();

	CMyDatabase::GetInstance()->OnDeinitDb();
	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}