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
#include <time.h>
#include "SettingDlg.h"
#include "LmnTelSvr.h"
#include "AboutDlg.h"
#include "BindingDlg.h"

CDuiFrameWnd::CDuiFrameWnd() : m_callback(&m_PaintManager, this) {
	m_eGridStatus = GRID_STATUS_GRIDS;
	m_dwInflateGridIndex = -1;
	memset(m_tLastTemp, 0, sizeof(m_tLastTemp));
	memset(m_tTagBinding, 0, sizeof(m_tTagBinding));	
	m_layMain = 0;
	m_LastSaveExcelTime = 0;
	m_nSelTabIndex = 0;
	m_dragDropTagIndex = -1;
	m_dragDropGridIndex = -1;

	m_dragDropGridIndex_1 = -1;
	m_dragDropGridIndex_2 = -1;
}

CDuiFrameWnd::~CDuiFrameWnd() {
	
}
       
void  CDuiFrameWnd::InitWindow() {
	CDuiString strText;
	g_data.m_hWnd = GetHWND();
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	g_hc = LoadCursor(NULL, IDC_SIZENS);

	m_layMain = static_cast<CTileLayoutUI*>(m_PaintManager.FindControl(LAYOUT_MAIN_NAME));
	m_layStatus = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(LAYOUT_STATUS_NAME));
	m_lblLaunchStatus = static_cast<CLabelUI*>(m_PaintManager.FindControl(LABEL_STATUS_NAME));
	m_lblBarTips = static_cast<CLabelUI*>(m_PaintManager.FindControl("lblBarTips"));
	g_edRemark = static_cast<CEditUI*>(m_PaintManager.FindControl("edRemark"));
	g_edRemark_1 = static_cast<CEditUI*>(m_PaintManager.FindControl("edRemark_1"));
	m_dragDropTag = m_PaintManager.FindControl("dragDropTag");
	g_dragDropTag = m_dragDropTag;
	m_lblProcTips = static_cast<CLabelUI*>(m_PaintManager.FindControl("lblProcessTips"));
	m_LblDbStatus = static_cast<CLabelUI*>(m_PaintManager.FindControl("lblDbTips"));
	m_LblConflictTips = static_cast<CLabelUI*>(m_PaintManager.FindControl("lblConflict"));
	m_layMain_1 = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl("layMain_1"));
	m_MyImage_hand_reader = static_cast<CMyImageUI_1*>(m_PaintManager.FindControl("my_image_hand_reader"));
	m_layTags = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl("layTags"));
	m_layHandTagView = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl("layHandTagView"));
	m_layGridsView = static_cast<CTileLayoutUI *>(m_PaintManager.FindControl("layGridsView"));	

	m_layMain->SetFixedColumns(g_data.m_CfgData.m_dwLayoutColumns);            
	for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
		CDialogBuilder builder;
		m_pGrids[i] = builder.Create(MYCHART_XML_FILE, (UINT)0, &m_callback, &m_PaintManager);
		m_pGrids[i]->SetBorderSize(4);           
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

		m_BtnEmpty[i] = static_cast<CButtonUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, "btnEmpty", 0));
		m_BtnEmpty[i]->SetTag(i);

		m_LblBodyPart[i] = static_cast<CLabelUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, "lblBodyPart", 0));
		m_LblBodyPart[i]->SetTag(i);
		m_LblBodyPart[i]->SetText("A");

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
#if DB_FLAG
		m_BtnName_grid[i]->SetText("--");
#else
		m_BtnName_grid[i]->SetText(g_data.m_CfgData.m_GridCfg[i].m_szName);
#endif

		m_EdtName_grid[i] = static_cast<CEditUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, EDIT_NAME_NAME_GRID, 0));
		m_EdtName_grid[i]->SetTag(i);
		m_EdtName_grid[i]->SetVisible(false);
#if !DB_FLAG
		m_EdtName_grid[i]->SetText(g_data.m_CfgData.m_GridCfg[i].m_szName);
#endif

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
#if DB_FLAG
		m_BtnName_max[i]->SetText("--");
#else
		m_BtnName_max[i]->SetText(g_data.m_CfgData.m_GridCfg[i].m_szName);
#endif

		m_EdtName_max[i] = static_cast<CEditUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, EDIT_NAME_NAME_MAX, 0));
		m_EdtName_max[i]->SetTag(i);
		m_EdtName_max[i]->SetVisible(false);
#if !DB_FLAG
		m_EdtName_max[i]->SetText(g_data.m_CfgData.m_GridCfg[i].m_szName);
#endif

		m_LblCurTemp_grid[i] = static_cast<CLabelUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, LABEL_CUR_TEMP_GRID, 0));
		m_LblCurTemp_grid[i]->SetTag(i);
		m_LblCurTemp_grid[i]->SetText("--");

		m_LblCurTemp_grid1[i] = static_cast<CMyLabelUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, LABEL_CUR_TEMP_GRID1, 0));
		m_LblCurTemp_grid1[i]->SetTag(i);
		m_LblCurTemp_grid1[i]->SetText("--");
		m_LblCurTemp_grid1[i]->SetFont(g_data.m_CfgData.m_dwTempFont);	
		m_LblCurTemp_grid1[i]->SetVisible(true);

		m_LblCurTempTime[i] = static_cast<CLabelUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, "lblCurTime_2", 0));
		m_LblCurTempTime[i]->SetTag(i);
		m_LblCurTempTime[i]->SetText("");  

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
		m_LblCurTemp_grid1[i]->SetMyImage(m_MyImage_max[i]);

		m_layReaders[i] = static_cast<CVerticalLayoutUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, LAYOUT_READERS, 0));
		m_layReaders[i]->SetTag(i);

		m_btnBinding[i] = static_cast<CButtonUI*>(m_pGrids[i]->FindControl(MY_FINDCONTROLPROC, "btnBinding", 0));
		m_btnBinding[i]->SetTag(i);
		m_btnBinding[i]->SetVisible(false);


		for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
			CDialogBuilder builder_child;
			m_UiReaders[i][j] = builder_child.Create(READER_FILE_NAME, (UINT)0, &m_callback, &m_PaintManager);
			m_UiReaders[i][j]->SetTag(MAKELONG(i, j));

			m_UiIndicator[i][j] = static_cast<CHorizontalLayoutUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, CTL_INDICATOR_NAME, 0));
			m_UiIndicator[i][j]->SetTag(MAKELONG(i, j));

			m_UiLayReader[i][j] = static_cast<CVerticalLayoutUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, LAY_READER_NAME, 0));
			m_UiLayReader[i][j]->SetTag( MAKELONG(i, j) );

			m_UiReaderSwitch[i][j] = static_cast<COptionUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, READER_SWITCH_NAME, 0));
			m_UiReaderSwitch[i][j]->SetTag( MAKELONG(i,j) );
			
			m_UiReaderTemp[i][j] = static_cast<CLabelUI *>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, READER_TEMP_NAME, 0));
			m_UiReaderTemp[i][j]->SetTag(MAKELONG(i, j));
			if (g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch)
				m_UiReaderTemp[i][j]->SetText("--");
			else
				m_UiReaderTemp[i][j]->SetText("");

			m_UiBtnReaderNames[i][j] = static_cast<CButtonUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, BTN_READER_NAME, 0));
			m_UiBtnReaderNames[i][j]->SetTag( MAKELONG(i, j) );

			m_UiEdtReaderNames[i][j] = static_cast<CEditUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, EDT_READER_NAME, 0));
			m_UiEdtReaderNames[i][j]->SetTag(MAKELONG(i, j));

			m_UiAlarms[i][j] = static_cast<CAlarmImageUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, ALARM_IMAGE_NAME, 0));
			m_UiAlarms[i][j]->SetTag(MAKELONG(i, j));

			m_LblReaderId[i][j] = static_cast<CLabelUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, "lblReaderId", 0));
			m_LblReaderId[i][j]->SetTag(MAKELONG(i, j));
			m_LblReaderId[i][j]->SetText("");

			m_LblTagId[i][j] = static_cast<CLabelUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, "lblTagId", 0));
			m_LblTagId[i][j]->SetTag(MAKELONG(i, j));
			m_LblTagId[i][j]->SetText("");

			m_LblTagBinding[i][j] = static_cast<CLabelUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, "lblTagBindingStatus", 0));
			m_LblTagBinding[i][j]->SetTag(MAKELONG(i, j));
			m_LblTagBinding[i][j]->SetText("");

			m_UiIndicator[i][j]->SetBkColor( g_data.m_skin.GetReaderIndicator(j) );
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

		int nSelectedIndex = 0;
		for (int k = 0; k < MAX_READERS_PER_GRID; k++) {
			if (g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[k].m_bSwitch) {
				nSelectedIndex = k;
				m_MyImage_max[i]->OnReaderSelected(k);
				m_MyImage_grid[i]->OnReaderSelected(k);				
				strText.Format("%c", 'A' + k);
				m_LblBodyPart[i]->SetText(strText);
				break;                               
			}
		}

		if (!g_data.m_CfgData.m_GridCfg[i].m_bSwitch) {
			//m_LblCurTemp_grid1[i]->SetVisible(false);
		}
		else {
			if ( nSelectedIndex >= MAX_READERS_PER_GRID ) {
				m_LblCurTemp_grid1[i]->SetVisible(false);
			}
			else {
				if (g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[nSelectedIndex].m_bSwitch) {
					m_LblCurTemp_grid1[i]->SetVisible(true);
				}
				else {
					//m_LblCurTemp_grid1[i]->SetVisible(false);
				}
			}
		}
	} 

	m_layGridsView->SetFixedColumns(6);
	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		CLabelUI * pLabel = new CLabelUI;
		strText.Format("%d", i + 1);
		pLabel->SetText(strText);
		pLabel->SetFont(10);
		pLabel->SetTextColor(0xFFFFFFFF);
		pLabel->SetBorderColor(0xFFFFFFFF);
		pLabel->SetBorderSize(1);
		pLabel->SetAttribute("align", "center"); 
		m_layGridsView->Add(pLabel);
	}

	m_btnMenu = static_cast<CButtonUI*>(m_PaintManager.FindControl(BTN_MENU_NAME));

	OnChangeSkin();

#if TEST_FLAG
	SetTimer(m_hWnd, TIMER_TEST_ID, TIMER_TEST_INTERVAL, NULL);
	SetTimer(m_hWnd, TIMER_TEST_ID_1, TIMER_TEST_INTERVAL_1, NULL);
#endif

	SetTimer(m_hWnd, TIMER_UPDATE_TIME_DESC, TIMER_UPDATE_TIME_DESC_INTERVAL, NULL);
	SetTimer(m_hWnd, TIMER_CHECK_CONFLICT,   TIMER_CHECK_CONFLICT_INTERVAL, NULL);
	SetTimer(m_hWnd, TIMER_CHECK_AUTO_SAVE,  TIMER_CHECK_AUTO_SAVE_INTERVAL, NULL);
	SetTimer(m_hWnd, TIMER_AUTO_PRUNE,       TIMER_AUTO_PRUNE_INTERVAL, NULL);

	// OnMyDeviceChanged();    // 放在获得所有的sqlite数据后，即QueryTagBindingGridsAsyn()的结果拿到后

	CBusiness::GetInstance()->QueryHandReaderTempFromSqliteAsyn(); 
	CBusiness::GetInstance()->QueryTagBindingGridsAsyn();

#if DB_FLAG
	CBusiness::GetInstance()->ReconnectDbAsyn();
#else
	m_LblDbStatus->SetText("");
#endif

	WindowImplBase::InitWindow();
}  
                
