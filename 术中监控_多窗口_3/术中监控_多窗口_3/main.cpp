// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include <assert.h>
#include "SettingDlg.h"

CDuiFrameWnd::CDuiFrameWnd() : m_callback(&m_PaintManager, this) {
	m_eGridStatus = GRID_STATUS_GRIDS;
	m_dwInflateGridIndex = -1;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	CDuiString strText;
	g_data.m_hWnd = GetHWND();
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_layMain = static_cast<CTileLayoutUI*>(m_PaintManager.FindControl(LAYOUT_MAIN_NAME));
	m_layStatus = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(LAYOUT_STATUS_NAME));

	m_layMain->SetFixedColumns(g_data.m_CfgData.m_dwLayoutColumns);
	for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
		CDialogBuilder builder;
		m_pGrids[i] = builder.Create(MYCHART_XML_FILE, (UINT)0, &m_callback, &m_PaintManager);
		m_pGrids[i]->SetBorderSize(1);
		m_pGrids[i]->SetName(GRID_NAME);
		m_pGrids[i]->SetTag(i);

		/* 多窗格显示 */
		m_layGrid[i] = static_cast<CVerticalLayoutUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, LAYOUT_GRID_NAME, 0));
		m_layGrid[i]->SetTag(i);
		m_layGrid[i]->SetVisible(true);

		strText.Format("%lu", i + 1);
		m_LblIndex_grid[i] = static_cast<CLabelUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, LABEL_INDEX_NAME_GRID, 0));
		m_LblIndex_grid[i]->SetTag(i);
		m_LblIndex_grid[i]->SetText(strText);		

		m_BtnBed_grid[i] = static_cast<CButtonUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, BUTTON_BED_NAME_GRID, 0));
		m_BtnBed_grid[i]->SetTag(i);
		m_BtnBed_grid[i]->SetVisible(true);
		m_BtnBed_grid[i]->SetText( g_data.m_CfgData.m_GridCfg[i].m_szBed );

		m_EdtBed_grid[i] = static_cast<CEditUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, EDIT_BED_NAME_GRID, 0));
		m_EdtBed_grid[i]->SetTag(i);
		m_EdtBed_grid[i]->SetVisible(false);
		m_EdtBed_grid[i]->SetText(g_data.m_CfgData.m_GridCfg[i].m_szBed);

		m_BtnName_grid[i] = static_cast<CButtonUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, BUTTON_NAME_NAME_GRID, 0));
		m_BtnName_grid[i]->SetTag(i);
		m_BtnName_grid[i]->SetVisible(true);
		m_BtnName_grid[i]->SetText(g_data.m_CfgData.m_GridCfg[i].m_szName);

		m_EdtName_grid[i] = static_cast<CEditUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, EDIT_NAME_NAME_GRID, 0));
		m_EdtName_grid[i]->SetTag(i);
		m_EdtName_grid[i]->SetVisible(false);
		m_EdtName_grid[i]->SetText(g_data.m_CfgData.m_GridCfg[i].m_szName);

		m_BtnBed_max[i] = static_cast<CButtonUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, BUTTON_BED_NAME_MAX, 0));
		m_BtnBed_max[i]->SetTag(i);
		m_BtnBed_max[i]->SetVisible(true);
		m_BtnBed_max[i]->SetText(g_data.m_CfgData.m_GridCfg[i].m_szBed);

		m_EdtBed_max[i] = static_cast<CEditUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, EDIT_BED_NAME_MAX, 0));
		m_EdtBed_max[i]->SetTag(i);
		m_EdtBed_max[i]->SetVisible(false);
		m_EdtBed_max[i]->SetText(g_data.m_CfgData.m_GridCfg[i].m_szBed);

		m_BtnName_max[i] = static_cast<CButtonUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, BUTTON_NAME_NAME_MAX, 0));
		m_BtnName_max[i]->SetTag(i);
		m_BtnName_max[i]->SetVisible(true);
		m_BtnName_max[i]->SetText(g_data.m_CfgData.m_GridCfg[i].m_szName);

		m_EdtName_max[i] = static_cast<CEditUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, EDIT_NAME_NAME_MAX, 0));
		m_EdtName_max[i]->SetTag(i);
		m_EdtName_max[i]->SetVisible(false);
		m_EdtName_max[i]->SetText(g_data.m_CfgData.m_GridCfg[i].m_szName);

		m_LblCurTemp_grid[i] = static_cast<CLabelUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, LABEL_CUR_TEMP_GRID, 0));
		m_LblCurTemp_grid[i]->SetTag(i);
		m_LblCurTemp_grid[i]->SetText("--");

		m_LblBedTitle_grid[i] = static_cast<CLabelUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, LABEL_BED_TITLE_GRID, 0));
		m_LblBedTitle_grid[i]->SetTag(i);

		m_LblNameTitle_grid[i] = static_cast<CLabelUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, LABEL_NAME_TITLE_GRID, 0));
		m_LblNameTitle_grid[i]->SetTag(i);

		m_LblCurTempTitle_grid[i] = static_cast<CLabelUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, LABEL_CUR_TITLE_GRID, 0));
		m_LblCurTempTitle_grid[i]->SetTag(i);

		m_MyImage_grid[i] = static_cast<CMyImageUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, MYIMAGE_NAME, 0));
		m_MyImage_grid[i]->SetTag(i);

		m_OptGridSwitch_grid[i] = static_cast<COptionUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, OPT_GRID_SWITCH_GRID, 0));
		m_OptGridSwitch_grid[i]->SetTag(i);
		m_OptGridSwitch_grid[i]->Selected(g_data.m_CfgData.m_GridCfg[i].m_bSwitch ? true : false);

		m_MyAlarm_grid[i] = static_cast<CAlarmImageUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, ALARM_NAME, 0));
		m_MyAlarm_grid[i]->SetTag(i);
		/* END 多窗格显示 */

		/* 最大化显示 */
		m_layMaxium[i] = static_cast<CHorizontalLayoutUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, LAYOUT_MAXIUM_NAME, 0));
		m_layMaxium[i]->SetTag(i);
		m_layMaxium[i]->SetVisible(false);

		m_LblIndex_maxium[i] = static_cast<CLabelUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, LABEL_INDEX_NAME_MAXIUM, 0));
		m_LblIndex_maxium[i]->SetTag(i);
		strText.Format("%lu", i + 1);
		m_LblIndex_maxium[i]->SetText(strText);

		m_OptGridSwitch_max[i] = static_cast<COptionUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, OPT_GRID_SWITCH_MAX, 0));
		m_OptGridSwitch_max[i]->SetTag(i);
		m_OptGridSwitch_max[i]->Selected(g_data.m_CfgData.m_GridCfg[i].m_bSwitch ? true : false);	

		m_lblTempTitle[i] = static_cast<CLabelUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, "lblTempTitle", 0));
		m_lblTempTitle[i]->SetTag(i);

		m_lay21[i] = static_cast<CVerticalLayoutUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, "lay_21", 0));
		m_lay21[i]->SetTag(i);

		m_MyImage_max[i] = static_cast<CMyImageUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, MYIMAGE_NAME_MAXIUM, 0));
		m_MyImage_max[i]->SetTag(i);

		m_layReaders[i] = static_cast<CVerticalLayoutUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, LAYOUT_READERS, 0));
		m_layReaders[i]->SetTag(i);

		for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
			CDialogBuilder builder_child;
			m_UiReaders[i][j] = builder_child.Create(READER_FILE_NAME, (UINT)0, &m_callback, &m_PaintManager);
			m_UiReaders[i][j]->SetTag(j);

			m_UiIndicator[i][j] = static_cast<CHorizontalLayoutUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, CTL_INDICATOR_NAME, 0));
			m_UiIndicator[i][j]->SetTag(j);

			m_UiLayReader[i][j] = static_cast<CHorizontalLayoutUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, LAY_READER_NAME, 0));
			m_UiLayReader[i][j]->SetTag(j);

			m_UiReaderSwitch[i][j] = static_cast<COptionUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, READER_SWITCH_NAME, 0));
			m_UiReaderSwitch[i][j]->SetTag( MAKELONG(i,j) );
			
			m_UiReaderTemp[i][j] = static_cast<CLabelUI *>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, READER_TEMP_NAME, 0));
			m_UiReaderTemp[i][j]->SetTag(j);

			m_UiBtnReaderNames[i][j] = static_cast<CButtonUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, BTN_READER_NAME, 0));
			m_UiBtnReaderNames[i][j]->SetTag( MAKELONG(i, j) );

			m_UiEdtReaderNames[i][j] = static_cast<CEditUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, EDT_READER_NAME, 0));
			m_UiEdtReaderNames[i][j]->SetTag(MAKELONG(i, j));

			m_UiAlarms[i][j] = static_cast<CAlarmImageUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, ALARM_IMAGE_NAME, 0));
			m_UiAlarms[i][j]->SetTag(j);

			m_UiIndicator[i][j]->SetBkColor( g_data.m_skin.GetReaderIndicator(j) );
			m_UiReaderTemp[i][j]->SetText("--");
			m_UiBtnReaderNames[i][j]->SetText(g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_szName);
			m_UiReaderSwitch[i][j]->Selected(g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch ? true : false);

			m_layReaders[i]->Add(m_UiReaders[i][j]);
		}
		/* END 最大化显示 */

		if ( i >= g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows ) {
			m_pGrids[i]->SetVisible(false);
		}
		m_layMain->Add(m_pGrids[i]);
		 
		m_MyAlarm_grid[i]->StartAlarm(CAlarmImageUI::DISCONNECTED);
		for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
			m_UiAlarms[i][j]->StartAlarm(CAlarmImageUI::DISCONNECTED);
		}
	} 

	m_btnMenu = static_cast<CButtonUI*>(m_PaintManager.FindControl(BTN_MENU_NAME));

	OnChangeSkin();

