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



CDuiFrameWnd::CDuiFrameWnd() : m_callback(&m_PaintManager,this) {
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
	m_nMaxGridIndex = -1;
}

void  CDuiFrameWnd::InitWindow() {
	CDuiString strText;

	g_hWnd = GetHWND();
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
	m_edRemark = static_cast<CEditUI*>(m_PaintManager.FindControl(EDIT_REMARK_NAME));
	m_edRemark->SetText("");
	m_edRemark->SetVisible(false);
 
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
		else if (name == MYIMAGE_NAME) {
			POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
			OnMyImageClick(&pt);
		}
	}
	else if (msg.sType == "menu_setting") {
		OnSetting();
	}
	else if (msg.sType == "menu_about") {
		OnAbout();
	}
	else if (msg.sType == "killfocus") {
		if (name == EDIT_REMARK_NAME ) {
			OnEdtRemarkKillFocus();
		}
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
	else if (uMsg == UM_UPDATE_SCROLL) {
		OnUpdateGridScroll(wParam, lParam);
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

// 重新布局(在设置更改之后)
void   CDuiFrameWnd::UpdateLayout() {
	DWORD dwInvisibleIndex = g_dwLayoutColumns * g_dwLayoutRows;
	for ( DWORD i = 0; i < MAX_GRID_COUNT; i++ ) {
		if (i < dwInvisibleIndex) {
			m_pGrids[i]->SetVisible(true);
		}
		else {
			m_pGrids[i]->SetVisible(false);
		}
	}

	if (m_nState == STATE_GRIDS) {
		m_layMain->SetFixedColumns(g_dwLayoutColumns);
		ReLayout(m_layMain->GetWidth(), m_layMain->GetHeight());
	}
	else {
		assert(m_nState == STATE_MAXIUM);
		// 如果当前格子超出范围
		if ( m_nMaxGridIndex >= (int)(g_dwLayoutColumns * g_dwLayoutRows) ) {			
			m_layMain->Remove(m_pGrids[m_nMaxGridIndex],true);
			for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
				m_layMain->AddAt(m_pGrids[i], i);
			}
			m_layMain->SetFixedColumns(g_dwLayoutColumns);
			m_nState = STATE_GRIDS;
			m_pMyImage[m_nMaxGridIndex]->SetState(STATE_GRIDS);
			m_nMaxGridIndex = -1;			
			ReLayout(m_layMain->GetWidth(), m_layMain->GetHeight());
		}
	}
	
}

void   CDuiFrameWnd::OnChangeSkin() {
	m_layMain->SetBkColor(g_skin[LAYOUT_MAIN_BK_COLOR_INDEX]);
	m_layStatus->SetBkColor(g_skin[LABEL_STATUS_BK_COLOR_INDEX]);

	for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
		int nIndex = i;
		m_pGrids[nIndex]->SetBorderColor(g_skin[GRID_BORDER_COLOR_INDEX]);
		m_pLblIndexes_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);

		m_pLblBed_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
		m_pLblName_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
		m_pLblCurTemp_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
		m_pLblBedTitle_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
		m_pLblNameTitle_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
		m_pLblCurTempTitle_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);

		m_pMyImage[nIndex]->SetBkColor(g_skin[MYIMAGE_BK_COLOR_INDEX]);
		m_pMyImage[nIndex]->OnChangeSkin();
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
	CDuiString  strText;
	DWORD  dwValue = 0;
	CSettingDlg * pSettingDlg = new CSettingDlg;

	pSettingDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pSettingDlg->CenterWindow();
	int ret = pSettingDlg->ShowModal();

	// 如果OK
	if (0 == ret) {
		dwValue = DEFAULT_MAIN_LAYOUT_COLUMNS;
		g_cfg->SetConfig( CFG_MAIN_LAYOUT_COLUMNS, g_dwLayoutColumns, &dwValue );

		dwValue = DEFAULT_MAIN_LAYOUT_ROWS;
		g_cfg->SetConfig(CFG_MAIN_LAYOUT_ROWS, g_dwLayoutRows, &dwValue);

		dwValue = DEFAULT_SKIN;
		g_cfg->SetConfig(CFG_SKIN, g_dwSkinIndex, &dwValue);

		dwValue = DEFAULT_ALARM_VOICE_SWITCH;
		g_cfg->SetConfig(CFG_ALARM_VOICE_SWITCH, g_bAlarmVoiceOff, &dwValue);

		for (int i = 0; i < MAX_GRID_COUNT; i++) {
			strText.Format(CFG_LOW_ALARM " %d", i + 1);
			dwValue = DEFAULT_LOW_ALARM;
			g_cfg->SetConfig(strText, g_dwLowTempAlarm[i], &dwValue );

			strText.Format(CFG_HIGH_ALARM " %d", i + 1);
			dwValue = DEFAULT_HI_ALARM;
			g_cfg->SetConfig(strText, g_dwHighTempAlarm[i], &dwValue);

			strText.Format(CFG_COLLECT_INTERVAL " %d", i + 1);
			dwValue = DEFAULT_COLLECT_INTERVAL;
			g_cfg->SetConfig(strText, g_dwCollectInterval[i], &dwValue);

			strText.Format(CFG_MIN_TEMP " %d", i + 1);
			dwValue = DEFAULT_MIN_TEMP;
			g_cfg->SetConfig(strText, g_dwMyImageMinTemp[i], &dwValue);

			strText.Format(CFG_COM_PORT " %d", i + 1);
			g_cfg->SetConfig(strText, g_szComPort[i], "");
		}

		g_cfg->Save();

		g_skin.SetSkin(g_dwSkinIndex);
		OnChangeSkin();
		UpdateLayout();
		//::InvalidateRect(GetHWND(), 0, TRUE);
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
				for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
					if (i != nIndex) {
						m_layMain->Remove(m_pGrids[i], true);
					}
				}
				m_layMain->SetFixedColumns(1);
				m_nState = STATE_MAXIUM;
				m_nMaxGridIndex = nIndex;
				m_pMyImage[nIndex]->SetState(m_nState);
				ReLayout(m_layMain->GetWidth(), m_layMain->GetHeight());
			}
			else {
				assert(STATE_MAXIUM == m_nState);
				assert(m_nMaxGridIndex == nIndex);

				for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
					if (i != nIndex) {
						m_layMain->AddAt(m_pGrids[i],i);
					}
				}

				m_layMain->SetFixedColumns(g_dwLayoutColumns);
				m_nState = STATE_GRIDS;
				m_nMaxGridIndex = -1;
				m_pMyImage[nIndex]->SetState(m_nState);
				ReLayout(m_layMain->GetWidth(), m_layMain->GetHeight());
			}
			
			break;
		}
		pFindControl = pFindControl->GetParent();
	}
}