LRESULT CDuiFrameWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	return WindowImplBase::OnCreate(uMsg, wParam, lParam, bHandled);
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyImage_1")) {
		return new CMyImageUI_1(); 
	}
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
		else if (name == MYIMAGE_NAME_MAXIUM) {
			POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
			OnMyImageClick(msg.pSender->GetTag(),&pt);
		}
		else if (name == "btnEmpty") {
			OnEmpty(msg);
		}
		else if (name == "btnBinding") {
			OnBinding(msg.pSender->GetTag());
		}
		else if (name == "btnTagNickName") {
			OnBtnTagNickname(msg);
		}
		else if (name == "my_image_hand_reader") {
			POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
			m_MyImage_hand_reader->OnMyClick(&pt);
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
		else if ( name == "edRemark" ) {
			OnEdtRemarkKillFocus();
		}
		else if (name == "edtTagNickName") {
			OnEdtTagNicknameKillFocus(msg);
		}
		else if (name == "edRemark_1") {
			OnEdtRemarkKillFocus_1();
		}
	}
	else if (msg.sType == "menu") {
		if (name == MYIMAGE_NAME_MAXIUM) {
			OnMyImageMenu(msg);
		}
	}
	else if (msg.sType == "menu_export_excel") {
		OnExportExcel();
	}
	else if (msg.sType == "menu_print_excel") {
		OnPrintExcel();
	}
	else if (msg.sType == _T("selectchanged"))
	{
		if (name == _T("opn_monitor")) {
			OnTabChanged(0);
		}
		else if (name == _T("opn_reader")) {
			OnTabChanged(1);
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
		if (wParam == TIMER_UPDATE_TIME_DESC) {
			OnUpdateTimeDescTimer();
		}
		else if (wParam == TIMER_CHECK_CONFLICT) {
			OnCheckConflictTagTimer();
		}
		else if (wParam == TIMER_CHECK_AUTO_SAVE) {
			OnCheckAutoSaveTimer();
		}
		else if (wParam == TIMER_AUTO_PRUNE) {
			OnAutoPruneTimer();
		}
	}
	else if (uMsg == UM_UPDATE_SCROLL) {
		OnUpdateGridScroll(wParam, lParam);
	}
	else if (uMsg == WM_LBUTTONDOWN) {
		OnMyLButtonDown(wParam, lParam);
	}
	else if (uMsg == WM_LBUTTONUP) {
		OnMyLButtonUp(wParam, lParam);
	}
	else if (uMsg == WM_MOUSEMOVE) {
		OnMyMouseMove(wParam, lParam);
	}
	else if ( uMsg == UM_LAUNCH_STATUS ) {
		OnLaunchStatus(wParam, lParam);
	}
	else if (uMsg == UM_READER_TEMP) {
		OnReaderTemp(wParam, lParam);
	}
	else if (uMsg == UM_READER_DISCONNECTED) {
		OnReaderDisconnected(wParam, lParam);
	}
	else if (uMsg == WM_MOUSEWHEEL) {
		OnMyMouseWheel(wParam, lParam);
		return 0;
	}
	else if (uMsg == WM_DEVICECHANGE) {
		OnMyDeviceChanged();
	}
	else if (uMsg == UM_READER_PROCESSING) {
		OnReaderProcessing(wParam, lParam);
	}
	else if (uMsg == UM_DB_STATUS) {
		OnDbStatus(wParam);
	}
	else if (uMsg == UM_QUERY_TAG_BINDING_RET) {
		OnQueryBindingRet(wParam, lParam);
	}
	else if (uMsg == UM_QUERY_TEMP_SQLITE_RET) {
		OnTempSqliteRet(wParam, lParam);
	} 
	else if (uMsg == UM_QUERY_HAND_READER_TEMP_SQLITE_RET) {
		OnHandReaderTempSqliteRet(wParam, lParam);
	}
	else if (uMsg == UM_HAND_READER_TEMP) {
		OnHandReaderTemp(wParam, lParam);
	}
	else if (uMsg == UM_TAG_BINDING_GRIDS_RET) {
		OnTagBindingGridsRet(wParam, lParam);
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
	g_data.m_hWnd = 0;
	ClearVector(m_vHandTagUIs);
	//ClearVector(m_vTagBindingGrids);
}

void   CDuiFrameWnd::OnSize(WPARAM wParam, LPARAM lParam) {
	DWORD dwWidth = LOWORD(lParam) - LAYOUT_WINDOW_HMARGIN * 2;           // layWindow的左右margin
																		  // 30是底部status的大致高度, 1是layWindow的上下margin, 32是标题栏高度
	DWORD dwHeight = HIWORD(lParam) - STATUS_PANE_HEIGHT - LAYOUT_WINDOW_VMARGIN * 2 - WINDOW_TITLE_HEIGHT - 85;
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
		m_BtnEmpty[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
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

		m_LblCurTempTime[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		if ( m_eGridStatus == GRID_STATUS_GRIDS ) {
			DWORD dwIndex = m_MyImage_max[i]->m_dwSelectedReaderIndex;
			if (m_UiAlarms[i][dwIndex]->m_alarm == CAlarmImageUI::HIGH_TEMP) {
				m_LblCurTemp_grid1[i]->SetTextColor(g_data.m_skin[CMySkin::HIGH_TEMP_ALARM_TEXT_COLOR]);
			}
			else if (m_UiAlarms[i][dwIndex]->m_alarm == CAlarmImageUI::LOW_TEMP) {
				m_LblCurTemp_grid1[i]->SetTextColor(g_data.m_skin[CMySkin::LOW_TEMP_ALARM_TEXT_COLOR]);
			}
			else if (m_UiAlarms[i][dwIndex]->m_alarm == CAlarmImageUI::DISCONNECTED) {
				m_LblCurTemp_grid1[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
			}
			else {
				m_LblCurTemp_grid1[i]->SetTextColor(g_data.m_skin[CMySkin::NORMAL_TEMP_TEXT_COLOR]);
			}
		}                       
		
		if (g_data.m_skin.GetSkin() == CMySkin::SKIN_WHITE)
			m_BtnEmpty[i]->SetForeImage("file='trash1.png' dest='10,5,30,25'");              
		else if (g_data.m_skin.GetSkin() == CMySkin::SKIN_BLACK)
			m_BtnEmpty[i]->SetForeImage("file='trash2.png' dest='10,5,30,25'");

		for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
			m_UiBtnReaderNames[i][j]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
			m_UiEdtReaderNames[i][j]->SetTextColor(g_data.m_skin[CMySkin::EDIT_TEXT]);
			m_UiEdtReaderNames[i][j]->SetBkColor(g_data.m_skin[CMySkin::EDIT_BK]);
			m_UiEdtReaderNames[i][j]->SetNativeEditBkColor(g_data.m_skin[CMySkin::EDIT_BK]);

			if (j == m_MyImage_max[i]->m_dwSelectedReaderIndex) {
				m_UiLayReader[i][j]->SetBkColor(g_data.m_skin[CMySkin::LAYOUT_READER_BK]);
			}
			else {
				m_UiLayReader[i][j]->SetBkColor(0);
			}
			
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
		DuiLib::CDuiString  clsName = pFindControl->GetClass();
		// 如果双击按钮，返回
		if (0 == strcmp(clsName, DUI_CTR_BUTTON)) {
			return;
		}
		else if (0 == strcmp(clsName, DUI_CTR_OPTION)) {
			return;
		}
		else if (0 == strcmp(clsName, "MyImage")) {
			CMyImageUI * pMyImage = (CMyImageUI *)pFindControl;
			pMyImage->OnDbClick();
			return;
		}
		else if ( 0 == strcmp(clsName, DUI_CTR_SCROLLBAR) ) {
			return;
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
	else {
		pFindControl = m_PaintManager.FindSubControlByPoint(m_layMain_1, point);

		DuiLib::CDuiString  clsName = pFindControl->GetClass();
		if (0 == strcmp(clsName, "MyImage_1")) {
			CMyImageUI_1 * pMyImage = (CMyImageUI_1 *)pFindControl;
			pMyImage->OnDbClick();
			return;
		}
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

		OnEdtRemarkKillFocus();

		m_layMain->SetFixedColumns( g_data.m_CfgData.m_dwLayoutColumns );
		m_eGridStatus = GRID_STATUS_GRIDS;
		m_dwInflateGridIndex = -1;
		OnGridInflateSub(dwIndex);
		ReLayout(m_layMain->GetWidth(), m_layMain->GetHeight());
		
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
	assert(GRID_STATUS_MAXIUM == m_eGridStatus);
	OnEdtRemarkKillFocus_g(m_MyImage_max[m_dwInflateGridIndex]);
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

		dwValue = DEFAULT_TEMP_FONT;
		g_data.m_cfg->SetConfig(CFG_TEMP_FONT, g_data.m_CfgData.m_dwTempFont, &dwValue);

		dwValue = DEFAULT_SKIN;
		g_data.m_cfg->SetConfig(CFG_SKIN, g_data.m_CfgData.m_dwSkinIndex, &dwValue);

		bValue = FALSE;
		g_data.m_cfg->SetBooleanConfig("auto save excel", g_data.m_CfgData.m_bAutoSaveExcel, &bValue);

		bValue = FALSE;
		g_data.m_cfg->SetBooleanConfig("cross anchor", g_data.m_CfgData.m_bCrossAnchor, &bValue);

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

			strText.Format("%s %lu", CFG_MAX_TEMP, i + 1);
			dwValue = DEFAULT_MAX_TEMP_INDEX;
			g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp, &dwValue);

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
				dwValue = i * MAX_READERS_PER_GRID + j + 1;
				g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwBed, &dwValue);
			}
		}

		dwValue = DEFAULT_MIN_TEMP_INDEX;
		g_data.m_cfg->SetConfig("hand min temperature", g_data.m_CfgData.m_dwHandReaderMinTemp, &dwValue);

		dwValue = DEFAULT_MAX_TEMP_INDEX;
		g_data.m_cfg->SetConfig("hand max temperature", g_data.m_CfgData.m_dwHandReaderMaxTemp, &dwValue);

		dwValue = 3500;
		g_data.m_cfg->SetConfig("hand low temperature alarm", g_data.m_CfgData.m_dwHandReaderLowTempAlarm, &dwValue);

		dwValue = 4000;
		g_data.m_cfg->SetConfig("hand high temperature alarm", g_data.m_CfgData.m_dwHandReaderHighTempAlarm, &dwValue);

		g_data.m_cfg->Save();

		// 程序内存变动
		if (oldData.m_dwSkinIndex != g_data.m_CfgData.m_dwSkinIndex) {
			g_data.m_skin.SetSkin((CMySkin::ENUM_SKIN)g_data.m_CfgData.m_dwSkinIndex);
			OnChangeSkin();
		}

		if (oldData.m_dwLayoutColumns != g_data.m_CfgData.m_dwLayoutColumns || oldData.m_dwLayoutRows != g_data.m_CfgData.m_dwLayoutRows) {
			UpdateLayout();
			CBusiness::GetInstance()->ReconnectLaunchAsyn();
		}

		if ( oldData.m_dwTempFont != g_data.m_CfgData.m_dwTempFont ) {
			for (int i = 0; i < MAX_GRID_COUNT; i++) {
				m_LblCurTemp_grid1[i]->SetFont(g_data.m_CfgData.m_dwTempFont);
			}			
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
	CAboutDlg * pAboutDlg = new CAboutDlg;

	pAboutDlg->Create(this->m_hWnd, _T("关于"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAboutDlg->CenterWindow();
	int ret = pAboutDlg->ShowModal();

	// 如果OK
	if (0 == ret) {

	}

	delete pAboutDlg;
}

void   CDuiFrameWnd::OnBtnBed_grid(TNotifyUI& msg) {
	return;

#if !DB_FLAG
	int nIndex = msg.pSender->GetTag();
	msg.pSender->SetVisible(false);
	m_EdtBed_grid[nIndex]->SetText(msg.pSender->GetText());
	m_EdtBed_grid[nIndex]->SetVisible(true);
	m_EdtBed_grid[nIndex]->SetFocus();
#endif
}

void   CDuiFrameWnd::OnEdtBedKillFocus_grid(TNotifyUI& msg) {
	DuiLib::CDuiString  strDefault;	
	int nIndex = msg.pSender->GetTag();
	strDefault.Format("%02d", nIndex + 1);

	STRNCPY( g_data.m_CfgData.m_GridCfg[nIndex].m_szBed, msg.pSender->GetText(), MAX_BED_LENGTH);
	//if (0 == strcmp(g_data.m_CfgData.m_GridCfg[nIndex].m_szBed, "--")) {
	//	g_data.m_CfgData.m_GridCfg[nIndex].m_szBed[0] = '\0';
	//}  

	CDuiString  strText;
	strText.Format(CFG_BED_NAME " %d", nIndex + 1);
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[nIndex].m_szBed, strDefault);
	g_data.m_cfg->Save();                      

	msg.pSender->SetVisible(false);
	m_BtnBed_grid[nIndex]->SetText(msg.pSender->GetText());
	m_BtnBed_grid[nIndex]->SetVisible(true);

	m_BtnBed_max[nIndex]->SetText(msg.pSender->GetText());
}

void  CDuiFrameWnd::OnBtnName_grid(TNotifyUI& msg) {
#if !(DB_FLAG)
	int nIndex = msg.pSender->GetTag();
	msg.pSender->SetVisible(false);
	m_EdtName_grid[nIndex]->SetText(msg.pSender->GetText());
	m_EdtName_grid[nIndex]->SetVisible(true);
	m_EdtName_grid[nIndex]->SetFocus();
#endif
}

void    CDuiFrameWnd::OnEdtNameKillFocus_grid(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	BOOL  bChanged = (0 == strcmp(msg.pSender->GetText(), g_data.m_CfgData.m_GridCfg[nIndex].m_szName) ? FALSE : TRUE );
	STRNCPY(g_data.m_CfgData.m_GridCfg[nIndex].m_szName, msg.pSender->GetText(), MAX_NAME_LENGTH);
	if (0 == strcmp(g_data.m_CfgData.m_GridCfg[nIndex].m_szName, "--")) {
		g_data.m_CfgData.m_GridCfg[nIndex].m_szName[0] = '\0';
	}

	CDuiString  strText;
	strText.Format(CFG_PATIENT_NAME " %d", nIndex + 1);
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[nIndex].m_szName, "");
	g_data.m_cfg->Save();

	msg.pSender->SetVisible(false);
	m_BtnName_grid[nIndex]->SetText(msg.pSender->GetText());
	m_BtnName_grid[nIndex]->SetVisible(true);

	m_BtnName_max[nIndex]->SetText(msg.pSender->GetText());

	if (bChanged) {
		CBusiness::GetInstance()->ExcelPatientNameChangedAsyn(nIndex);
	}
}

void    CDuiFrameWnd::OnGridSwitch(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	COptionUI * p = (COptionUI*)msg.pSender;
	g_data.m_CfgData.m_GridCfg[nIndex].m_bSwitch = !p->IsSelected();
	CDuiString  strText;
	strText.Format("%s %lu", CFG_GRID_SWITCH, nIndex + 1);
	g_data.m_cfg->SetBooleanConfig(strText, g_data.m_CfgData.m_GridCfg[nIndex].m_bSwitch, DEFAULT_READER_SWITCH);
	g_data.m_cfg->Save();

	DWORD  dwIndex         = nIndex;
	DWORD  dwSelectedIndex = m_MyImage_max[dwIndex]->m_dwSelectedReaderIndex;

	if (!g_data.m_CfgData.m_GridCfg[dwIndex].m_bSwitch) {
		//m_LblCurTemp_grid1[dwIndex]->SetVisible(false);
		m_MyAlarm_grid[dwIndex]->StartAlarm(CAlarmImageUI::DISCONNECTED);
		for (int k = 0; k < MAX_READERS_PER_GRID; k++) {
			//m_UiReaderTemp[dwIndex][k]->SetVisible(false);
			m_tLastTemp[dwIndex][k].m_dwTemp = 0;
			m_tLastTemp[dwIndex][k].m_Time = 0;
		}
	}
	else {
		for (int k = 0; k < MAX_READERS_PER_GRID; k++) {
			if (g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[k].m_bSwitch)
			{
				m_UiReaderTemp[dwIndex][k]->SetVisible(true);
				m_UiReaderTemp[dwIndex][k]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
				m_UiReaderTemp[dwIndex][k]->SetText("--");
			}
			else
			{
				//m_UiReaderTemp[dwIndex][k]->SetVisible(false);
				m_tLastTemp[dwIndex][k].m_dwTemp = 0;
				m_tLastTemp[dwIndex][k].m_Time = 0;
			}
		}

		if (dwSelectedIndex >= MAX_READERS_PER_GRID) {
			m_LblCurTemp_grid1[dwIndex]->SetVisible(false);
			m_MyAlarm_grid[dwIndex]->StartAlarm(CAlarmImageUI::DISCONNECTED);
		}
		else {
			if (g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSelectedIndex].m_bSwitch) {
				m_LblCurTemp_grid1[dwIndex]->SetVisible(true);
				m_LblCurTemp_grid1[dwIndex]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
				m_LblCurTemp_grid1[dwIndex]->SetText("--");
			}
			else {
				//m_LblCurTemp_grid1[dwIndex]->SetVisible(false);
				m_MyAlarm_grid[dwIndex]->StartAlarm(CAlarmImageUI::DISCONNECTED);
			}
		}
	}

	CDuiString name = msg.pSender->GetName();
	if (name == OPT_GRID_SWITCH_GRID) {
		m_OptGridSwitch_max[nIndex]->Selected(g_data.m_CfgData.m_GridCfg[nIndex].m_bSwitch ? true : false);
	}
	else if (name == OPT_GRID_SWITCH_MAX) {
		m_OptGridSwitch_grid[nIndex]->Selected(g_data.m_CfgData.m_GridCfg[nIndex].m_bSwitch ? true : false);
	}     
} 

void   CDuiFrameWnd::OnBtnBed_max(TNotifyUI& msg) {
#if !DB_FLAG
	int nIndex = msg.pSender->GetTag();
	msg.pSender->SetVisible(false);
	m_EdtBed_max[nIndex]->SetText(msg.pSender->GetText());
	m_EdtBed_max[nIndex]->SetVisible(true);
	m_EdtBed_max[nIndex]->SetFocus();
#endif
}

void   CDuiFrameWnd::OnBtnName_max(TNotifyUI& msg) {
#if !(DB_FLAG)
	int nIndex = msg.pSender->GetTag();
	msg.pSender->SetVisible(false);
	m_EdtName_max[nIndex]->SetText(msg.pSender->GetText());
	m_EdtName_max[nIndex]->SetVisible(true);
	m_EdtName_max[nIndex]->SetFocus();
#endif
}
      
void   CDuiFrameWnd::OnEdtBedKillFocus_max(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	STRNCPY(g_data.m_CfgData.m_GridCfg[nIndex].m_szBed, msg.pSender->GetText(), MAX_BED_LENGTH);
	if (0 == strcmp(g_data.m_CfgData.m_GridCfg[nIndex].m_szBed, "--")) {
		g_data.m_CfgData.m_GridCfg[nIndex].m_szBed[0] = '\0';
	}

	CDuiString  strText;
	CDuiString  strDefault;
	strText.Format(CFG_BED_NAME " %d", nIndex + 1);
	strDefault.Format("%02lu", nIndex + 1);
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[nIndex].m_szBed, (const char *)strDefault);
	g_data.m_cfg->Save();

	msg.pSender->SetVisible(false);
	m_BtnBed_max[nIndex]->SetText(msg.pSender->GetText());
	m_BtnBed_max[nIndex]->SetVisible(true);

	m_BtnBed_grid[nIndex]->SetText(msg.pSender->GetText());
}

void   CDuiFrameWnd::OnEdtNameKillFocus_max(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	BOOL  bChanged = (0 == strcmp(msg.pSender->GetText(), g_data.m_CfgData.m_GridCfg[nIndex].m_szName) ? FALSE : TRUE);
	STRNCPY(g_data.m_CfgData.m_GridCfg[nIndex].m_szName, msg.pSender->GetText(), MAX_NAME_LENGTH);
	if (0 == strcmp(g_data.m_CfgData.m_GridCfg[nIndex].m_szName, "--")) {
		g_data.m_CfgData.m_GridCfg[nIndex].m_szName[0] = '\0';
	}

	CDuiString  strText;
	strText.Format(CFG_PATIENT_NAME " %d", nIndex + 1);
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[nIndex].m_szName, "");
	g_data.m_cfg->Save();

	msg.pSender->SetVisible(false);
	m_BtnName_max[nIndex]->SetText(msg.pSender->GetText());
	m_BtnName_max[nIndex]->SetVisible(true);

	m_BtnName_grid[nIndex]->SetText(msg.pSender->GetText());
	if (bChanged) {
		CBusiness::GetInstance()->ExcelPatientNameChangedAsyn(nIndex);
	}
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

	if ( !g_data.m_CfgData.m_GridCfg[dwIndex].m_bSwitch ) {
		//m_UiReaderTemp[dwIndex][dwSubIndex]->SetVisible(false);
		m_tLastTemp[dwIndex][dwSubIndex].m_dwTemp = 0;
		m_tLastTemp[dwIndex][dwSubIndex].m_Time = 0;
		//m_LblCurTemp_grid1[dwIndex]->SetVisible(false);
		m_MyAlarm_grid[dwIndex]->StartAlarm(CAlarmImageUI::DISCONNECTED);
	}
	else {
		if (g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_bSwitch)
		{
			m_UiReaderTemp[dwIndex][dwSubIndex]->SetVisible(true);
			m_UiReaderTemp[dwIndex][dwSubIndex]->SetText("--");
			m_UiReaderTemp[dwIndex][dwSubIndex]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		}
		else
		{
			//m_UiReaderTemp[dwIndex][dwSubIndex]->SetVisible(false);
			m_tLastTemp[dwIndex][dwSubIndex].m_dwTemp = 0;
			m_tLastTemp[dwIndex][dwSubIndex].m_Time = 0;
		}

		DWORD  dwSelectedIndex = m_MyImage_max[dwIndex]->m_dwSelectedReaderIndex;
		if (dwSelectedIndex == dwSubIndex) {
			if (g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_bSwitch)
			{
				m_LblCurTemp_grid1[dwIndex]->SetVisible(true);
				m_LblCurTemp_grid1[dwIndex]->SetText("--");
				m_LblCurTemp_grid1[dwIndex]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
			}
			else
			{
				//m_LblCurTemp_grid1[dwIndex]->SetVisible(false);
				m_MyAlarm_grid[dwIndex]->StartAlarm(CAlarmImageUI::DISCONNECTED);
			}
		}
	}

#if DB_FLAG
	OnCheckGridBinding(dwIndex);
#endif
}