#if TEST_FLAG
	SetTimer(m_hWnd, TIMER_TEST_ID, TIMER_TEST_INTERVAL, NULL);
	SetTimer(m_hWnd, TIMER_TEST_ID_1, TIMER_TEST_INTERVAL_1, NULL);
#endif

	WindowImplBase::InitWindow();
}

LRESULT CDuiFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	return WindowImplBase::OnCreate(uMsg, wParam, lParam, bHandled);
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString & name = msg.pSender->GetName();

	if ( msg.sType == "click" ) {
		if (name == BTN_MENU_NAME) {
			OnBtnMenu(msg);
		}
		else if (name == BUTTON_BED_NAME_GRID) {
			OnBtnBed_grid(msg);
		}
		else if (name == BUTTON_NAME_NAME_GRID) {
			OnBtnName_grid(msg);
		}
		else if (name == BUTTON_BED_NAME_MAX) {
			OnBtnBed_max(msg);
		}
		else if (name == BUTTON_NAME_NAME_MAX) {
			OnBtnName_max(msg);
		}
		else if ( name == OPT_GRID_SWITCH_GRID ) {
			OnGridSwitch(msg);
		}
		else if (name == OPT_GRID_SWITCH_MAX ) {
			OnGridSwitch(msg);
		}
		else if (name == READER_SWITCH_NAME) {
			OnReaderSwitch(msg);
		}
		else if (name == BTN_READER_NAME) {
			OnReaderName(msg);
		}
	}
	else if (msg.sType == "menu_setting") {
		OnSetting();
	}
	else if (msg.sType == "menu_about") {
		OnAbout();
	}
	else if (msg.sType == "killfocus") {
		if ( name == EDIT_BED_NAME_GRID ) {
			OnEdtBedKillFocus_grid(msg);
		}
		else if (name == EDIT_NAME_NAME_GRID) {
			OnEdtNameKillFocus_grid(msg);
		}
		else if (name == EDIT_BED_NAME_MAX) {
			OnEdtBedKillFocus_max(msg);
		}
		else if (name == EDIT_NAME_NAME_MAX) {
			OnEdtNameKillFocus_max(msg);
		}
		else if ( name == EDT_READER_NAME ) {
			OnEdtReaderNameKillFocus(msg);
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_SIZE) {
		OnSize(wParam, lParam);
	}
	else if (uMsg == WM_LBUTTONDBLCLK) {
		OnDbClick();
	}
	else if (uMsg == WM_TIMER) {
#if TEST_FLAG
		if ( wParam == TIMER_TEST_ID || wParam == TIMER_TEST_ID_1 ) {
			OnTestTimer(wParam);
		}
#endif		
	}
	else if (uMsg == UM_UPDATE_SCROLL) {
		OnUpdateGridScroll(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void CDuiFrameWnd::OnFinalMessage(HWND hWnd) {
	// 销毁没有添加进layMain的grids
	if (m_eGridStatus == GRID_STATUS_MAXIUM ) {
		for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
			if (m_dwInflateGridIndex != i) {
				m_pGrids[i]->Delete();
			}
		}
	}
}

void   CDuiFrameWnd::OnSize(WPARAM wParam, LPARAM lParam) {
	DWORD dwWidth = LOWORD(lParam) - LAYOUT_WINDOW_HMARGIN * 2;           // layWindow的左右margin
																		  // 30是底部status的大致高度, 1是layWindow的上下margin, 32是标题栏高度
	DWORD dwHeight = HIWORD(lParam) - STATUS_PANE_HEIGHT - LAYOUT_WINDOW_VMARGIN * 2 - WINDOW_TITLE_HEIGHT;
	ReLayout(dwWidth, dwHeight);
}

// 重新布局
void   CDuiFrameWnd::ReLayout(DWORD dwWidth, DWORD dwHeight) {
	if (0 == m_layMain) {
		return;
	}

	SIZE s;
	if (GRID_STATUS_GRIDS == m_eGridStatus) {
		s.cx = (dwWidth - 1) / g_data.m_CfgData.m_dwLayoutColumns + 1;
		s.cy = (dwHeight - 1) / g_data.m_CfgData.m_dwLayoutRows + 1;
		m_layMain->SetItemSize(s);

		// 重新修正标题栏的高度
		m_layStatus->SetFixedHeight(STATUS_PANE_HEIGHT - (s.cy * g_data.m_CfgData.m_dwLayoutRows - dwHeight));
	}
	else {
		s.cx = dwWidth;
		s.cy = dwHeight;
		m_layMain->SetItemSize(s);

		// 重新修正标题栏的高度
		m_layStatus->SetFixedHeight(STATUS_PANE_HEIGHT);
	}
}

void   CDuiFrameWnd::OnChangeSkin() {
	m_layMain->SetBkColor(g_data.m_skin[CMySkin::LAYOUT_MAIN_BK]);
	m_layStatus->SetBkColor(g_data.m_skin[CMySkin::LABEL_STATUS_BK]);

	for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
		m_pGrids[i]->SetBorderColor(g_data.m_skin[CMySkin::GRID_BORDER]);
		m_LblIndex_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_LblIndex_maxium[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_BtnBed_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_EdtBed_grid[i]->SetTextColor(g_data.m_skin[CMySkin::EDIT_TEXT]);
		m_EdtBed_grid[i]->SetBkColor(g_data.m_skin[CMySkin::EDIT_BK]);
		m_EdtBed_grid[i]->SetNativeEditBkColor(g_data.m_skin[CMySkin::EDIT_BK]);
		m_BtnName_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_EdtName_grid[i]->SetTextColor(g_data.m_skin[CMySkin::EDIT_TEXT]);
		m_EdtName_grid[i]->SetBkColor(g_data.m_skin[CMySkin::EDIT_BK]);
		m_EdtName_grid[i]->SetNativeEditBkColor(g_data.m_skin[CMySkin::EDIT_BK]);
		m_BtnBed_max[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_EdtBed_max[i]->SetTextColor(g_data.m_skin[CMySkin::EDIT_TEXT]);
		m_EdtBed_max[i]->SetBkColor(g_data.m_skin[CMySkin::EDIT_BK]);
		m_EdtBed_max[i]->SetNativeEditBkColor(g_data.m_skin[CMySkin::EDIT_BK]);
		m_BtnName_max[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_EdtName_max[i]->SetTextColor(g_data.m_skin[CMySkin::EDIT_TEXT]);
		m_EdtName_max[i]->SetBkColor(g_data.m_skin[CMySkin::EDIT_BK]);
		m_EdtName_max[i]->SetNativeEditBkColor(g_data.m_skin[CMySkin::EDIT_BK]);
		m_LblCurTemp_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_LblBedTitle_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_LblNameTitle_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_LblCurTempTitle_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_MyImage_grid[i]->SetBkColor(g_data.m_skin[CMySkin::MYIMAGE_BK]);
		m_MyImage_grid[i]->OnChangeSkin();
		m_OptGridSwitch_grid[i]->SetSelectedImage(g_data.m_skin.GetImageName(CMySkin::OPT_SELECTED));
		m_OptGridSwitch_grid[i]->SetNormalImage(g_data.m_skin.GetImageName(CMySkin::OPT_NOT_SELECTED));
		m_layMaxium[i]->SetBkColor(g_data.m_skin[CMySkin::LAYOUT_MAIN_BK]);
		m_lblTempTitle[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_lay21[i]->SetBkColor(g_data.m_skin[CMySkin::LAYOUT_MAIN_BK]);
		m_lay21[i]->SetBkColor2(g_data.m_skin[CMySkin::LAYOUT_MAIN_BK_1]);
		m_lay21[i]->FindControl(CS_FINDCONTROLPROC, 0, 0);	
		m_MyImage_max[i]->SetBkColor(g_data.m_skin[CMySkin::MYIMAGE_BK]);
		m_MyImage_max[i]->OnChangeSkin(); 

		for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
			m_UiBtnReaderNames[i][j]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
			m_UiEdtReaderNames[i][j]->SetTextColor(g_data.m_skin[CMySkin::EDIT_TEXT]);
			m_UiEdtReaderNames[i][j]->SetBkColor(g_data.m_skin[CMySkin::EDIT_BK]);
			m_UiEdtReaderNames[i][j]->SetNativeEditBkColor(g_data.m_skin[CMySkin::EDIT_BK]);
		}
	} 
}

void   CDuiFrameWnd::OnDbClick() {
	POINT point;
	CDuiString strName;

	GetCursorPos(&point);
	::ScreenToClient(m_hWnd, &point);
	CControlUI * pFindControl = m_PaintManager.FindSubControlByPoint(m_layMain, point);
	if (pFindControl) {
		// 如果双击按钮，返回
		if (0 == strcmp(pFindControl->GetClass(), DUI_CTR_BUTTON)) {
			return;
		}
		else if (0 == strcmp(pFindControl->GetClass(), DUI_CTR_OPTION)) {
			return;
		}
	}

	while (pFindControl) {
		strName = pFindControl->GetName();
		if (0 == strcmp(strName, GRID_NAME)) {
			DWORD nIndex = pFindControl->GetTag();
			OnGridInflate(nIndex);
			break;
		}
		pFindControl = pFindControl->GetParent();
	}
}

// 窗格扩大或缩小
void  CDuiFrameWnd::OnGridInflate(DWORD dwIndex) {
	if ( m_eGridStatus == GRID_STATUS_GRIDS ) {
		for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
			if (i != dwIndex) {
				m_layMain->Remove(m_pGrids[i], true);
			}
		}
		m_layMain->SetFixedColumns(1);
		m_eGridStatus = GRID_STATUS_MAXIUM;
		m_dwInflateGridIndex = dwIndex;
		OnGridInflateSub(dwIndex);
		ReLayout(m_layMain->GetWidth(), m_layMain->GetHeight());
	}
	else {
		assert(GRID_STATUS_MAXIUM == m_eGridStatus);
		assert(m_dwInflateGridIndex == dwIndex);

		for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
			if (i != dwIndex) {
				m_layMain->AddAt(m_pGrids[i], i);
			}
		}

		m_layMain->SetFixedColumns( g_data.m_CfgData.m_dwLayoutColumns );
		m_eGridStatus = GRID_STATUS_GRIDS;
		m_dwInflateGridIndex = -1;
		OnGridInflateSub(dwIndex);
		ReLayout(m_layMain->GetWidth(), m_layMain->GetHeight());
		OnEdtRemarkKillFocus();
	}
}

void   CDuiFrameWnd::OnGridInflateSub( DWORD dwIndex ) {
	assert(dwIndex >= 0 && dwIndex < MAX_GRID_COUNT);

	if (m_eGridStatus == GRID_STATUS_MAXIUM) {
		m_layGrid[dwIndex]->SetVisible(false);
		m_layMaxium[dwIndex]->SetVisible(true);
	}
	else {
		m_layGrid[dwIndex]->SetVisible(true);
		m_layMaxium[dwIndex]->SetVisible(false);
	}
}
   
void   CDuiFrameWnd::OnEdtRemarkKillFocus() {

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
	BOOL   bValue = FALSE;

	CfgData  oldData = g_data.m_CfgData;

	CSettingDlg * pSettingDlg = new CSettingDlg;
	pSettingDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pSettingDlg->CenterWindow();
	int ret = pSettingDlg->ShowModal();
	// 如果是click ok
	if (0 == ret) {
		g_data.m_CfgData = pSettingDlg->m_data;

		dwValue = DEFAULT_MAIN_LAYOUT_COLUMNS;
		g_data.m_cfg->SetConfig( CFG_MAIN_LAYOUT_COLUMNS, g_data.m_CfgData.m_dwLayoutColumns, &dwValue);

		dwValue = DEFAULT_MAIN_LAYOUT_ROWS;
		g_data.m_cfg->SetConfig(CFG_MAIN_LAYOUT_ROWS, g_data.m_CfgData.m_dwLayoutRows, &dwValue);

		dwValue = 0;
		g_data.m_cfg->SetConfig(CFG_AREA_ID_NAME, g_data.m_CfgData.m_dwAreaNo, &dwValue);

		bValue = FALSE;
		g_data.m_cfg->SetBooleanConfig(CFG_ALARM_VOICE_SWITCH, g_data.m_CfgData.m_bAlarmVoiceOff, &bValue);

		dwValue = DEFAULT_SKIN;
		g_data.m_cfg->SetConfig(CFG_SKIN, g_data.m_CfgData.m_dwSkinIndex, &dwValue);

		for (int i = 0; i < MAX_GRID_COUNT; i++) {
			strText.Format("%s %lu", CFG_GRID_SWITCH, i + 1);
			bValue = FALSE;
			g_data.m_cfg->SetBooleanConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_bSwitch, &bValue);
			m_OptGridSwitch_max[i]->Selected(g_data.m_CfgData.m_GridCfg[i].m_bSwitch ? true : false);
			m_OptGridSwitch_grid[i]->Selected(g_data.m_CfgData.m_GridCfg[i].m_bSwitch ? true : false);

			strText.Format("%s %lu", CFG_COLLECT_INTERVAL, i + 1);
			dwValue = 0;
			g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval, &dwValue);

			strText.Format("%s %lu", CFG_MIN_TEMP, i + 1);
			dwValue = DEFAULT_MIN_TEMP_INDEX;
			g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp, &dwValue);

			for (int j = 0; j < MAX_READERS_PER_GRID; j++) {
				strText.Format("%s %lu %lu", CFG_READER_SWITCH, i + 1, j + 1);
				bValue = DEFAULT_READER_SWITCH;
				g_data.m_cfg->SetBooleanConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch, &bValue);
				m_UiReaderSwitch[i][j]->Selected(g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch ? true : false);

				strText.Format("%s %lu %lu", CFG_LOW_TEMP_ALARM, i + 1, j + 1);
				dwValue = 3500;
				g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm, &dwValue);

				strText.Format("%s %lu %lu", CFG_HIGH_TEMP_ALARM, i + 1, j + 1);
				dwValue = 4000;
				g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm, &dwValue);

				strText.Format("%s %lu %lu", CFG_BED_NO, i + 1, j + 1);
				dwValue = 0;
				g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwBed, &dwValue);
			}
		}

		g_data.m_cfg->Save();

		// 程序内存变动
		if (oldData.m_dwSkinIndex != g_data.m_CfgData.m_dwSkinIndex) {
			g_data.m_skin.SetSkin((CMySkin::ENUM_SKIN)g_data.m_CfgData.m_dwSkinIndex);
			OnChangeSkin();
		}

		if (oldData.m_dwLayoutColumns != g_data.m_CfgData.m_dwLayoutColumns || oldData.m_dwLayoutRows != g_data.m_CfgData.m_dwLayoutRows) {
			UpdateLayout();
		}
		
		::InvalidateRect(this->GetHWND(), 0, TRUE);
	}
	delete pSettingDlg;
}

