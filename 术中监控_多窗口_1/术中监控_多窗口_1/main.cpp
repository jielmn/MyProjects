// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "LmnTelSvr.h"
#include "SettingDlg.h"



CDuiFrameWnd::CDuiFrameWnd() : m_callback(&m_PaintManager) {
	memset( m_pGrids, 0, sizeof(m_pGrids) );
	memset(m_pLblIndexes_small, 0, sizeof(m_pLblIndexes_small));
	memset(m_pLblBed_small, 0, sizeof(m_pLblBed_small));
	memset(m_pLblName_small, 0, sizeof(m_pLblName_small));
	memset(m_pLblCurTemp_small, 0, sizeof(m_pLblCurTemp_small));	
	memset(m_pLblBedTitle_small, 0, sizeof(m_pLblBedTitle_small));
	memset(m_pLblNameTitle_small, 0, sizeof(m_pLblNameTitle_small));
	memset(m_pLblCurTempTitle_small, 0, sizeof(m_pLblCurTempTitle_small));
	memset(m_pMyImage, 0, sizeof(m_pMyImage));
	m_nState = STATE_GRIDS;	
}

void  CDuiFrameWnd::InitWindow() {
	CDuiString strText;

	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	
	m_btnMenu = static_cast<CButtonUI*>(m_PaintManager.FindControl(BTN_MENU_NAME));
	m_layWindow = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(LAYOUT_WINDOW_NAME));
	m_layMain = static_cast<CTileLayoutUI*>(m_PaintManager.FindControl(LAYOUT_MAIN_NAME));
	m_layMain->SetFixedColumns(g_dwLayoutColumns);
	
	for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
		CDialogBuilder builder;
		int nIndex = i;
		m_pGrids[nIndex] = builder.Create(MYCHART_XML_FILE, (UINT)0, &m_callback, &m_PaintManager);
		m_pGrids[nIndex]->SetBorderSize(1);
		m_pGrids[nIndex]->SetName(GRID_NAME);
		m_pGrids[nIndex]->SetTag(nIndex);

		m_pLblIndexes_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_INDEX_SMALL_NAME, 0));
		if (m_pLblIndexes_small[nIndex]) {
			strText.Format("%lu", nIndex + 1);   
			m_pLblIndexes_small[nIndex]->SetText(strText);
		}

		m_pLblBed_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_BED_SMALL_NAME, 0));
		m_pLblName_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_NAME_SMALL_NAME, 0));
		m_pLblCurTemp_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_CUR_TEMP_SMALL_NAME, 0));

		m_pLblBedTitle_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_BED_TITLE_SMALL_NAME, 0));
		m_pLblNameTitle_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_NAME_TITLE_SMALL_NAME, 0));
		m_pLblCurTempTitle_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_CUR_TEMP_TITLE_SMALL_NAME, 0));

		m_pMyImage[nIndex] = static_cast<CMyImageUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, MYIMAGE_NAME, 0));
		m_pMyImage[nIndex]->SetIndex(nIndex);

		if ((DWORD)nIndex >= g_dwLayoutColumns * g_dwLayoutRows) {
			m_pGrids[nIndex]->SetVisible(false);
		}
		m_layMain->Add(m_pGrids[nIndex]); 
	}

	m_layStatus = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(LAYOUT_STATUS_NAME));
	
	OnChangeSkin();

#if TEST_FLAG
	SetTimer(m_hWnd, TIMER_TEST_ID, TIMER_TEST_INTERVAL, NULL);
	InitRand(TRUE, 100);