void   CDuiFrameWnd::OnTestTimer(DWORD  dwTimer) {
#if TEST_FLAG
	if (dwTimer == TIMER_TEST_ID ) {
		DWORD dwTmp = GetRand(3600, 3800);
		OnTemp(0, 0, dwTmp);
	}
	else if (dwTimer == TIMER_TEST_ID_1) {
		DWORD dwTmp = GetRand(3400, 3600);
		OnTemp(0, 1, dwTmp);
	}	
#endif
}
    
void   CDuiFrameWnd::OnUpdateGridScroll(WPARAM wParam, LPARAM lParam) {
	// 如果正在编辑Remark,停止自动更新滚动条
	if ( !g_data.m_bAutoScroll ) {
		return;
	}

	DWORD  dwIndex = wParam;
	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)m_MyImage_max[dwIndex]->GetParent();
	SIZE tParentScrollPos = pParent->GetScrollPos();
	SIZE tParentScrollRange = pParent->GetScrollRange();
	if (tParentScrollPos.cx != tParentScrollRange.cx) {
		pParent->SetScrollPos(tParentScrollRange);
	}
}
        
void  CDuiFrameWnd::OnReaderName(TNotifyUI& msg) {
#if !DB_FLAG
	DWORD  dwTag      = msg.pSender->GetTag();
	DWORD  dwIndex    = LOWORD(dwTag);
	DWORD  dwSubIndex = HIWORD(dwTag);
	msg.pSender->SetVisible(false);
	m_UiEdtReaderNames[dwIndex][dwSubIndex]->SetText(msg.pSender->GetText());
	m_UiEdtReaderNames[dwIndex][dwSubIndex]->SetVisible(true);
	m_UiEdtReaderNames[dwIndex][dwSubIndex]->SetFocus();
#endif
}