// 重新布局(在设置更改之后)
void   CDuiFrameWnd::UpdateLayout() {
	DWORD dwInvisibleIndex = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
		if (i < dwInvisibleIndex) {
			m_pGrids[i]->SetVisible(true);
		}
		else {
			m_pGrids[i]->SetVisible(false);
		}
	}

	if (m_eGridStatus == GRID_STATUS_GRIDS) {
		m_layMain->SetFixedColumns(g_data.m_CfgData.m_dwLayoutColumns);
		ReLayout(m_layMain->GetWidth(), m_layMain->GetHeight());
	}
	else {
		assert(m_eGridStatus == GRID_STATUS_MAXIUM);
		// 如果当前格子超出范围
		if (m_dwInflateGridIndex >= (int)(g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows)) {
			m_layMain->Remove(m_pGrids[m_dwInflateGridIndex], true);
			for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
				m_layMain->AddAt(m_pGrids[i], i);
			}
			m_layMain->SetFixedColumns(g_data.m_CfgData.m_dwLayoutColumns);
			m_eGridStatus = GRID_STATUS_GRIDS;
			m_dwInflateGridIndex = -1;
			ReLayout(m_layMain->GetWidth(), m_layMain->GetHeight());
		}
	}

}

void   CDuiFrameWnd::OnAbout() {

}

