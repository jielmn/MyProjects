// win32_1.cpp : ����Ӧ�ó������ڵ㡣
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

		/* �ര����ʾ */
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
		/* END �ര����ʾ */

		/* �����ʾ */
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
			m_UiReaderSwitch[i][j]->SetTag(j);
			
			m_UiReaderTemp[i][j] = static_cast<CLabelUI *>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, READER_TEMP_NAME, 0));
			m_UiReaderTemp[i][j]->SetTag(j);

			m_UiBtnReaderNames[i][j] = static_cast<CButtonUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, BTN_READER_NAME, 0));
			m_UiBtnReaderNames[i][j]->SetTag(j);

			m_UiEdtReaderNames[i][j] = static_cast<CEditUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, EDT_READER_NAME, 0));
			m_UiEdtReaderNames[i][j]->SetTag(j);

			m_UiAlarms[i][j] = static_cast<CAlarmImageUI*>(m_UiReaders[i][j]->FindControl(MY_FINDCONTROLPROC, ALARM_IMAGE_NAME, 0));
			m_UiAlarms[i][j]->SetTag(j);

			m_UiIndicator[i][j]->SetBkColor( g_data.m_skin.GetReaderIndicator(j) );
			m_UiReaderTemp[i][j]->SetText("--");
			m_UiBtnReaderNames[i][j]->SetText("--");
			m_UiReaderSwitch[i][j]->Selected(g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_bSwitch ? true : false);

			m_layReaders[i]->Add(m_UiReaders[i][j]);
		}
		/* END �����ʾ */

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
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void CDuiFrameWnd::OnFinalMessage(HWND hWnd) {
	// ����û����ӽ�layMain��grids
	if (m_eGridStatus == GRID_STATUS_MAXIUM ) {
		for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
			if (m_dwInflateGridIndex != i) {
				m_pGrids[i]->Delete();
			}
		}
	}
}

void   CDuiFrameWnd::OnSize(WPARAM wParam, LPARAM lParam) {
	DWORD dwWidth = LOWORD(lParam) - LAYOUT_WINDOW_HMARGIN * 2;           // layWindow������margin
																		  // 30�ǵײ�status�Ĵ��¸߶�, 1��layWindow������margin, 32�Ǳ������߶�
	DWORD dwHeight = HIWORD(lParam) - STATUS_PANE_HEIGHT - LAYOUT_WINDOW_VMARGIN * 2 - WINDOW_TITLE_HEIGHT;
	ReLayout(dwWidth, dwHeight);
}

// ���²���
void   CDuiFrameWnd::ReLayout(DWORD dwWidth, DWORD dwHeight) {
	if (0 == m_layMain) {
		return;
	}

	SIZE s;
	if (GRID_STATUS_GRIDS == m_eGridStatus) {
		s.cx = (dwWidth - 1) / g_data.m_CfgData.m_dwLayoutColumns + 1;
		s.cy = (dwHeight - 1) / g_data.m_CfgData.m_dwLayoutRows + 1;
		m_layMain->SetItemSize(s);

		// ���������������ĸ߶�
		m_layStatus->SetFixedHeight(STATUS_PANE_HEIGHT - (s.cy * g_data.m_CfgData.m_dwLayoutRows - dwHeight));
	}
	else {
		s.cx = dwWidth;
		s.cy = dwHeight;
		m_layMain->SetItemSize(s);

		// ���������������ĸ߶�
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
		m_EdtBed_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_BtnName_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_EdtName_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_LblCurTemp_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_LblBedTitle_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_LblNameTitle_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_LblCurTempTitle_grid[i]->SetTextColor(g_data.m_skin[CMySkin::COMMON_TEXT]);
		m_MyImage_grid[i]->SetBkColor(g_data.m_skin[CMySkin::MYIMAGE_BK]);
		m_MyImage_grid[i]->OnChangeSkin();
		m_OptGridSwitch_grid[i]->SetSelectedImage(g_data.m_skin.GetImageName(CMySkin::OPT_SELECTED));
		m_OptGridSwitch_grid[i]->SetNormalImage(g_data.m_skin.GetImageName(CMySkin::OPT_NOT_SELECTED));
	} 
}

void   CDuiFrameWnd::OnDbClick() {
	POINT point;
	CDuiString strName;

	GetCursorPos(&point);
	::ScreenToClient(m_hWnd, &point);
	CControlUI * pFindControl = m_PaintManager.FindSubControlByPoint(m_layMain, point);
	if (pFindControl) {
		// ���˫����ť������
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

// �����������С
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
	pSettingDlg->Create(this->m_hWnd, _T("����"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pSettingDlg->CenterWindow();
	int ret = pSettingDlg->ShowModal();
	// �����click ok
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

		// �����ڴ�䶯
		if (oldData.m_dwSkinIndex != g_data.m_CfgData.m_dwSkinIndex) {
			g_data.m_skin.SetSkin((CMySkin::ENUM_SKIN)g_data.m_CfgData.m_dwSkinIndex);
			OnChangeSkin();
		}
	}
	delete pSettingDlg;
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
}

void  CDuiFrameWnd::OnBtnName_grid(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	msg.pSender->SetVisible(false);
	m_EdtName_grid[nIndex]->SetText(msg.pSender->GetText());
	m_EdtName_grid[nIndex]->SetVisible(true);
	m_EdtName_grid[nIndex]->SetFocus();
}

void   CDuiFrameWnd::OnEdtNameKillFocus_grid(TNotifyUI& msg) {
	int nIndex = msg.pSender->GetTag();
	STRNCPY(g_data.m_CfgData.m_GridCfg[nIndex].m_szName, msg.pSender->GetText(), MAX_NAME_LENGTH);

	CDuiString  strText;
	strText.Format(CFG_PATIENT_NAME " %d", nIndex + 1);
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[nIndex].m_szName, "");
	g_data.m_cfg->Save();

	msg.pSender->SetVisible(false);
	m_BtnName_grid[nIndex]->SetText(msg.pSender->GetText());
	m_BtnName_grid[nIndex]->SetVisible(true);
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

	// ����icon
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
            