void   CDuiFrameWnd::OnEdtReaderNameKillFocus(TNotifyUI& msg) {
	DWORD  dwTag      = msg.pSender->GetTag();
	DWORD  dwIndex    = LOWORD(dwTag);
	DWORD  dwSubIndex = HIWORD(dwTag);

	STRNCPY( g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_szName, 
		     msg.pSender->GetText(), 
		     sizeof(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_szName));

	CDuiString  strText;
	CDuiString  strDefault;
	strText.Format(CFG_READER_NAME " %lu %lu", dwIndex + 1, dwSubIndex + 1);    
	strDefault.Format("读卡器%02lu%c", dwIndex + 1, dwSubIndex + 'A');
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_szName, strDefault);
	g_data.m_cfg->Save();

	msg.pSender->SetVisible(false);
	m_UiBtnReaderNames[dwIndex][dwSubIndex]->SetText(msg.pSender->GetText());
	m_UiBtnReaderNames[dwIndex][dwSubIndex]->SetVisible(true);
}


void   CDuiFrameWnd::OnMyLButtonDown(WPARAM wParam, LPARAM lParam) {
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CControlUI* pCtl = m_PaintManager.FindSubControlByPoint(0, pt);
	if (0 == pCtl) {
		return;
	}

	CControlUI* pOriginalCtl = pCtl;

	while (pCtl) {
		if (pCtl->GetName() == LAY_READER_NAME) {
			DWORD  dwTag = pCtl->GetTag();
			OnLayReaderSelected( LOWORD(dwTag), HIWORD(dwTag) );
			break;
		}
		else if (pCtl->GetName() == "layHandTag") {
			m_dragDropTagIndex = OnLayHandTagSelected(pCtl);
			// 如果不是点击修改名字按钮
			if ( 0 != strcmp( pOriginalCtl->GetClass(),"Button") ) {
				if (m_dragDropTagIndex >= 0) {
					m_dragDropTagId = *m_vHandTagUIs[m_dragDropTagIndex]->m_pTagId;
				}
				m_dragDropGridIndex = -1;
			}
			// 点击了按钮，不用drag drop操作
			else {
				m_dragDropTagIndex = -1;
			}
			break;
		}
		else if (pCtl->GetName() == LAYOUT_GRID_NAME) {
			m_dragDropGridIndex_1 = pCtl->GetTag();
			// 如果不是点击修改名字按钮
			if (0 != strcmp(pOriginalCtl->GetClass(), "Button")) {
				m_dragDropGridIndex_2 = -1;
			}
			// 点击了按钮，不用drag drop操作
			else {
				m_dragDropGridIndex_1 = -1;
			}
		}
		pCtl = pCtl->GetParent();
	}
}

void   CDuiFrameWnd::OnMyLButtonUp(WPARAM wParam, LPARAM lParam) {
	if ( m_dragDropTagIndex >= 0 ) {
		m_dragDropTag->SetVisible(false);		
		m_layHandTagView->SetVisible(true);
		m_layGridsView->SetVisible(false);

		// 最后拖动到的grid index
		if ( m_dragDropGridIndex >= 0 ) {
			// 如果m_dragDropTagIndex还有效
			if ( (int)m_vHandTagUIs.size() > m_dragDropTagIndex ) {
				// 如果tagid 验证也对
				if ( 0 == strcmp( m_dragDropTagId.c_str(), m_vHandTagUIs[m_dragDropTagIndex]->m_pTagId->c_str() ) ) {
					// 如果绑定的index有改变
					if ( m_vHandTagUIs[m_dragDropTagIndex]->m_nBindingGridIndex != m_dragDropGridIndex * MAX_READERS_PER_GRID ) {
						OnHandTagBindingGrid(m_dragDropTagIndex, m_dragDropGridIndex * MAX_READERS_PER_GRID );
					}
				}
			}
		}

		m_dragDropTagIndex = -1;
	}
	else if (m_dragDropGridIndex_1 >= 0 ){
		m_dragDropTag->SetVisible(false);

		if ( m_dragDropGridIndex_2 >= 0 ) {
			CControlUI* pCtl = m_layMain->GetItemAt(m_dragDropGridIndex_2);
			pCtl->SetBorderColor(g_data.m_skin[CMySkin::GRID_BORDER]);
		}

		m_dragDropGridIndex_2 = -1;
		m_dragDropGridIndex_1 = -1;
	}
}

void   CDuiFrameWnd::OnMyMouseMove(WPARAM wParam, LPARAM lParam) {
	if (m_dragDropTagIndex >= 0 ) {
		if (!m_dragDropTag->IsVisible()) {
			RECT rect = m_layHandTagView->GetPos();

			m_dragDropTag->SetVisible(true);
			m_layHandTagView->SetVisible(false);
			m_layGridsView->SetVisible(true);
			OnGridsViewSize( rect.right - rect.left, rect.bottom - rect.top );
		}

		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);

		int x = 100 / 2;
		int y = 100 / 2;

		RECT r;
		r.left = pt.x - x;
		r.right = r.left + x * 2;
		r.top = pt.y - y;
		r.bottom = r.top + y * 2;
		m_dragDropTag->SetPos(r);

		m_dragDropGridIndex = OnMouseMoveGridsView(pt);
	}
	else if (m_dragDropGridIndex_1 >= 0) {

		if (!m_dragDropTag->IsVisible()) {
			m_dragDropTag->SetVisible(true);
		}

		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);

		int x = 100 / 2;
		int y = 100 / 2;

		RECT r;
		r.left = pt.x - x;
		r.right = r.left + x * 2;
		r.top = pt.y - y;
		r.bottom = r.top + y * 2;
		m_dragDropTag->SetPos(r);

		m_dragDropGridIndex_2 = OnMouseMoveGridsView_1(pt);
	}
}

void   CDuiFrameWnd::OnLayReaderSelected(DWORD dwIndex, DWORD dwSubIndex) {
	assert(dwSubIndex < MAX_READERS_PER_GRID);
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		if (i == dwSubIndex) {
			m_UiLayReader[dwIndex][i]->SetBkColor(g_data.m_skin[CMySkin::LAYOUT_READER_BK]);
		}
		else {
			m_UiLayReader[dwIndex][i]->SetBkColor(0);
		}
	}
	//m_MyImage_max[dwIndex]->m_dwSelectedReaderIndex = dwSubIndex;
	//m_MyImage_grid[dwIndex]->m_dwSelectedReaderIndex = dwSubIndex;
	m_MyImage_max[dwIndex]->OnReaderSelected(dwSubIndex);
	m_MyImage_grid[dwIndex]->OnReaderSelected(dwSubIndex);

	m_LblCurTemp_grid1[dwIndex]->SetTextColor(m_UiReaderTemp[dwIndex][dwSubIndex]->GetTextColor());
	//m_LblCurTemp_grid1[dwIndex]->SetVisible(m_UiReaderTemp[dwIndex][dwSubIndex]->IsVisible());
	m_LblCurTemp_grid1[dwIndex]->SetText(m_UiReaderTemp[dwIndex][dwSubIndex]->GetText());
	
	time_t  now = time(0);
	char buf[8192];
	if (m_tLastTemp[dwIndex][dwSubIndex].m_Time != 0) {
		// 显示时间
		GetElapsedTimeDesc(buf, sizeof(buf), now - m_tLastTemp[dwIndex][dwSubIndex].m_Time);
		m_LblCurTempTime[dwIndex]->SetText(buf);
	}
	else {
		m_LblCurTempTime[dwIndex]->SetText("");  
	}

	CDuiString strText;
	strText.Format("%c", 'A' + dwSubIndex);
	m_LblBodyPart[dwIndex]->SetText(strText);
	m_MyAlarm_grid[dwIndex]->StartAlarm(m_UiAlarms[dwIndex][dwSubIndex]->m_alarm);
	m_MyImage_max[dwIndex]->MyInvalidate();
	m_MyImage_grid[dwIndex]->MyInvalidate();  
}

void   CDuiFrameWnd::OnTemp( DWORD dwIndex, DWORD dwSubIndex, DWORD dwTemp, BOOL bChangeAlarm /*= TRUE*/) {

	m_MyImage_grid[dwIndex]->AddTemp(dwSubIndex, dwTemp);
	m_MyImage_max[dwIndex]->AddTemp(dwSubIndex, dwTemp);

	CAlarmImageUI::ENUM_ALARM e = CAlarmImageUI::ALARM_OK;
	if (dwTemp > g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_dwHighTempAlarm) {
		e = CAlarmImageUI::HIGH_TEMP;
		//m_UiAlarms[dwIndex][dwSubIndex]->StartAlarm(CAlarmImageUI::HIGH_TEMP);
	}
	else if (dwTemp < g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_dwLowTempAlarm) {
		e = CAlarmImageUI::LOW_TEMP;
		//m_UiAlarms[dwIndex][dwSubIndex]->StartAlarm(CAlarmImageUI::LOW_TEMP);
	}
	else {
		e = CAlarmImageUI::ALARM_OK;
		//m_UiAlarms[dwIndex][dwSubIndex]->StopAlarm();
	}

	if (bChangeAlarm) {
		m_UiAlarms[dwIndex][dwSubIndex]->StartAlarm(e);
	}
	//m_LblCurTemp_grid1[dwIndex]->SetTextColor(0xFF4E8B20);

	if ( dwSubIndex == m_MyImage_max[dwIndex]->m_dwSelectedReaderIndex ) {
		if (bChangeAlarm)
			m_MyAlarm_grid[dwIndex]->StartAlarm(e);

		if ( e == CAlarmImageUI::HIGH_TEMP ) {
			m_LblCurTemp_grid1[dwIndex]->SetTextColor(g_data.m_skin[CMySkin::HIGH_TEMP_ALARM_TEXT_COLOR]);
		}
		else if (e == CAlarmImageUI::LOW_TEMP) {
			m_LblCurTemp_grid1[dwIndex]->SetTextColor(g_data.m_skin[CMySkin::LOW_TEMP_ALARM_TEXT_COLOR]);
		}
		else {
			m_LblCurTemp_grid1[dwIndex]->SetTextColor(g_data.m_skin[CMySkin::NORMAL_TEMP_TEXT_COLOR]);
		}

		DuiLib::CDuiString  strText;
		strText.Format("%.2f℃", dwTemp / 100.0);
		m_LblCurTemp_grid[dwIndex]->SetText(strText);
		m_LblCurTemp_grid1[dwIndex]->SetText(strText);
		m_LblCurTempTime[dwIndex]->SetText("刚刚");
	}
}

void   CDuiFrameWnd::OnLaunchStatus(WPARAM wParam, LPARAM  lParam) {
	CLmnSerialPort::PortStatus eStatus = (CLmnSerialPort::PortStatus)wParam;

	if (eStatus == CLmnSerialPort::OPEN) {
		m_lblLaunchStatus->SetText("发射器连接OK");
	}
	else {
		m_lblLaunchStatus->SetText("发射器连接断开");
		m_lblProcTips->SetText("");

		for (int i = 0; i < MAX_GRID_COUNT; i++) {
			for (int j = 0; j < MAX_READERS_PER_GRID; j++) {
				OnReaderDisconnected( MAKELONG(i,j), 0 );
			}			
		}
	}
}