void   CDuiFrameWnd::OnBtnBed_grid(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	msg.pSender->SetVisible(false);
	m_EdtBed_grid[nIndex]->SetText(msg.pSender->GetText());
	m_EdtBed_grid[nIndex]->SetVisible(true);
	m_EdtBed_grid[nIndex]->SetFocus();
}

void   CDuiFrameWnd::OnEdtBedKillFocus_grid(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	STRNCPY( g_data.m_CfgData.m_GridCfg[nIndex].m_szBed, msg.pSender->GetText(), MAX_BED_LENGTH);

	CDuiString  strText;
	strText.Format(CFG_BED_NAME " %d", nIndex + 1);
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[nIndex].m_szBed, "");
	g_data.m_cfg->Save();

	msg.pSender->SetVisible(false);
	m_BtnBed_grid[nIndex]->SetText(msg.pSender->GetText());
	m_BtnBed_grid[nIndex]->SetVisible(true);

	m_BtnBed_max[nIndex]->SetText(msg.pSender->GetText());
}

void  CDuiFrameWnd::OnBtnName_grid(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	msg.pSender->SetVisible(false);
	m_EdtName_grid[nIndex]->SetText(msg.pSender->GetText());
	m_EdtName_grid[nIndex]->SetVisible(true);
	m_EdtName_grid[nIndex]->SetFocus();
}