void   CDuiFrameWnd::OnUpdateGridScroll(WPARAM wParam, LPARAM lParam) {

	if ( m_nState != STATE_MAXIUM ) {
		return;
	}

	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)m_pMyImage[m_nMaxGridIndex]->GetParent();
	SIZE tParentScrollPos   = pParent->GetScrollPos();
	SIZE tParentScrollRange = pParent->GetScrollRange();
	if (tParentScrollPos.cx != tParentScrollRange.cx ) {
		pParent->SetScrollPos(tParentScrollRange);
	}	
}

void   CDuiFrameWnd::OnMyImageClick(const POINT * pPoint) {
	if ( m_nState != STATE_MAXIUM ) {
		return;
	}

	//JTelSvrPrint("click on my image, x = %d, y = %d", pPoint->x, pPoint->y);
	m_pMyImage[m_nMaxGridIndex]->OnMyClick( pPoint );	
}

void   CDuiFrameWnd::OnEdtRemarkKillFocus() {
	DuiLib::CDuiString  strRemark = m_edRemark->GetText();
	m_edRemark->SetText("");
	m_edRemark->SetVisible(false);

	assert(m_nMaxGridIndex >= 0);
	if (m_nMaxGridIndex < 0) {
		return;
	}

	m_pMyImage[m_nMaxGridIndex]->SetRemark(strRemark);
}

void  CDuiFrameWnd::OnFinalMessage(HWND hWnd) {
	// 销毁没有添加进layMain的grids
	if ( m_nState == STATE_MAXIUM ) {
		for ( DWORD i = 0; i < MAX_GRID_COUNT; i++ ) {
			if (m_nMaxGridIndex != i) {
				m_pGrids[i]->Delete();
			}
		}
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

         
             