void   CDuiFrameWnd::OnReaderTemp(WPARAM wParam, LPARAM  lParam) {
	DWORD  dwIndex = LOWORD(wParam);
	DWORD  dwSubIndex = HIWORD(wParam);
	LastTemp * pTemp = (LastTemp*)lParam;

	// 如果开关没有打开
	if (!g_data.m_CfgData.m_GridCfg[dwIndex].m_bSwitch) {
		delete pTemp;
		return;
	}

	if (!g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_bSwitch) {
		delete pTemp;
		return;
	}

	DWORD  dwTemp = pTemp->m_dwTemp;

	// 保存sqlite
	CBusiness::GetInstance()->SaveTemp2SqliteAsyn(pTemp->m_szTagId, dwTemp);

	// 如果tag改变
	if (0 != strcmp(pTemp->m_szTagId, m_tLastTemp[dwIndex][dwSubIndex].m_szTagId)) {
		m_MyImage_max[dwIndex]->SetTagId(dwSubIndex, pTemp->m_szTagId);
		m_MyImage_max[dwIndex]->EmptyData(dwSubIndex);
		m_MyImage_max[dwIndex]->Invalidate();

		// 查询tag绑定关系
#if DB_FLAG
		memset(&m_tTagBinding[dwIndex][dwSubIndex], 0, sizeof(m_tTagBinding[dwIndex][dwSubIndex]));
		m_btnBinding[dwIndex]->SetVisible(false);
		m_LblTagBinding[dwIndex][dwSubIndex]->SetText("");
		m_UiBtnReaderNames[dwIndex][dwSubIndex]->SetText(g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_szName);
		CBusiness::GetInstance()->QueryBindingAsyn(dwIndex, dwSubIndex, pTemp->m_szTagId);
#endif

		// 查询sqlite历史温度数据
		CBusiness::GetInstance()->QueryTempFromSqliteByTagAsyn(pTemp->m_szTagId, dwIndex, dwSubIndex);
	}

	memcpy(&m_tLastTemp[dwIndex][dwSubIndex], pTemp, sizeof(LastTemp));
	m_tLastTemp[dwIndex][dwSubIndex].m_Time = time(0);
	CBusiness::GetInstance()->ExcelTempAsyn(dwIndex, dwSubIndex, &m_tLastTemp[dwIndex][dwSubIndex]);

#if DB_FLAG
	CBusiness::GetInstance()->SaveTempAsyn(dwIndex, dwSubIndex, &m_tLastTemp[dwIndex][dwSubIndex]);
#endif
	
	OnTemp(dwIndex, dwSubIndex, dwTemp);

	if (dwTemp > g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_dwHighTempAlarm) {
		m_UiReaderTemp[dwIndex][dwSubIndex]->SetTextColor(g_data.m_skin[CMySkin::HIGH_TEMP_ALARM_TEXT_COLOR]);
	}
	else if (dwTemp < g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_dwLowTempAlarm) {
		m_UiReaderTemp[dwIndex][dwSubIndex]->SetTextColor(g_data.m_skin[CMySkin::LOW_TEMP_ALARM_TEXT_COLOR]);
	}
	else {
		m_UiReaderTemp[dwIndex][dwSubIndex]->SetTextColor(g_data.m_skin[CMySkin::NORMAL_TEMP_TEXT_COLOR]);
	}

	DuiLib::CDuiString  strText;
	strText.Format("%.2f℃", dwTemp / 100.0);
	m_UiReaderTemp[dwIndex][dwSubIndex]->SetText(strText);
	m_lblProcTips->SetText("");

	m_LblReaderId[dwIndex][dwSubIndex]->SetText(pTemp->m_szReaderId);
	m_LblTagId[dwIndex][dwSubIndex]->SetText(pTemp->m_szTagId);
	
	delete pTemp;
}           
 
void   CDuiFrameWnd::OnReaderDisconnected(WPARAM wParam, LPARAM  lParam) {
	DuiLib::CDuiString  strText;

	DWORD  dwIndex = LOWORD(wParam);
	DWORD  dwSubIndex = HIWORD(wParam);
	m_UiAlarms[dwIndex][dwSubIndex]->StartAlarm(CAlarmImageUI::DISCONNECTED);

	if (!g_data.m_CfgData.m_GridCfg[dwIndex].m_bSwitch) {
		//m_UiReaderTemp[dwIndex][dwSubIndex]->SetVisible(false);
	}
	else {
		if (g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_bSwitch) {
			m_UiReaderTemp[dwIndex][dwSubIndex]->SetVisible(true);
			if (m_tLastTemp[dwIndex][dwSubIndex].m_dwTemp == 0) {
				m_UiReaderTemp[dwIndex][dwSubIndex]->SetText("--");
				m_UiReaderTemp[dwIndex][dwSubIndex]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);				
			}
			else {
				strText.Format("--/%.2f℃", m_tLastTemp[dwIndex][dwSubIndex].m_dwTemp / 100.0);
				m_UiReaderTemp[dwIndex][dwSubIndex]->SetText(strText);
			}
		}
		else {
			//m_UiReaderTemp[dwIndex][dwSubIndex]->SetVisible(false);
		}
	}

	if (dwSubIndex == m_MyImage_max[dwIndex]->m_dwSelectedReaderIndex) {
		m_MyAlarm_grid[dwIndex]->StartAlarm(CAlarmImageUI::DISCONNECTED);
		m_LblCurTemp_grid[dwIndex]->SetText("--");		

		if (!g_data.m_CfgData.m_GridCfg[dwIndex].m_bSwitch) {
			//m_LblCurTemp_grid1[dwIndex]->SetVisible(false);
		}
		else {
			if (g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_bSwitch) {
				m_LblCurTemp_grid1[dwIndex]->SetVisible(true);
				if (m_tLastTemp[dwIndex][dwSubIndex].m_dwTemp == 0) {
					m_LblCurTemp_grid1[dwIndex]->SetText("--");
					m_LblCurTemp_grid1[dwIndex]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
				}
				else {
					strText.Format("--/%.2f℃", m_tLastTemp[dwIndex][dwSubIndex].m_dwTemp / 100.0);
					m_LblCurTemp_grid1[dwIndex]->SetText(strText);
				}
			}
			else {
				//m_LblCurTemp_grid1[dwIndex]->SetVisible(false);
			}
		}
	}

	m_lblProcTips->SetText("");
}
    
void   CDuiFrameWnd::OnMyMouseWheel(WPARAM wParam, LPARAM lParam) {
	int nDirectory = (int)wParam;
	BOOL bChanged = FALSE;
	if (nDirectory > 0)
	{
		if (0 == m_nSelTabIndex) {
			if (m_eGridStatus == GRID_STATUS_MAXIUM) {
				m_MyImage_max[m_dwInflateGridIndex]->OnMouseWheel(TRUE);
			}
		}
		else if (1 == m_nSelTabIndex) {
			m_MyImage_hand_reader->OnMouseWheel(TRUE);
		}
	}
	else
	{
		if (0 == m_nSelTabIndex) {
			if (m_eGridStatus == GRID_STATUS_MAXIUM) {
				m_MyImage_max[m_dwInflateGridIndex]->OnMouseWheel(FALSE);
			}
		}
		else if (1 == m_nSelTabIndex) {
			m_MyImage_hand_reader->OnMouseWheel(FALSE);
		}
	}
}

// 
void   CDuiFrameWnd::OnMyDeviceChanged() {
	char szComPort[16];
	int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));
	if (nFindCount > 1) {
		m_lblBarTips->SetText("存在多个USB-SERIAL CH340串口，请只连接一个发射器");
	}
	else if (0 == nFindCount) {
		m_lblBarTips->SetText("没有找到USB-SERIAL CH340串口，请连接发射器的USB线");
	}
	else {
		m_lblBarTips->SetText("");
	}

	CBusiness::GetInstance()->CheckLaunchStatusAsyn();
}    

//
void  CDuiFrameWnd::OnMyImageClick(DWORD dwIndex,const POINT * pPoint) {
	m_MyImage_max[dwIndex]->OnMyClick(pPoint);
}