void    CDuiFrameWnd::OnEdtNameKillFocus_grid(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	STRNCPY(g_data.m_CfgData.m_GridCfg[nIndex].m_szName, msg.pSender->GetText(), MAX_NAME_LENGTH);

	CDuiString  strText;
	strText.Format(CFG_PATIENT_NAME " %d", nIndex + 1);
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[nIndex].m_szName, "");
	g_data.m_cfg->Save();

	msg.pSender->SetVisible(false);
	m_BtnName_grid[nIndex]->SetText(msg.pSender->GetText());
	m_BtnName_grid[nIndex]->SetVisible(true);

	m_BtnName_max[nIndex]->SetText(msg.pSender->GetText());
}

void    CDuiFrameWnd::OnGridSwitch(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	COptionUI * p = (COptionUI*)msg.pSender;
	g_data.m_CfgData.m_GridCfg[nIndex].m_bSwitch = !p->IsSelected();
	CDuiString  strText;
	strText.Format("%s %lu", CFG_GRID_SWITCH, nIndex + 1);
	g_data.m_cfg->SetBooleanConfig(strText, g_data.m_CfgData.m_GridCfg[nIndex].m_bSwitch, DEFAULT_READER_SWITCH);
	g_data.m_cfg->Save();

	CDuiString name = msg.pSender->GetName();
	if (name == OPT_GRID_SWITCH_GRID) {
		m_OptGridSwitch_max[nIndex]->Selected(g_data.m_CfgData.m_GridCfg[nIndex].m_bSwitch ? true : false);
	}
	else if (name == OPT_GRID_SWITCH_MAX) {
		m_OptGridSwitch_grid[nIndex]->Selected(g_data.m_CfgData.m_GridCfg[nIndex].m_bSwitch ? true : false);
	}     
} 