#endif

	WindowImplBase::InitWindow();    
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString & name = msg.pSender->GetName();

	if (msg.sType == "click") {
		if ( name == BTN_MENU_NAME ) {
			OnBtnMenu(msg);			
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
	if ( uMsg == WM_SIZE ) {
		OnSize(wParam, lParam);		 
	}
	else if (uMsg == WM_LBUTTONDBLCLK) {
		OnDbClick();
	}
	else if (uMsg == WM_TIMER) {
#if TEST_FLAG
		if (wParam == TIMER_TEST_ID) {
			m_pMyImage[0]->AddTemp(GetRand(3200, 4200));
			m_pMyImage[1]->AddTemp(GetRand(3200, 4200));
		}
#endif
	}
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

// main frame events
void  CDuiFrameWnd::OnSize(WPARAM wParam, LPARAM lParam) {
	DWORD dwWidth  = LOWORD(lParam) - LAYOUT_WINDOW_HMARGIN * 2;           // layWindow的左右margin
	// 30是底部status的大致高度, 1是layWindow的上下margin, 32是标题栏高度
	DWORD dwHeight = HIWORD(lParam) - STATUS_PANE_HEIGHT - LAYOUT_WINDOW_VMARGIN * 2 - WINDOW_TITLE_HEIGHT;  

	ReLayout(dwWidth, dwHeight);
}

// dwWidth,  layMain的宽度
// dwHeight, layMain的高度
void   CDuiFrameWnd::ReLayout(DWORD dwWidth, DWORD dwHeight) {
	if ( 0 == m_layMain) {
		return;		
	}

	SIZE s;
	if (m_nState == STATE_GRIDS) {
		s.cx = (dwWidth - 1) / g_dwLayoutColumns + 1;
		s.cy = (dwHeight - 1) / g_dwLayoutRows + 1;
		m_layMain->SetItemSize(s);

		// 重新修正标题栏的高度
		m_layStatus->SetFixedHeight(STATUS_PANE_HEIGHT - (s.cy * g_dwLayoutRows - dwHeight));
	}
	else {
		s.cx = dwWidth;
		s.cy = dwHeight;
		m_layMain->SetItemSize(s);

		// 重新修正标题栏的高度
		m_layStatus->SetFixedHeight(30);
	}
	
	
}

void   CDuiFrameWnd::OnChangeSkin() {
	m_layMain->SetBkColor(g_skin[LAYOUT_MAIN_BK_COLOR_INDEX]);
	m_layStatus->SetBkColor(g_skin[LABEL_STATUS_BK_COLOR_INDEX]);

	for (DWORD i = 0; i < g_dwLayoutRows; i++) {
		for (DWORD j = 0; j < g_dwLayoutColumns; j++) {
			int nIndex = i*g_dwLayoutColumns + j;
			m_pGrids[nIndex]->SetBorderColor(g_skin[GRID_BORDER_COLOR_INDEX]);
			m_pLblIndexes_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);

			m_pLblBed_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
			m_pLblName_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
			m_pLblCurTemp_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
			m_pLblBedTitle_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
			m_pLblNameTitle_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
			m_pLblCurTempTitle_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);

			m_pMyImage[nIndex]->SetBkColor(g_skin[MYIMAGE_BK_COLOR_INDEX]);
		}
	}
}

void   CDuiFrameWnd::OnBtnMenu(TNotifyUI& msg) {
	RECT r = m_btnMenu->GetPos();
	POINT pt = { r.left, r.bottom };
	CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);

	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

void   CDuiFrameWnd::OnSetting() {
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

void   CDuiFrameWnd::OnAbout() {
	
}

void   CDuiFrameWnd::OnDbClick() {
	POINT point;
	CDuiString strName;

	GetCursorPos(&point);
	::ScreenToClient(m_hWnd, &point);
	CControlUI * pFindControl = m_PaintManager.FindSubControlByPoint(m_layMain, point);

	while (pFindControl) {
		strName = pFindControl->GetName();
		if (0 == strcmp(strName, GRID_NAME)) {
			DWORD nIndex = pFindControl->GetTag();

			if ( m_nState == STATE_GRIDS ) {				
				for (DWORD i = 0; i < g_dwLayoutColumns * g_dwLayoutRows; i++) {
					if (i != nIndex) {
						m_layMain->Remove(m_pGrids[i], true);
					}
				}
				m_layMain->SetFixedColumns(1);
				m_nState = STATE_MAXIUM;
				ReLayout(m_layMain->GetWidth(), m_layMain->GetHeight());
			}
			else {
				for (DWORD i = 0; i < g_dwLayoutColumns * g_dwLayoutRows; i++) {
					if (i != nIndex) {
						m_layMain->AddAt(m_pGrids[i],i);
					}
				}
				m_layMain->SetFixedColumns(g_dwLayoutColumns);
				m_nState = STATE_GRIDS;
				ReLayout(m_layMain->GetWidth(), m_layMain->GetHeight());
			}
			
			break;
		}
		pFindControl = pFindControl->GetParent();
	}
}
  
 

 
 
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HANDLE handle = ::CreateMutex(NULL, FALSE, GLOBAL_LOCK_NAME);//handle为声明的HANDLE类型的全局变量 
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		HWND hWnd = ::FindWindow(MAIN_FRAME_NAME, 0);
		::ShowWindow(hWnd, SW_SHOWMAXIMIZED);
		::InvalidateRect(hWnd, 0, TRUE);
		return 0;
	}

	int ret = 0;
	LmnToolkits::ThreadManager::GetInstance();
	ret = CBusiness::GetInstance()->Init();
	if (0 != ret) {
		goto LABEL_ERROR;
	}

	g_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

	DWORD  dwPort = 0;
	g_cfg->GetConfig(CFG_TELNET_PORT, dwPort, 1135);
	JTelSvrStart((unsigned short)dwPort, 10);

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

	g_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

LABEL_ERROR:
	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}

         
             