//
void   CDuiFrameWnd::OnMyImageMenu(TNotifyUI& msg) {
	POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
	CDuiMenu *pMenu = new CDuiMenu(_T("menu_image.xml"), msg.pSender);
	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

//
void   CDuiFrameWnd::OnExportExcel() {
	assert(m_eGridStatus == GRID_STATUS_MAXIUM);
	if (m_eGridStatus != GRID_STATUS_MAXIUM) {
		return;
	}

	m_MyImage_max[m_dwInflateGridIndex]->ExportExcel( g_data.m_CfgData.m_GridCfg[m_dwInflateGridIndex].m_szName );
}

//
void   CDuiFrameWnd::OnPrintExcel() {
	assert(m_eGridStatus == GRID_STATUS_MAXIUM);
	if (m_eGridStatus != GRID_STATUS_MAXIUM) {
		return;
	}              

	char szReaderName[MAX_READERS_PER_GRID][64] = { 0 };
	for (DWORD i = 0; i < MAX_READERS_PER_GRID; i++) {
		STRNCPY( szReaderName[i], g_data.m_CfgData.m_GridCfg[m_dwInflateGridIndex].m_ReaderCfg[i].m_szName, 64 );
		if (szReaderName[i][0] == '\0') {
			SNPRINTF(szReaderName[i], 64, "No.%lu", i + 1);
		}
		else if (0 == strcmp(szReaderName[i], "--")) {
			SNPRINTF(szReaderName[i], 64, "No.%lu", i + 1);
		}
	}
	m_MyImage_max[m_dwInflateGridIndex]->PrintExcel(szReaderName, g_data.m_CfgData.m_GridCfg[m_dwInflateGridIndex].m_szName );
}

// 清空数据
void   CDuiFrameWnd::OnEmpty(TNotifyUI& msg) {
	if ( IDNO == ::MessageBox(this->GetHWND(), "确定要重置数据吗？", "重置数据", MB_YESNO | MB_DEFBUTTON2 )) {
		return;
	}

	DWORD dwIndex = msg.pSender->GetTag();
	m_MyImage_max[dwIndex]->EmptyData();

	for (int i = 0; i < MAX_READERS_PER_GRID; i++) {
		m_tLastTemp[dwIndex][i].m_dwTemp = 0;
		m_tLastTemp[dwIndex][i].m_Time = 0;
		OnReaderDisconnected(MAKELONG(dwIndex, i), 0);
	}	
}
         
void   CDuiFrameWnd::OnReaderProcessing(WPARAM wParam, LPARAM  lParam) {
	WORD  dwIndex = LOWORD(wParam);
	DWORD  dwSubIndex = HIWORD(wParam);
	DuiLib::CDuiString  strText;

	strText.Format("\"%s%c\"读卡器正在测温......", g_data.m_CfgData.m_GridCfg[dwIndex].m_szBed, dwSubIndex + 'A');
	m_lblProcTips->SetText(strText);
}

void   CDuiFrameWnd::OnUpdateTimeDescTimer( ) {
	time_t  now = time(0);
	char buf[8192];
	for ( int i = 0; i < MAX_GRID_COUNT; i++ ) {
		DWORD  dwSelectIndex = m_MyImage_max[i]->m_dwSelectedReaderIndex;
		if ( dwSelectIndex < MAX_READERS_PER_GRID ) {
			if ( m_tLastTemp[i][dwSelectIndex].m_Time != 0 ) {
				// 显示时间
				GetElapsedTimeDesc(buf, sizeof(buf), now - m_tLastTemp[i][dwSelectIndex].m_Time);
				m_LblCurTempTime[i]->SetText( buf );
			}
		}
	}
}

//
void   CDuiFrameWnd::OnDbStatus(int nStatus) {
	if (0 == nStatus) {
		m_LblDbStatus->SetText("数据库连接断开");
	}
	else {
		m_LblDbStatus->SetText("数据库连接OK");

		for (int i = 0; i < MAX_GRID_COUNT; i++) {
			for (int j = 0; j < MAX_READERS_PER_GRID; j++) {
				if ( !g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch ) {
					continue;
				}

				if (m_tLastTemp[i][j].m_szTagId[0] == '\0') {
					continue;
				}

				if (m_tTagBinding[i][j].m_bGetBindingRet) {
					continue;
				}

				CBusiness::GetInstance()->QueryBindingAsyn(i, j, m_tLastTemp[i][j].m_szTagId);
			}
		}
	}
}

//
void    CDuiFrameWnd::OnQueryBindingRet(WPARAM wParam, LPARAM  lParam) {
	WORD  dwIndex = LOWORD(wParam);
	DWORD  dwSubIndex = HIWORD(wParam);
	TagBinding * pRet = (TagBinding *)lParam;

	// 如果tag id不等
	if ( 0 != strcmp(pRet->m_szTagId, m_tLastTemp[dwIndex][dwSubIndex].m_szTagId)) {
		delete pRet;
		return;
	}    

	memcpy(&m_tTagBinding[dwIndex][dwSubIndex], pRet, sizeof(TagBinding));
	m_tTagBinding[dwIndex][dwSubIndex].m_bGetBindingRet = TRUE;
	if (pRet->m_dwPatientId > 0) {
		m_LblTagBinding[dwIndex][dwSubIndex]->SetText("已绑定");
		m_UiBtnReaderNames[dwIndex][dwSubIndex]->SetText(m_tTagBinding[dwIndex][dwSubIndex].m_szTagName);
	}
	else {
		m_LblTagBinding[dwIndex][dwSubIndex]->SetText("未绑定");
	}  

	OnCheckGridBinding(dwIndex);
	
	delete pRet;
}

// 
void   CDuiFrameWnd::OnCheckGridBinding(DWORD dwIndex ) {

	BOOL    bAllGetRet;
	DWORD   dwPatientId;
	BOOL    bValidate;
	BOOL    bTotalBinding;

	int nSwitchOnCnt = 0;
	bAllGetRet = TRUE;
	// 检查一个窗格的所有tag都是否取到结果
	for (int i = 0; i < MAX_READERS_PER_GRID; i++) {
		if (!g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_bSwitch) {
			continue;
		}

		nSwitchOnCnt++;
		if (!m_tTagBinding[dwIndex][i].m_bGetBindingRet) {
			bAllGetRet = FALSE;
			break;
		}
	}

	// 没有一个Reader打开开关
	if (nSwitchOnCnt <= 0) {
		m_btnBinding[dwIndex]->SetVisible(false);
		return;
	}

	// 不是所有的tag都查询到结果
	if (!bAllGetRet) {
		m_btnBinding[dwIndex]->SetVisible(false);
		return;
	}

	// 检查是否所有tag绑定到同一个人
	// 检查是否有tag没有绑定
	dwPatientId = 0;
	bValidate = TRUE;
	bTotalBinding = TRUE;
	CDuiString  strPatientName="--";
	for (int i = 0; i < MAX_READERS_PER_GRID; i++) {
		if (!g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_bSwitch) {
			continue;
		}

		if (0 == dwPatientId) {
			if (m_tTagBinding[dwIndex][i].m_dwPatientId > 0) {
				dwPatientId = m_tTagBinding[dwIndex][i].m_dwPatientId;
				strPatientName = m_tTagBinding[dwIndex][i].m_szPatientName;
			}
			else {
				bTotalBinding = FALSE;
			}
		}
		else {
			if (m_tTagBinding[dwIndex][i].m_dwPatientId > 0) {
				// 两个Tag绑定到不同的病人，有问题
				if (m_tTagBinding[dwIndex][i].m_dwPatientId != dwPatientId) {
					bValidate = FALSE;
					break;
				}
			}
			else {
				bTotalBinding = FALSE;
			}
		}
	}

	// 如果所有绑定的tag的绑定者是同一人
	if (bValidate) {
		// 所有的tag均没有绑定
		if (dwPatientId == 0) {
			m_btnBinding[dwIndex]->SetVisible(true);
		}
		else {
			// 所有的tag中，只有部分绑定
			if (!bTotalBinding) {
				m_btnBinding[dwIndex]->SetVisible(true);
			}
			// 所有的tag中，都已经绑定
			else {
				m_btnBinding[dwIndex]->SetVisible(false);
			}			
		}
		m_BtnName_grid[dwIndex]->SetText(strPatientName);
		m_BtnName_max[dwIndex]->SetText(strPatientName);
	}
	// 如果所有绑定的tag的绑定者不是同一人
	else {
		m_btnBinding[dwIndex]->SetVisible(false);
	}
}
 
void   CDuiFrameWnd::OnCheckConflictTagTimer() {
	// DWORD  a = LmnGetTickCount();

	DWORD   dwGridsCnt = MAX_GRID_COUNT;
	DWORD   dwReaderCnt = MAX_READERS_PER_GRID * dwGridsCnt;

	std::vector<DWORD>  vConfilcts;
	for (DWORD i = 0; i < dwReaderCnt; i++) {
		DWORD  m = i / 6;
		DWORD  n = i % 6;

		if ( m_tLastTemp[m][n].m_szTagId[0] == '\0' ) {
			continue;
		}

		for (DWORD j = i + 1; j < dwReaderCnt; j++) {
			DWORD  x = j / 6;
			DWORD  y = j % 6;

			if (m_tLastTemp[x][y].m_szTagId[0] == '\0') {
				continue;
			}

			// 如果有冲突
			if (0 == strcmp(m_tLastTemp[m][n].m_szTagId, m_tLastTemp[x][y].m_szTagId)) {
				DWORD  time_diff = 0;
				if ( m_tLastTemp[m][n].m_Time >= m_tLastTemp[x][y].m_Time ) {
					time_diff = (DWORD)(m_tLastTemp[m][n].m_Time - m_tLastTemp[x][y].m_Time);
				}
				else {
					time_diff = (DWORD)(m_tLastTemp[x][y].m_Time - m_tLastTemp[m][n].m_Time);
				}
				// 如果间隔小于10分钟
				if ( time_diff <= 600 ) {
					vConfilcts.push_back(MAKELONG(i, j));
				}				
			}
		}
	}

	std::vector<DWORD>::iterator  it;
	DuiLib::CDuiString  strText;
	DuiLib::CDuiString  strTips;
	DWORD  k = 0;
	for (it = vConfilcts.begin(), k = 0; it != vConfilcts.end(); it++, k++) {
		DWORD  i = LOWORD( *it );
		DWORD  j = HIWORD( *it );

		DWORD  m = i / 6;
		DWORD  n = i % 6;

		DWORD  x = j / 6;
		DWORD  y = j % 6;

		strText.Format("\"%s%c\"和\"%s%c\"", 
			g_data.m_CfgData.m_GridCfg[m].m_szBed, n + 'A', 
			g_data.m_CfgData.m_GridCfg[x].m_szBed, y + 'A');
		if ( 0 == k ) {
			strTips += strText;
		}
		else {
			strTips += ",";
			strTips += strText;
		}
	}

	if (vConfilcts.size() > 0) {
		strTips += "读取的tag冲突";
	}

	//DWORD  b = LmnGetTickCount();
	//strText.Format("elapsed: %.3f秒", (b - a) / 1000.0);
	//strTips += strText;

	m_LblConflictTips->SetText(strTips);
}

//
void   CDuiFrameWnd::OnBinding(DWORD dwIndex) {
	BOOL    bAllGetRet;
	DWORD   dwPatientId;
	BOOL    bValidate;
	BOOL    bTotalBinding;

	int nSwitchOnCnt = 0;
	bAllGetRet = TRUE;
	// 检查一个窗格的所有tag都是否取到结果
	for (int i = 0; i < MAX_READERS_PER_GRID; i++) {
		if (!g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_bSwitch) {
			continue;
		}

		nSwitchOnCnt++;
		if (!m_tTagBinding[dwIndex][i].m_bGetBindingRet) {
			bAllGetRet = FALSE;
			break;
		}
	}

	// 没有一个Reader打开开关
	if (nSwitchOnCnt <= 0) {
		return;
	}

	// 不是所有的tag都查询到结果
	if (!bAllGetRet) {
		return;
	}

	// 检查是否所有tag绑定到同一个人
	// 检查是否有tag没有绑定
	dwPatientId = 0;
	bValidate = TRUE;
	bTotalBinding = TRUE;
	CBindingDlg * pDlg = new CBindingDlg;

	pDlg->m_dwIndex = dwIndex;	
	int nItemCnt = 0;
	for (int i = 0,k = 0; i < MAX_READERS_PER_GRID; i++) {
		if (!g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[i].m_bSwitch) {
			continue;
		}

		if (0 == dwPatientId) {
			if (m_tTagBinding[dwIndex][i].m_dwPatientId > 0) {
				dwPatientId = m_tTagBinding[dwIndex][i].m_dwPatientId;
			}
			else {
				bTotalBinding = FALSE;

				nItemCnt++;
				pDlg->m_tTagItem[k].m_dwSubIndex = i;
				STRNCPY(pDlg->m_tTagItem[k].m_szTagId, m_tTagBinding[dwIndex][i].m_szTagId,
					sizeof(pDlg->m_tTagItem[k].m_szTagId));
				k++;
			}
		}
		else {
			if (m_tTagBinding[dwIndex][i].m_dwPatientId > 0) {
				// 两个Tag绑定到不同的病人，有问题
				if (m_tTagBinding[dwIndex][i].m_dwPatientId != dwPatientId) {
					bValidate = FALSE;
					break;
				}
			}
			else {
				bTotalBinding = FALSE;

				nItemCnt++;
				pDlg->m_tTagItem[k].m_dwSubIndex = i;
				STRNCPY(pDlg->m_tTagItem[k].m_szTagId, m_tTagBinding[dwIndex][i].m_szTagId,
					sizeof(pDlg->m_tTagItem[k].m_szTagId));
				k++;
			}
		}
	}

	pDlg->m_dwTagItemCnt = nItemCnt;
	pDlg->m_dwPatientId = dwPatientId;
	// 如果所有绑定的tag的绑定者是同一人
	if (bValidate) {
		// 所有的tag均没有绑定
		if (dwPatientId == 0) {
			
		}
		else {
			// 所有的tag中，只有部分绑定
			if (!bTotalBinding) {
				
			}
			// 所有的tag中，都已经绑定
			else {
				delete pDlg;
				return;
			}
		}
	}
	// 如果所有绑定的tag的绑定者不是同一人
	else {
		delete pDlg;
		return;
	}

	pDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();
	// 如果是click ok(绑定成功)
	if (0 == ret) {
		for (DWORD i = 0; i < pDlg->m_dwTagItemCnt; i++) {
			DWORD  dwSubIndex = pDlg->m_tTagItem[i].m_dwSubIndex;
			if (m_tTagBinding[dwIndex][dwSubIndex].m_bGetBindingRet && 0 == strcmp(m_tTagBinding[dwIndex][dwSubIndex].m_szTagId, pDlg->m_tTagItem[i].m_szTagId)) {
				m_tTagBinding[dwIndex][dwSubIndex].m_dwPatientId = pDlg->m_dwBindingPatientId;
				STRNCPY(m_tTagBinding[dwIndex][dwSubIndex].m_szPatientName,
					pDlg->m_szBindingPatientName, sizeof(m_tTagBinding[dwIndex][dwSubIndex].m_szPatientName));
				STRNCPY(m_tTagBinding[dwIndex][dwSubIndex].m_szTagName,
					pDlg->m_szTagName[i], sizeof(m_tTagBinding[dwIndex][dwSubIndex].m_szTagName));

				m_LblTagBinding[dwIndex][dwSubIndex]->SetText("已绑定");
				m_UiBtnReaderNames[dwIndex][dwSubIndex]->SetText(m_tTagBinding[dwIndex][dwSubIndex].m_szTagName);
			}	
		}
		OnCheckGridBinding(dwIndex);
	}
	delete pDlg;
}

//
void   CDuiFrameWnd::OnCheckAutoSaveTimer() {
	time_t t = time(0);
	struct tm  tmp;
	localtime_s(&tmp, &t);

	if ( (tmp.tm_hour == 8 || tmp.tm_hour == 12 || tmp.tm_hour == 18 || tmp.tm_hour == 0) 
		&& (tmp.tm_min >= 0 && tmp.tm_min <= 1 ) ) {
		time_t now = time(0);
		// 两次间隔时间最少30分钟
		if ( now - m_LastSaveExcelTime >= 1800 ) {
			// 保存
			CBusiness::GetInstance()->SaveExcelAsyn();
			m_LastSaveExcelTime = time(0);
		}		
	}
}

void   CDuiFrameWnd::OnTempSqliteRet(WPARAM wParam, LPARAM  lParam) {
	std::vector<TempData*> * pvRet = (std::vector<TempData*> *)wParam;
	DWORD  dwIndex = LOWORD(lParam);
	DWORD  dwSubIndex = HIWORD(lParam);

	m_MyImage_max[dwIndex]->OnTempSqliteRet(*pvRet, dwSubIndex);
	m_MyImage_max[dwIndex]->MyInvalidate();

	::InvalidateRect(GetHWND(), 0, TRUE);
	// ClearVector(*pvRet);
	delete pvRet;
}

void   CDuiFrameWnd::OnTabChanged(DWORD  dwIndex) {
	if ( 0 == dwIndex ) {
		m_layMain->SetVisible(true);
		m_layMain_1->SetVisible(false);
		m_nSelTabIndex = 0;
	}
	else if (1 == dwIndex) {
		m_layMain->SetVisible(false);
		m_layMain_1->SetVisible(true);
		m_nSelTabIndex = 1;
	}
}

//
void   CDuiFrameWnd::OnHandReaderTempSqliteRet(WPARAM wParam, LPARAM  lParam) {
	void ** pParam = (void**)wParam;
	if (0 == pParam) {
		return;
	}

	vector< vector<TempData *> * > * pvData = ( vector< vector<TempData *> * > * )pParam[0];
	vector< string * >  * pvTagId = ( vector< string * > * )pParam[1];
	vector< string * > * pvTagName = ( vector< string * > * ) pParam[2];

	m_MyImage_hand_reader->OnTempSqliteRet( *pvData, *pvTagId, *pvTagName );

	vector< vector<TempData *> * >::iterator it;
	int nIndex = 0;
	for (it = pvData->begin(), nIndex =0; it != pvData->end(); ++it,++nIndex) {

		CDialogBuilder builder_child;
		CControlUI * pTagUI = builder_child.Create("HandTag.xml", (UINT)0, &m_callback, &m_PaintManager);
		pTagUI->FindControl(CS_FINDCONTROLPROC, 0, 0);
		m_layTags->Add(pTagUI);

		string * pTagId  = pvTagId->at(nIndex);
		HandReaderTemp tTemp;
		memset( &tTemp, 0, sizeof(tTemp) );
		STRNCPY( tTemp.m_szTagId, pTagId->c_str(), sizeof(tTemp.m_szTagId) );
		SetHandTagData(&tTemp, pTagUI, pvTagName->at(nIndex)->c_str() );

		TagControlItem * pItem = new TagControlItem;
		pItem->m_Control = pTagUI;
		pItem->m_pTagId = pTagId;
		pItem->m_nBindingGridIndex = -1;
		m_vHandTagUIs.push_back(pItem);
	}

	if (m_vHandTagUIs.size() > 0) {
		OnLayHandTagSelected(m_vHandTagUIs[0]->m_Control);
	}

	if (0 != pvData) {
		delete pvData;
	}

	if (0 != pvTagId) {
		delete pvTagId;
	}

	if (0 != pvTagName) {
		delete pvTagName;
	}

	delete[] pParam;
}

// 需要设置一个定时器，定时清除7天前的数据和tag栏
void  CDuiFrameWnd::OnHandReaderTemp(WPARAM wParam, LPARAM  lParam) {
	HandReaderTemp * pTemp = (HandReaderTemp*)wParam;
	BOOL  bNewTag = FALSE;
	string * pTagId = 0;
	time_t now = time(0);
	m_MyImage_hand_reader->OnTempData(pTemp, bNewTag, pTagId,now);

	if ( bNewTag ) {
		CDialogBuilder builder_child;
		CControlUI * pTagUI = builder_child.Create("HandTag.xml", (UINT)0, &m_callback, &m_PaintManager);
		pTagUI->FindControl(CS_FINDCONTROLPROC, 0, 0);
		m_layTags->AddAt(pTagUI,0);
		SetHandTagData(pTemp, pTagUI);

		TagControlItem * pItem = new TagControlItem;
		pItem->m_Control = pTagUI;
		pItem->m_pTagId = pTagId;
		pItem->m_nBindingGridIndex = -1;
		m_vHandTagUIs.insert( m_vHandTagUIs.begin(), pItem );

		if (m_vHandTagUIs.size() == 1) {
			OnLayHandTagSelected(pTagUI);
		}

		int nGridIndex = GetHandTagGridIndex(pTemp->m_szTagId);
		if (nGridIndex >= 0) {
			DWORD  dwIndex = nGridIndex / MAX_READERS_PER_GRID;
			DWORD  dwSubIndex = nGridIndex % MAX_READERS_PER_GRID;
			LastTemp temp;
			memset(&temp, 0, sizeof(LastTemp));
			temp.m_dwTemp = pTemp->m_dwTemp;
			STRNCPY(temp.m_szReaderId, pTemp->m_szReaderId, sizeof(temp.m_szReaderId));
			STRNCPY(temp.m_szTagId, pTemp->m_szTagId, sizeof(temp.m_szTagId));
			temp.m_Time = time(0);
			PassTemp2Grid(dwIndex, dwSubIndex, &temp);
		}
	}
	else {
		vector<TagControlItem *>::iterator it;
		for (it = m_vHandTagUIs.begin(); it != m_vHandTagUIs.end(); ++it) {
			TagControlItem * pItem = *it;
			// 找到
			if ( 0 == strcmp(pItem->m_pTagId->c_str(), pTemp->m_szTagId) ) {
				SetHandTagData(pTemp, pItem->m_Control);

				int nGridIndex = GetHandTagGridIndex(pTemp->m_szTagId);
				if (nGridIndex >= 0) {
					DWORD  dwIndex = nGridIndex / MAX_READERS_PER_GRID;
					DWORD  dwSubIndex = nGridIndex % MAX_READERS_PER_GRID;
					LastTemp temp;
					memset(&temp, 0, sizeof(LastTemp));
					temp.m_dwTemp = pTemp->m_dwTemp;
					STRNCPY(temp.m_szReaderId, pTemp->m_szReaderId, sizeof(temp.m_szReaderId));
					STRNCPY(temp.m_szTagId, pTemp->m_szTagId, sizeof(temp.m_szTagId));
					temp.m_Time = time(0);
					PassTemp2Grid(dwIndex, dwSubIndex, &temp);
				}
				break;
			}
		}
	}

	// 保存数据库
	CBusiness::GetInstance()->SaveHandTempAsyn(pTemp->m_szTagId, pTemp->m_dwTemp, pTemp->m_szCardId, now);

	delete pTemp;
}

//
void   CDuiFrameWnd::SetHandTagData(const HandReaderTemp * pTemp, CControlUI * pTagUI, const char * szTagName /*= 0*/) {
	CLabelUI * pLblReaderId = static_cast<CLabelUI*>(pTagUI->FindControl(MY_FINDCONTROLPROC, "lblReaderSn", 0));
	pLblReaderId->SetText(pTemp->m_szReaderId);

	CLabelUI * pLblTagId = static_cast<CLabelUI*>(pTagUI->FindControl(MY_FINDCONTROLPROC, "lblHandTagId", 0));
	pLblTagId->SetText(pTemp->m_szTagId);

	if (szTagName) {
		CButtonUI * pbtnName = static_cast<CButtonUI*>(pTagUI->FindControl(MY_FINDCONTROLPROC, "btnTagNickName", 0));
		pbtnName->SetText(szTagName);

		CEditUI * pedName = static_cast<CEditUI*>(pTagUI->FindControl(MY_FINDCONTROLPROC, "edtTagNickName", 0));
		pedName->SetText(szTagName);
	}

	CDuiString strText;
	CLabelUI * pLblTemp = static_cast<CLabelUI*>(pTagUI->FindControl(MY_FINDCONTROLPROC, "lblHandTagTemp", 0));

	if (pTemp->m_dwTemp > 0) {
		strText.Format("%.2f", pTemp->m_dwTemp / 100.0f);
		pLblTemp->SetText(strText);

		if (pTemp->m_dwTemp >= g_data.m_CfgData.m_dwHandReaderHighTempAlarm) {
			pLblTemp->SetTextColor(g_data.m_skin[CMySkin::HIGH_TEMP_ALARM_TEXT_COLOR]);
		}
		else if (pTemp->m_dwTemp <= g_data.m_CfgData.m_dwHandReaderLowTempAlarm) {
			pLblTemp->SetTextColor(g_data.m_skin[CMySkin::LOW_TEMP_ALARM_TEXT_COLOR]);
		}
		else {
			pLblTemp->SetTextColor(g_data.m_skin[CMySkin::NORMAL_TEMP_TEXT_COLOR]);
		}

		CLabelUI * pLblTime = static_cast<CLabelUI*>(pTagUI->FindControl(MY_FINDCONTROLPROC, "lblTempTime", 0));
		time_t now = time(0);
		char buf[256];
		Date2String_2(buf, sizeof(buf), &now); 
		pLblTime->SetText(buf);
	}
	else {
		pLblTemp->SetText("--");
		pLblTemp->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
	}
	
}

void   CDuiFrameWnd::OnAutoPruneTimer() {
	BOOL bChanged = FALSE;;
	int  nNewIndex = 0;
	m_MyImage_hand_reader->OnAutoPrune(m_layTags, m_vHandTagUIs, bChanged, nNewIndex);
	if ( bChanged && nNewIndex < (int)m_vHandTagUIs.size() ) {
		OnLayHandTagSelected(m_vHandTagUIs[nNewIndex]->m_Control);
	}
}

int   CDuiFrameWnd::OnLayHandTagSelected(CControlUI* pCtrl) {
	vector<TagControlItem *>::iterator it;
	int nIndex = -1;
	int i = 0;
	for ( it = m_vHandTagUIs.begin(), i = 0; it != m_vHandTagUIs.end(); ++it, ++i ) {
		TagControlItem * pItem = *it;
		if ( pItem->m_Control == pCtrl) {
			pItem->m_Control->SetBkColor(g_data.m_skin[CMySkin::LAYOUT_READER_BK]);
			m_MyImage_hand_reader->OnTagSelected(it -  m_vHandTagUIs.begin());
			nIndex = i;
		}
		else {
			pItem->m_Control->SetBkColor(0);
		}
	}

	return nIndex;
}

void   CDuiFrameWnd::OnBtnTagNickname(TNotifyUI& msg) {
	CControlUI * pParent = msg.pSender->GetParent();
	CEditUI * pedName = static_cast<CEditUI*>(pParent->FindControl(MY_FINDCONTROLPROC, "edtTagNickName", 0));

	msg.pSender->SetVisible(false);
	pedName->SetText(msg.pSender->GetText());
	pedName->SetVisible(true);
	pedName->SetFocus();
}

void   CDuiFrameWnd::OnEdtTagNicknameKillFocus(TNotifyUI& msg) {
	CControlUI * pParent = msg.pSender->GetParent();
	CControlUI * pGrandParent = pParent->GetParent()->GetParent()->GetParent();
	CButtonUI * pbtnName = static_cast<CButtonUI*>(pParent->FindControl(MY_FINDCONTROLPROC, "btnTagNickName", 0));
	CLabelUI * pLblTagId = static_cast<CLabelUI*>(pGrandParent->FindControl(MY_FINDCONTROLPROC, "lblHandTagId", 0));

	CDuiString strText;
	strText = pbtnName->GetText();

	pbtnName->SetText(msg.pSender->GetText());
	msg.pSender->SetVisible(false);
	pbtnName->SetVisible(true);

	if (strText != msg.pSender->GetText())
		CBusiness::GetInstance()->SaveHandTagNicknameAsyn(pLblTagId->GetText(), pbtnName->GetText());
}

void   CDuiFrameWnd::OnEdtRemarkKillFocus_1() {
	OnEdtRemarkKillFocus1_g(m_MyImage_hand_reader);
}

void   CDuiFrameWnd::OnGridsViewSize(DWORD  dwWidth, DWORD  dwHeight) {
	SIZE s;
	s.cx = (dwWidth - 1) / DRAG_DROP_COLUMNS + 1;
	s.cy = (dwHeight - 1) / DRAG_DROP_ROWS + 1;
	m_layGridsView->SetItemSize(s);
}

int  CDuiFrameWnd::OnMouseMoveGridsView(const POINT & pt) {
	RECT rect = m_layGridsView->GetPos();
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	if (nWidth <= 0 || nHeight <= 0 ) {
		return -1;
	}

	if ( pt.x <= rect.left || pt.x >= rect.right || pt.y <= rect.top || pt.y >= rect.bottom ) {
		for (int i = 0; i < DRAG_DROP_COLUMNS; i++) {
			for (int j = 0; j < DRAG_DROP_ROWS; j++) {
				CLabelUI* pCtl = (CLabelUI*)m_layGridsView->GetItemAt(j*DRAG_DROP_COLUMNS + i);
				pCtl->SetBorderColor(0xFFFFFFFF);
				pCtl->SetTextColor(0xFFFFFFFF);
				pCtl->SetBkColor(0xFF192431);
			}
		}
		return -1;
	}

	SIZE s;
	s.cx = (nWidth - 1) / DRAG_DROP_COLUMNS + 1;
	s.cy = (nHeight - 1) / DRAG_DROP_ROWS + 1;

	int nIndex = -1;
	for (int i = 0; i < DRAG_DROP_COLUMNS; i++) {
		for (int j = 0; j < DRAG_DROP_ROWS; j++) {
			if ( (pt.x > rect.left + s.cx * i) && (pt.x < rect.left + s.cx * (i+1)) 
				&& (pt.y > rect.top + s.cy * j) && (pt.y < rect.top + s.cy * (j+1) ) ) {
				CLabelUI* pCtl = (CLabelUI*)m_layGridsView->GetItemAt(j*DRAG_DROP_COLUMNS + i);
				pCtl->SetBorderColor(0xFFCAF100);
				pCtl->SetTextColor(0xFF000000);
				pCtl->SetBkColor(0xFFCCCCCC);
				nIndex = j*DRAG_DROP_COLUMNS + i;
			}
			else {
				CLabelUI* pCtl = (CLabelUI*)m_layGridsView->GetItemAt(j*DRAG_DROP_COLUMNS + i);
				pCtl->SetBorderColor(0xFFFFFFFF);
				pCtl->SetTextColor(0xFFFFFFFF);
				pCtl->SetBkColor(0xFF192431);
			}
		}
	}
	return nIndex;
}

void  CDuiFrameWnd::OnTagBindingGridsRet(WPARAM wParam, LPARAM  lParam) {
	vector<TagBindingGrid*> * pvRet = (vector<TagBindingGrid*> *)wParam;
	//m_vTagBindingGrids.insert(m_vTagBindingGrids.begin(), pvRet->begin(), pvRet->end());	

	vector<TagControlItem *>::iterator it;
	vector<TagBindingGrid*>::iterator ix;
	for (it = m_vHandTagUIs.begin(); it != m_vHandTagUIs.end(); ++it) {
		TagControlItem * pItem = *it;
		for (ix = pvRet->begin(); ix != pvRet->end(); ++ix) {
			TagBindingGrid* pBinding = *ix;
			if (0 == strcmp(pBinding->m_szTagId, pItem->m_pTagId->c_str())) {
				SetBindingGridText(pItem, pBinding->m_nGridIndex);
			}
		}
	}

	OnMyDeviceChanged();

	ClearVector(*pvRet);
	delete pvRet;
}

//
void  CDuiFrameWnd::OnHandTagBindingGrid(int nTagIndex, int nGridIndex) {
	vector<TagControlItem *>::iterator it;
	int i = 0;
	CDuiString strText;

	for (it = m_vHandTagUIs.begin(), i = 0; it != m_vHandTagUIs.end(); ++it, ++i) {
		TagControlItem * pItem = *it;
		// 绑定
		if ( i == nTagIndex ) {
			SetBindingGridText( pItem, nGridIndex );
		}
		else {
			// 取消绑定
			if (pItem->m_nBindingGridIndex == nGridIndex) {
				SetBindingGridText(pItem, -1);
			}
		}
	}

	CBusiness::GetInstance()->SetTagBindingGridAsyn(m_vHandTagUIs[nTagIndex]->m_pTagId->c_str(), nGridIndex);

	
}

//
void  CDuiFrameWnd::SetBindingGridText( TagControlItem * pItem, int nGridIndex ) {
	CDuiString strText;
	CLabelUI * plblBindingGrid = static_cast<CLabelUI*>(pItem->m_Control->FindControl(MY_FINDCONTROLPROC, "bindingGrid", 0));
	if ( nGridIndex >= 0 ) {
		int x = nGridIndex / MAX_READERS_PER_GRID;
		int y = nGridIndex % MAX_READERS_PER_GRID;

		strText.Format("绑定窗格%02d%c", x+1, 'A'+y);
		plblBindingGrid->SetText(strText);
		pItem->m_nBindingGridIndex = nGridIndex;
		plblBindingGrid->SetTextColor(0xFFCAF100);

		m_MyImage_max[x]->SetTagId(y, pItem->m_pTagId->c_str());
		m_MyImage_max[x]->EmptyData(y);
		m_MyImage_max[x]->Invalidate();
		CBusiness::GetInstance()->QueryTempFromSqliteByTagAsyn(pItem->m_pTagId->c_str(), x, y);
		m_LblTagId[x][y]->SetText(pItem->m_pTagId->c_str());
	}
	else {
		plblBindingGrid->SetText("未绑定窗格");
		plblBindingGrid->SetTextColor(0xFFCCCCCC);

		if (pItem->m_nBindingGridIndex >= 0) {
			int x = pItem->m_nBindingGridIndex / MAX_READERS_PER_GRID;
			int y = pItem->m_nBindingGridIndex % MAX_READERS_PER_GRID;
			m_MyImage_max[x]->SetTagId(y, "");
			m_MyImage_max[x]->EmptyData(y);
			m_MyImage_max[x]->Invalidate();
			m_LblTagId[x][y]->SetText("");
		}
		
		pItem->m_nBindingGridIndex = -1;
	}
}

void   CDuiFrameWnd::PassTemp2Grid(DWORD  dwIndex, DWORD  dwSubIndex, LastTemp * pTemp ) {
	DWORD  dwTemp = pTemp->m_dwTemp;

	memcpy(&m_tLastTemp[dwIndex][dwSubIndex], pTemp, sizeof(LastTemp));
	m_tLastTemp[dwIndex][dwSubIndex].m_Time = time(0);
	CBusiness::GetInstance()->ExcelTempAsyn(dwIndex, dwSubIndex, &m_tLastTemp[dwIndex][dwSubIndex]);

	OnTemp(dwIndex, dwSubIndex, dwTemp, FALSE);

	if (dwTemp > g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_dwHighTempAlarm) {
		m_UiReaderTemp[dwIndex][dwSubIndex]->SetTextColor(g_data.m_skin[CMySkin::HIGH_TEMP_ALARM_TEXT_COLOR]);
	}
	else if (dwTemp < g_data.m_CfgData.m_GridCfg[dwIndex].m_ReaderCfg[dwSubIndex].m_dwLowTempAlarm) {
		m_UiReaderTemp[dwIndex][dwSubIndex]->SetTextColor(g_data.m_skin[CMySkin::LOW_TEMP_ALARM_TEXT_COLOR]);
	}
	else {
		m_UiReaderTemp[dwIndex][dwSubIndex]->SetTextColor(g_data.m_skin[CMySkin::NORMAL_TEMP_TEXT_COLOR]);
	}

	DuiLib::CDuiString  strText;
	strText.Format("%.2f℃", dwTemp / 100.0);
	m_UiReaderTemp[dwIndex][dwSubIndex]->SetText(strText);
	m_lblProcTips->SetText("");

	m_LblReaderId[dwIndex][dwSubIndex]->SetText(pTemp->m_szReaderId);
	m_LblTagId[dwIndex][dwSubIndex]->SetText(pTemp->m_szTagId);
}

int   CDuiFrameWnd::GetHandTagGridIndex(const char * szTagId) {
	assert(szTagId);

	vector<TagControlItem *>::iterator it;
	for (it = m_vHandTagUIs.begin(); it != m_vHandTagUIs.end(); ++it) {
		TagControlItem * pItem = *it;
		if ( 0 == strcmp(szTagId, pItem->m_pTagId->c_str()) ) {
			return pItem->m_nBindingGridIndex;
		}
	}
	return -1;
}

//
int    CDuiFrameWnd::OnMouseMoveGridsView_1(const POINT & pt) {
	RECT rect = m_layMain->GetPos();
	int nWidth = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	if (nWidth <= 0 || nHeight <= 0) {
		return -1;
	}
	
	assert(g_data.m_CfgData.m_dwLayoutColumns > 0);
	assert(g_data.m_CfgData.m_dwLayoutRows > 0);

	if (pt.x <= rect.left || pt.x >= rect.right || pt.y <= rect.top || pt.y >= rect.bottom) {
		m_layMain->SetBkColor(g_data.m_skin[CMySkin::LAYOUT_MAIN_BK]);
		for (int i = 0; i < (int)g_data.m_CfgData.m_dwLayoutColumns; i++) {
			for (int j = 0; j < (int)g_data.m_CfgData.m_dwLayoutRows; j++) {
				CControlUI * pCtl = m_layMain->GetItemAt(j*g_data.m_CfgData.m_dwLayoutColumns + i);
				pCtl->SetBorderColor(g_data.m_skin[CMySkin::GRID_BORDER]);
			}
		}
		return -1;
	}

	
	SIZE s;
	s.cx = (nWidth - 1) / g_data.m_CfgData.m_dwLayoutColumns + 1;
	s.cy = (nHeight - 1) / g_data.m_CfgData.m_dwLayoutRows + 1;

	CDuiString strText;
	int nIndex = -1;
	for (int i = 0; i < (int)g_data.m_CfgData.m_dwLayoutColumns; i++) {
		for (int j = 0; j < (int)g_data.m_CfgData.m_dwLayoutRows; j++) {
			if ((pt.x > rect.left + s.cx * i) && (pt.x < rect.left + s.cx * (i + 1))
				&& (pt.y > rect.top + s.cy * j) && (pt.y < rect.top + s.cy * (j + 1))) {
				CControlUI* pCtl = m_layMain->GetItemAt(j*g_data.m_CfgData.m_dwLayoutColumns + i);
				pCtl->SetBorderColor(0xFFCAF100);
				nIndex = j*g_data.m_CfgData.m_dwLayoutColumns + i;
			}
			else {
				CControlUI* pCtl = m_layMain->GetItemAt(j*g_data.m_CfgData.m_dwLayoutColumns + i);
				pCtl->SetBorderColor(g_data.m_skin[CMySkin::GRID_BORDER]);
			}
		}
	}

	return nIndex;
}


void PrintStatus(int nCnt, void * args[]) {
	CBusiness::GetInstance()->PrintStatusAsyn();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#if DBG_FLAG
	SetUnhandledExceptionFilter(pfnUnhandledExceptionFilter);
#endif

	HANDLE handle = ::CreateMutex(NULL, FALSE, GLOBAL_LOCK_NAME);//handle为声明的HANDLE类型的全局变量 
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		::MessageBox(0, "程序已经打开或没有关闭完全，请先关闭或等待完全关闭!", "错误", 0);
		return 0;
	}

	JTelSvrRegCommand("status", "print status", PrintStatus, 0);

	DWORD  dwPort = 2018;
	JTelSvrStart((unsigned short)dwPort, 10);
	
	CoInitialize(NULL);
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
	CoUninitialize();

	return 0;
}
              
 
    