void   CDuiFrameWnd::OnBtnBed_max(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	msg.pSender->SetVisible(false);
	m_EdtBed_max[nIndex]->SetText(msg.pSender->GetText());
	m_EdtBed_max[nIndex]->SetVisible(true);
	m_EdtBed_max[nIndex]->SetFocus();
}

void   CDuiFrameWnd::OnBtnName_max(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	msg.pSender->SetVisible(false);
	m_EdtName_max[nIndex]->SetText(msg.pSender->GetText());
	m_EdtName_max[nIndex]->SetVisible(true);
	m_EdtName_max[nIndex]->SetFocus();
}

void   CDuiFrameWnd::OnEdtBedKillFocus_max(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	STRNCPY(g_data.m_CfgData.m_GridCfg[nIndex].m_szBed, msg.pSender->GetText(), MAX_BED_LENGTH);

	CDuiString  strText;
	strText.Format(CFG_BED_NAME " %d", nIndex + 1);
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[nIndex].m_szBed, "");
	g_data.m_cfg->Save();

	msg.pSender->SetVisible(false);
	m_BtnBed_max[nIndex]->SetText(msg.pSender->GetText());
	m_BtnBed_max[nIndex]->SetVisible(true);

	m_BtnBed_grid[nIndex]->SetText(msg.pSender->GetText());
}

void   CDuiFrameWnd::OnEdtNameKillFocus_max(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	STRNCPY(g_data.m_CfgData.m_GridCfg[nIndex].m_szName, msg.pSender->GetText(), MAX_NAME_LENGTH);

	CDuiString  strText;
	strText.Format(CFG_PATIENT_NAME " %d", nIndex + 1);
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[nIndex].m_szName, "");
	g_data.m_cfg->Save();

	msg.pSender->SetVisible(false);
	m_BtnName_max[nIndex]->SetText(msg.pSender->GetText());
	m_BtnName_max[nIndex]->SetVisible(true);

	m_BtnName_grid[nIndex]->SetText(msg.pSender->GetText());
}

void   CDuiFrameWnd::OnReaderSwitch(TNotifyUI& msg) {
	DWORD dwTag = msg.pSender->GetTag();
	DWORD dwIndex = LOWORD(dwTag);
	DWORD dwSubIndex = HIWORD(dwTag);

	COptionUI * p = (COptionUI*)msg.pSender;
	g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_bSwitch = !p->IsSelected();
	CDuiString  strText;
	strText.Format("%s %lu %lu", CFG_READER_SWITCH, dwIndex + 1, dwSubIndex + 1);
	g_data.m_cfg->SetBooleanConfig(strText, g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_bSwitch, DEFAULT_READER_SWITCH);
	g_data.m_cfg->Save();
}

void   CDuiFrameWnd::OnTestTimer(DWORD  dwTimer) {
	if (dwTimer == TIMER_TEST_ID ) {
		DWORD dwTmp = GetRand(3600, 3800);
		m_MyImage_grid[0]->AddTemp(0, dwTmp);
		m_MyImage_max[0]->AddTemp(0, dwTmp);
	}
	else if (dwTimer == TIMER_TEST_ID_1) {
		DWORD dwTmp = GetRand(3400, 3600);
		m_MyImage_grid[0]->AddTemp(1, dwTmp);
		m_MyImage_max[0]->AddTemp(1, dwTmp);
	}	
}

void   CDuiFrameWnd::OnUpdateGridScroll(WPARAM wParam, LPARAM lParam) {
	DWORD  dwIndex = wParam;
	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)m_MyImage_max[dwIndex]->GetParent();
	SIZE tParentScrollPos = pParent->GetScrollPos();
	SIZE tParentScrollRange = pParent->GetScrollRange();
	if (tParentScrollPos.cx != tParentScrollRange.cx) {
		pParent->SetScrollPos(tParentScrollRange);
	}
}

void  CDuiFrameWnd::OnReaderName(TNotifyUI& msg) {
	DWORD  dwTag      = msg.pSender->GetTag();
	DWORD  dwIndex    = LOWORD(dwTag);
	DWORD  dwSubIndex = HIWORD(dwTag);
	msg.pSender->SetVisible(false);
	m_UiEdtReaderNames[dwIndex][dwSubIndex]->SetText(msg.pSender->GetText());
	m_UiEdtReaderNames[dwIndex][dwSubIndex]->SetVisible(true);
	m_UiEdtReaderNames[dwIndex][dwSubIndex]->SetFocus();
}

void   CDuiFrameWnd::OnEdtReaderNameKillFocus(TNotifyUI& msg) {
	DWORD  dwTag      = msg.pSender->GetTag();
	DWORD  dwIndex    = LOWORD(dwTag);
	DWORD  dwSubIndex = HIWORD(dwTag);

	STRNCPY( g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_szName, 
		     msg.pSender->GetText(), 
		     sizeof(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_szName));

	CDuiString  strText;
	strText.Format(CFG_READER_NAME " %lu %lu", dwIndex + 1, dwSubIndex + 1);
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_szName, "");
	g_data.m_cfg->Save();

	msg.pSender->SetVisible(false);
	m_UiBtnReaderNames[dwIndex][dwSubIndex]->SetText(msg.pSender->GetText());
	m_UiBtnReaderNames[dwIndex][dwSubIndex]->SetVisible(true);
}




int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

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

	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}
            

