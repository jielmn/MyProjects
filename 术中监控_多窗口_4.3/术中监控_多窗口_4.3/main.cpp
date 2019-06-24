// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "DuiMenu.h"
#include "AboutDlg.h"
#include "SettingDlg.h"
#include "TagUI.h"

#define   TIMER_DRAG_DROP_GRID                   1001
#define   INTERVAL_TIMER_DRAG_DROP_GRIDS         1500

// 1分钟定时器
#define   TIMER_60_SECONDS                       1002
#define   INTERVAL_TIMER_60_SECONDS              60000
  
CDuiFrameWnd::CDuiFrameWnd() : m_callback(&m_PaintManager) {	
	m_dwCurSelGridIndex = 0;
	m_eGridStatus = GRID_STATUS_GRIDS;

	m_tabs = 0;
	m_layMain = 0;
	memset(m_pGrids, 0, sizeof(m_pGrids));
	m_layPages = 0;
	m_btnPrevPage = 0;
	m_btnNextPage = 0;

	m_nDgSourceIndex = -1;
	m_nDgDestIndex = -1;
	m_dragdropGrid = 0;
	m_bDragTimer = FALSE;
	m_bFlipPrevPage = FALSE;

	m_btnMenu = 0;
	m_dwDgStartTick = 0;

	m_lblBarTips = 0;
	m_lblLaunchStatus = 0;

	m_cstHandImg = 0;
	m_layTags = 0;
	m_dragdrop_tag_dest_index = -1;
	m_hand_tabs = 0;
	m_dragdrop_tag_timetick = 0;
	m_layDragDropGrids = 0;

	m_LastSaveExcelTime = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::RemoveAllGrids() {
	// 清空子控件
	int nCnt = m_layMain->GetCount();
	for (int i = 0; i < nCnt; i++) {
		m_layMain->RemoveAt(0, true);
	}
}

void CDuiFrameWnd::OnFinalMessage(HWND hWnd) {
	RemoveAllGrids();

	// 销毁grids
	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		assert(m_pGrids[i]);
		m_pGrids[i]->Delete();
	}
}

void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	g_data.m_hWnd = GetHWND();
	g_data.m_hCursor = LoadCursor(NULL, IDC_SIZENS);

	/*************  获取控件 *****************/
	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(TABS_ID));
	m_layMain = static_cast<DuiLib::CTileLayoutUI*>(m_PaintManager.FindControl(LAYOUT_MAIN_NAME));
	m_layPages = static_cast<DuiLib::CHorizontalLayoutUI *>(m_PaintManager.FindControl(LAYOUT_PAGES));
	m_btnPrevPage = static_cast<DuiLib::CButtonUI *>(m_PaintManager.FindControl(BUTTON_PREV_PAGE));
	m_btnNextPage = static_cast<DuiLib::CButtonUI *>(m_PaintManager.FindControl(BUTTON_NEXT_PAGE));
	m_dragdropGrid = m_PaintManager.FindControl(DRAG_DROP_GRID); 
	m_btnMenu = static_cast<CButtonUI*>(m_PaintManager.FindControl(BTN_MENU));
	m_lblBarTips = static_cast<CLabelUI*>(m_PaintManager.FindControl(LBL_BAR_TIPS));
	m_lblLaunchStatus = static_cast<CLabelUI*>(m_PaintManager.FindControl(LBL_LAUNCH_STATUS));
	m_lblSurReaderTry = static_cast<CLabelUI*>(m_PaintManager.FindControl(LBL_PROC_TIPS));

	// 添加窗格
	for ( DWORD i = 0; i < MAX_GRID_COUNT; i++ ) {
		m_pGrids[i] = new CGridUI;
		m_pGrids[i]->SetBorderSize(GRID_BORDER_SIZE);
		m_pGrids[i]->SetBorderColor(GRID_BORDER_COLOR);
		m_pGrids[i]->SetName(GRID_NAME);
		m_pGrids[i]->SetTag(i);
		m_pGrids[i]->SetBedNo(i + 1);
		m_pGrids[i]->SetMode( (CModeButton::Mode)g_data.m_CfgData.m_GridCfg[i].m_dwGridMode );
		m_layMain->Add(m_pGrids[i]);
		m_pGrids[i]->SetPatientName(g_data.m_CfgData.m_GridCfg[i].m_szPatientName);
	}
	m_cstHandImg = static_cast<CMyHandImage *>(m_PaintManager.FindControl(CST_HAND_IMAGE));
	m_layTags = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(LAYOUT_TAGS));

	g_data.m_DragDropCtl = m_PaintManager.FindControl(DRAG_DROP_CTL); 
	g_data.m_edRemark    = static_cast<CEditUI *>( m_PaintManager.FindControl(EDIT_REMARK) );
	g_data.m_edHandRemark = static_cast<CEditUI *>(m_PaintManager.FindControl(EDIT_HAND_REMARK)); 

	m_optDefaultSort = static_cast<COptionUI *>(m_PaintManager.FindControl(OPT_DEFAULT));
	m_optTimeSort = static_cast<COptionUI *>(m_PaintManager.FindControl(OPT_TIME));

	m_hand_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(HAND_TABS_ID));
	m_layDragDropGrids = static_cast<CTileLayoutUI *>(m_PaintManager.FindControl(LAY_DRAGDROP_GRIDS));
	/*************  end 获取控件 *****************/

	m_cstHandImg->m_sigTagErased.connect(this, &CDuiFrameWnd::OnHandTagErasedNotify);

	RefreshGridsPage();

	// 放在prepared后
	//CheckDevice();
	CBusiness::GetInstance()->PrepareAsyn();

	/***** 启动定时器 ****/
	SetTimer( GetHWND(), TIMER_60_SECONDS, INTERVAL_TIMER_60_SECONDS, 0 );
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	DuiLib::CDialogBuilder builder;
	DuiLib::CDuiString  strText;
	DuiLib::CControlUI * pUI = 0;

	if ( 0 == strcmp("SurgencyTemp", pstrClass) ) {		
		strText.Format( "%s.xml", pstrClass );
		pUI = builder.Create( (const char *)strText, (UINT)0, &m_callback, &m_PaintManager );
		return pUI; 
	}
	else if (0 == strcmp("HandReaderTemp", pstrClass)) {
		strText.Format("%s.xml", pstrClass);
		pUI = builder.Create((const char *)strText, (UINT)0, &m_callback, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("DragDrop", pstrClass)) {
		return new CDragDropUI;
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();

	if (msg.sType == _T("selectchanged"))
	{
		if (name == _T("opn_monitor")) {
			m_tabs->SelectItem(TAB_INDEX_MONITOR);
		}
		else if (name == _T("opn_reader")) {
			m_tabs->SelectItem(TAB_INDEX_READER);                  
		}
		else if ( name == "rdTimeOrder" ) {
			OnHandTagTimeOrder();
		}
	}
	else if (msg.sType == "click") {
		if ( name == BUTTON_NEXT_PAGE ) {
			NextPage();
		}
		else if (name == BUTTON_PREV_PAGE) {
			PrevPage();
		}
		else if (name == BTN_MENU) {
			OnBtnMenu(msg);
		}
	}
	else if (msg.sType == "setting") {
		OnSetting();
	}
	else if (msg.sType == "about") {
		OnAbout();
	}
	else if (msg.sType == "killfocus") {
		if (name == EDIT_REMARK) {
			::OnEdtRemarkKillFocus();
		}
		else if (name == EDIT_HAND_REMARK) {
			::OnEdtHandRemarkKillFocus();
		}
	}
	else if (msg.sType == "menu") {
		if (name == CST_IMG_LBL_TEMP) { 
			OnImageMenu(msg.pSender->GetTag(), msg.ptMouse, msg.pSender);
		}
		else if (name == CST_IMAGE) {
			OnImageMenu(msg.pSender->GetTag(), msg.ptMouse, msg.pSender);
		}
		else if (name == "cstMyImageHand") {
			OnHandImageMenu(msg.ptMouse, msg.pSender);
		}
	}
	else if ( msg.sType == "menu_export_excel" ) {
		OnExportExcel(msg.wParam);
	}
	else if ( msg.sType == "menu_print_excel" ) {
		OnPrintExcel(msg.wParam);
	}
	else if (msg.sType == "menu_hand_export_excel") {
		OnHanxExportExcel();
	}
	else if (msg.sType == "menu_hand_print_excel") {
		OnHandPrintExcel();
	}
	else if (msg.sType == "tag_selected") {
		OnHandTagSelected(msg.pSender);
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_LBUTTONDBLCLK) {
		BOOL bHandled = FALSE;
		OnDbClick(bHandled);
		if (bHandled)
			return 0;
	}
	else if (uMsg == WM_TIMER) {
		if (wParam == TIMER_DRAG_DROP_GRID) {
			OnFlipPage();
		}
		else if (wParam == TIMER_60_SECONDS) {
			On60SecondsTimer();
		}
	}
	else if (uMsg == WM_DEVICECHANGE) {
		CheckDevice();
	}
	else if (uMsg == UM_LAUNCH_STATUS) {
		OnLaunchStatus(wParam, lParam);
	}
	else if (uMsg == UM_TRY_SUR_READER) {
		OnTrySurReader(wParam, lParam);
	}
	else if (uMsg == UM_SUR_READER_STATUS) {
		OnSurReaderStatus(wParam, lParam);
	}
	else if (uMsg == UM_SUR_READER_TEMP) {
		OnSurReaderTemp(wParam, lParam);
	}
	else if (uMsg == UM_QUERY_TEMP_BY_TAG_ID_RET) {
		OnQueryTempRet(wParam, lParam);
	}
	else if (uMsg == UM_HAND_READER_TEMP) {
		OnHandReaderTemp(wParam, lParam);
	}
	else if (uMsg == UM_PREPARED) {
		OnPrepared(wParam, lParam);
	}
	else if (uMsg == UM_ALL_HAND_TAG_TEMP_DATA) {
		OnAllHandTagTempData(wParam, lParam);
	}
	else if (uMsg == UM_BINDING_TAG_GRID_RET) {
		OnTagBindingGridRet(wParam, lParam);
	}
	else if (uMsg == UM_QUERY_HAND_TEMP_BY_TAG_ID_RET) {
		OnQueryHandTempRet(wParam, lParam);
	}
	else if ( uMsg == UM_TAG_NAME_CHANGED ) {
		OnTagNameChanged( wParam, lParam );
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

LRESULT  CDuiFrameWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	// layWindow的左右margin
	DWORD dwWidth = LOWORD(lParam) - 10 * 2;           
	// 30是底部status的高度, 32是标题栏高度, 85是tab页高度
	DWORD dwHeight = HIWORD(lParam) - 30 - 1 * 2 - 32 - 85;
	RefreshGridsSize(dwWidth, dwHeight);

	return WindowImplBase::OnSize(uMsg, wParam, lParam, bHandled);
}

LRESULT  CDuiFrameWnd::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	POINT pt;
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	CControlUI* pCtl = m_PaintManager.FindSubControlByPoint(0, pt);
	// 如果没有点击到任何控件
	if (0 == pCtl) {
		return WindowImplBase::OnLButtonDown(uMsg, wParam, lParam, bHandled);
	}

	// 保存原始点击的控件
	CControlUI* pOriginalCtl = pCtl;
	int nSelTab = m_tabs->GetCurSel();

	// 如果是第一Tab页
	if ( nSelTab == TAB_INDEX_MONITOR ) {
		// 如果是多格子状态
		if (m_eGridStatus == GRID_STATUS_GRIDS) {
			while (pCtl) {
				if (pCtl->GetName() == GRID_NAME) {
					// 如果不是点击修改名字按钮
					if (0 != strcmp(pOriginalCtl->GetClass(), "Button")
						&& 0 != strcmp(pOriginalCtl->GetClass(), "ModeButton")) {
						m_nDgSourceIndex = GetGridOrderByGridId(pCtl->GetTag());
						m_nDgDestIndex = -1;
						m_dwDgStartTick = LmnGetTickCount();
					}
					break;
				}
				pCtl = pCtl->GetParent();
			}
		}
		// 如果是最大化状态
		else if (m_eGridStatus == GRID_STATUS_MAXIUM) {
			while (pCtl) {
				if (pCtl->GetName() == LAY_READER) {
					CReaderUI * pReader = (CReaderUI *)pCtl;
					DWORD i = pReader->GetGridIndex();
					DWORD j = pReader->GetReaderIndex();
					m_pGrids[i]->OnSurReaderSelected(j);
					break;
				}
				pCtl = pCtl->GetParent();
			}
		}
	}
	else if (nSelTab == TAB_INDEX_READER) {
		while (pCtl) {
			if ( 0 == strcmp( pCtl->GetClass(), "TagUI") ) {
				// 不是点击了按钮
				if ( 0 != strcmp( pOriginalCtl->GetClass(), "Button") ) {
					CTagUI * pTagUI = (CTagUI *)pCtl;
					m_dragdrop_tag = pTagUI->GetTagId();
					m_dragdrop_tag_dest_index = -1;
					m_dragdrop_tag_timetick = LmnGetTickCount();
				}
			}
			pCtl = pCtl->GetParent();
		}
	}
	
	
	return WindowImplBase::OnLButtonDown(uMsg, wParam, lParam, bHandled);
}

LRESULT  CDuiFrameWnd::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	if (m_nDgSourceIndex >= 0) {
		StopMoveGrid();
	}
	// 正在拖动tag ui
	else if (m_dragdrop_tag.GetLength() > 0) {
		StopMoveTagUI();
	}
	return WindowImplBase::OnLButtonUp(uMsg, wParam, lParam, bHandled);
}

LRESULT  CDuiFrameWnd::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	POINT  pt;
	DWORD  dwCurTick = 0;

	if (m_nDgSourceIndex >= 0) {
		dwCurTick = LmnGetTickCount();
		if (dwCurTick - m_dwDgStartTick < DRAG_DROP_DELAY_TIME) {
			return WindowImplBase::OnMouseMove(uMsg, wParam, lParam, bHandled);
		}

		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		MoveGrid(pt);
	}
	// 正在拖动tag ui
	else if (m_dragdrop_tag.GetLength() > 0) {
		dwCurTick = LmnGetTickCount();
		// 超过1秒
		if ( dwCurTick - m_dragdrop_tag_timetick >= 100 ) {
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			MoveTagUI(pt);
		}
	}
	return WindowImplBase::OnMouseMove(uMsg, wParam, lParam, bHandled);
}

// 移动格子，调整顺序
void   CDuiFrameWnd::MoveGrid(const POINT & pt) {
	if (!m_dragdropGrid->IsVisible()) {
		m_dragdropGrid->SetVisible(true);
	}

	int x = 100 / 2;
	int y = 100 / 2;

	RECT r;
	r.left   = pt.x - x;
	r.right  = r.left + x * 2;
	r.top    = pt.y - y;
	r.bottom = r.top + y * 2;
	m_dragdropGrid->SetPos(r);

	// 高亮经过的格子
	OnMoveGrid(pt);
}

// 格子移动过程中，经过的格子要高亮
void   CDuiFrameWnd::OnMoveGrid(const POINT & pt) {
	RECT rect   = m_layMain->GetPos();
	int nWidth  = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;

	if ( nWidth <= 0 || nHeight <= 0 ) {
		return;
	}

	assert(g_data.m_CfgData.m_dwLayoutColumns > 0);
	assert(g_data.m_CfgData.m_dwLayoutRows > 0);

	// 每页多少grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// 当前页index
	DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
	// 当前页的第一个grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
	// 当前页的最大grid index
	DWORD   dwMaxGridIndex = dwFirstGridIndexCurPage + dwCntPerPage < g_data.m_CfgData.m_dwLayoutGridsCnt
		? dwFirstGridIndexCurPage + dwCntPerPage : g_data.m_CfgData.m_dwLayoutGridsCnt;

	// 超出了范围
	if ( pt.x <= rect.left || pt.x >= rect.right || pt.y <= rect.top || pt.y >= rect.bottom ) {	
		if (m_nDgDestIndex >= 0) {
			m_pGrids[g_data.m_CfgData.m_GridOrder[m_nDgDestIndex]]->SetBorderColor(GRID_BORDER_COLOR);
		}
		m_nDgDestIndex = -1;

		// 如果是垂直方向超出范围，水平方向没有超出
		if ( pt.x > rect.left && pt.x < rect.right ) {
			// 进入了确定区域，关闭Timer
			if (m_bDragTimer) {
				KillTimer( GetHWND(), TIMER_DRAG_DROP_GRID );
				m_bDragTimer = FALSE;
			}
		}
		// 如果是水平方向超出范围，垂直方向没有超出
		else if (pt.y > rect.top && pt.y < rect.bottom) {
			// 可能上下翻页，需要开启定时器
			// 向前翻页
			if ( pt.x <= rect.left ) {
				// 如果可以向前翻页
				if ( dwGridPageIndex > 0) {
					// 如果没有开启定时器
					if (!m_bDragTimer) {
						SetTimer(GetHWND(), TIMER_DRAG_DROP_GRID, INTERVAL_TIMER_DRAG_DROP_GRIDS, 0);
						m_bDragTimer = TRUE;
						m_bFlipPrevPage = TRUE;
					}
				}				
			}
			// 向后翻页
			else if (pt.x >= rect.right) {
				// 可以向后翻页
				if ( dwMaxGridIndex < g_data.m_CfgData.m_dwLayoutGridsCnt - 1 ) {
					// 如果没有开启定时器
					if (!m_bDragTimer) {
						SetTimer(GetHWND(), TIMER_DRAG_DROP_GRID, INTERVAL_TIMER_DRAG_DROP_GRIDS, 0);
						m_bDragTimer = TRUE;
						m_bFlipPrevPage = FALSE;
					}
				}
			}
		}
		// 水平和垂直方向都超出范围
		else {
			// 进入了确定区域，关闭Timer
			if (m_bDragTimer) {
				KillTimer(GetHWND(), TIMER_DRAG_DROP_GRID);
				m_bDragTimer = FALSE;
			}
		}
		return;
	}

	SIZE s = m_layMain->GetItemSize();
	for ( DWORD i = dwFirstGridIndexCurPage; i < dwMaxGridIndex; i++ ) {
		int  nRowIndex = (i - dwFirstGridIndexCurPage) / g_data.m_CfgData.m_dwLayoutColumns;
		int  nColIndex = (i - dwFirstGridIndexCurPage) % g_data.m_CfgData.m_dwLayoutColumns;
		if ((pt.x > rect.left + s.cx * nColIndex) && (pt.x < rect.left + s.cx * (nColIndex + 1))
			&& (pt.y > rect.top + s.cy * nRowIndex) && (pt.y < rect.top + s.cy * (nRowIndex + 1))) {
			m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetBorderColor(GRID_HILIGHT_BORDER_COLOR);
			m_nDgDestIndex = i;
		}
		else {
			m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetBorderColor(GRID_BORDER_COLOR);
		}
	}
	KillTimer(GetHWND(), TIMER_DRAG_DROP_GRID);
	m_bDragTimer = FALSE;
}

// 停止移动格子，确定最终位置
void  CDuiFrameWnd::StopMoveGrid() {
	m_dragdropGrid->SetVisible(false);
	KillTimer(GetHWND(), TIMER_DRAG_DROP_GRID);
	m_bDragTimer = FALSE;

	// 没有选定要拖动的位置
	if (m_nDgDestIndex < 0) {
		m_nDgSourceIndex = -1;
		m_nDgDestIndex = -1;
		return;
	}

	// 取消高亮的border
	assert(m_nDgDestIndex < (int)g_data.m_CfgData.m_dwLayoutGridsCnt);
	m_pGrids[g_data.m_CfgData.m_GridOrder[m_nDgDestIndex]]->SetBorderColor(GRID_BORDER_COLOR);

	// 如果source和dest相等
	if (m_nDgSourceIndex == m_nDgDestIndex) {
		m_nDgSourceIndex = -1;
		m_nDgDestIndex = -1;
		return;
	}

	// 例如：grid 2拖到grid 4位置
	// 新位置排序: 1, 3, 4, 2
	if ( m_nDgSourceIndex < m_nDgDestIndex ) {
		DWORD  dwSourceId = g_data.m_CfgData.m_GridOrder[m_nDgSourceIndex];
		for ( int i = m_nDgSourceIndex; i < m_nDgDestIndex; i++ ) {
			g_data.m_CfgData.m_GridOrder[i] = g_data.m_CfgData.m_GridOrder[i + 1];
		}
		g_data.m_CfgData.m_GridOrder[m_nDgDestIndex] = dwSourceId;
		m_dwCurSelGridIndex = m_nDgDestIndex;
	}
	else {
		DWORD  dwSourceId = g_data.m_CfgData.m_GridOrder[m_nDgSourceIndex];
		for (int i = m_nDgSourceIndex; i > m_nDgDestIndex; i--) {
			g_data.m_CfgData.m_GridOrder[i] = g_data.m_CfgData.m_GridOrder[i - 1];
		}
		g_data.m_CfgData.m_GridOrder[m_nDgDestIndex] = dwSourceId;
		m_dwCurSelGridIndex = m_nDgDestIndex;
	}
	RefreshGridsPage();

	char szDefaultOrder[256];	
	GetDefaultGridOrder(szDefaultOrder, sizeof(szDefaultOrder), g_data.m_CfgData.m_dwLayoutGridsCnt);

	char szOrder[256];
	GetGridOrder(szOrder, sizeof(szOrder), g_data.m_CfgData.m_dwLayoutGridsCnt, g_data.m_CfgData.m_GridOrder);

	g_data.m_cfg->SetConfig ( CFG_GRIDS_ORDER, szOrder, szDefaultOrder );
	g_data.m_cfg->Save();

	m_nDgSourceIndex = -1;
	m_nDgDestIndex = -1;
}

// 拖动格子过程中，拖到最左或最右，导致翻页
void  CDuiFrameWnd::OnFlipPage() {
	// 每页多少grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// 当前页index
	DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
	// 当前页的第一个grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
	// 当前页的最大grid index
	DWORD   dwMaxGridIndex = dwFirstGridIndexCurPage + dwCntPerPage < g_data.m_CfgData.m_dwLayoutGridsCnt
		? dwFirstGridIndexCurPage + dwCntPerPage : g_data.m_CfgData.m_dwLayoutGridsCnt;

	if (m_bFlipPrevPage) {
		if ( dwGridPageIndex > 0 ) {
			m_dwCurSelGridIndex = dwFirstGridIndexCurPage - dwCntPerPage;
			RefreshGridsPage();
		}
		else {
			KillTimer(GetHWND(), TIMER_DRAG_DROP_GRID);
			m_bDragTimer = FALSE;
		}
	}
	else {
		if ( dwMaxGridIndex < g_data.m_CfgData.m_dwLayoutGridsCnt - 1 ) {
			m_dwCurSelGridIndex = dwFirstGridIndexCurPage + dwCntPerPage;
			RefreshGridsPage();
		}
		else {
			KillTimer(GetHWND(), TIMER_DRAG_DROP_GRID);
			m_bDragTimer = FALSE;
		}
	}
}

// 更新grids一页(比如点击了"上一页", "下一页"，初始化等等)
void   CDuiFrameWnd::RefreshGridsPage() {
	// 先清空所有的grids
	RemoveAllGrids();

	/**** 再添加需要的grids  ***/

	// 多格子状态
	if (m_eGridStatus == GRID_STATUS_GRIDS) {
		// 每页多少grids
		DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
		// 当前页index
		DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
		// 当前页的第一个grid index
		DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
		// 当前页的最大grid index
		DWORD   dwMaxGridIndex = dwFirstGridIndexCurPage + dwCntPerPage  < g_data.m_CfgData.m_dwLayoutGridsCnt
			? dwFirstGridIndexCurPage + dwCntPerPage : g_data.m_CfgData.m_dwLayoutGridsCnt;

		// 设定固定列数
		m_layMain->SetFixedColumns(g_data.m_CfgData.m_dwLayoutColumns);
		for (DWORD i = dwFirstGridIndexCurPage; i < dwMaxGridIndex; i++) {
			assert(g_data.m_CfgData.m_GridOrder[i] < g_data.m_CfgData.m_dwLayoutGridsCnt);
			m_layMain->Add(m_pGrids[g_data.m_CfgData.m_GridOrder[i]]);
			// m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetVisible(true);
			m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetInternVisible(true);
			m_pGrids[g_data.m_CfgData.m_GridOrder[i]]->SetView(0);
		}

		// 如果需要多页显示
		if (g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows
			< g_data.m_CfgData.m_dwLayoutGridsCnt) {
			m_layPages->SetVisible(true);

			if ( dwGridPageIndex == 0 ) {
				m_btnPrevPage->SetEnabled(false);
			}
			else {
				m_btnPrevPage->SetEnabled(true);
			}

			if (dwGridPageIndex == (g_data.m_CfgData.m_dwLayoutGridsCnt - 1) / dwCntPerPage) {
				m_btnNextPage->SetEnabled(false); 
			}
			else {
				m_btnNextPage->SetEnabled(true);
			}
		}
		else {
			m_layPages->SetVisible(false);
		}
	}
	else {
		// 设定固定列数
		m_layMain->SetFixedColumns(1);
		// 添加单个窗格
		m_layMain->Add(m_pGrids[g_data.m_CfgData.m_GridOrder[m_dwCurSelGridIndex]]);
		// 关闭上下页
		m_layPages->SetVisible(false);
	}
	
}

// 更新grids size
// width, height: m_layMain的parent的
void   CDuiFrameWnd::RefreshGridsSize(int width, int height) {
	if (0 == m_layMain)
		return;

	SIZE s;
	CContainerUI * pParent = (CContainerUI *)m_layMain->GetParent();

	if ( GRID_STATUS_GRIDS == m_eGridStatus ) { 

		// 如果需要多页显示
		if (g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows
			< g_data.m_CfgData.m_dwLayoutGridsCnt) {
			height -= 30;
		}

		s.cx = ( width - 1 ) / (int)g_data.m_CfgData.m_dwLayoutColumns + 1;
		s.cy = ( height - 1 ) / (int)g_data.m_CfgData.m_dwLayoutRows + 1;
		OnGridSizeChanged(s);
		m_layMain->SetItemSize(s);
	}
	else {
		// height += 30;
		s.cx = width;
		s.cy = height;
		m_layMain->SetItemSize(s);
	}
}

void   CDuiFrameWnd::RefreshGridsSize() {
	if (0 == m_layMain)
		return;
	CControlUI* pParent = m_layMain->GetParent();
	RECT rect = pParent->GetPos();
	RefreshGridsSize(rect.right - rect.left, rect.bottom - rect.top);
}

void   CDuiFrameWnd::NextPage() {
	// 每页多少grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// 当前页index
	DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
	// 当前页的第一个grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
	// 下一页的第一grid index，并修改当前的grid index
	m_dwCurSelGridIndex = dwFirstGridIndexCurPage + dwCntPerPage;

	RefreshGridsPage();
}

void   CDuiFrameWnd::PrevPage() {
	// 每页多少grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// 当前页index
	DWORD   dwGridPageIndex = m_dwCurSelGridIndex / dwCntPerPage;
	// 当前页的第一个grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;

	// 上一页的第一grid index，并修改当前的grid index
	assert(dwFirstGridIndexCurPage >= dwCntPerPage);
	m_dwCurSelGridIndex = dwFirstGridIndexCurPage - dwCntPerPage;

	RefreshGridsPage();
}

void  CDuiFrameWnd::OnDbClick(BOOL & bHandled) {
	POINT point;
	CDuiString strName;
	bHandled = FALSE;

	GetCursorPos(&point);
	::ScreenToClient(m_hWnd, &point);

	CControlUI * pFindControl = m_PaintManager.FindSubControlByPoint(m_tabs, point);
	if (0 == pFindControl) {
		return;
	}

	DuiLib::CDuiString  clsName = pFindControl->GetClass();
	// 如果双击一些按钮，滑动条类的，不处理
	if (0 == strcmp(clsName, DUI_CTR_BUTTON)) {
		return;
	}
	else if (0 == strcmp(clsName, DUI_CTR_OPTION)) {
		return;
	}
	else if (0 == strcmp(clsName, DUI_CTR_SCROLLBAR)) {
		return;
	}
	else if (0 == strcmp(clsName, "MyImage")) {
		return; 
	}
	else if (0 == strcmp(clsName, "ModeButton")) {
		return;
	}

	while (pFindControl) {
		strName = pFindControl->GetName();
		if (0 == strcmp(strName, GRID_NAME)) {

			if (m_eGridStatus == GRID_STATUS_GRIDS) {
				// 找到序号
				DWORD dwGridIndex = GetGridOrderByGridId(pFindControl->GetTag());
				m_dwCurSelGridIndex = dwGridIndex;
				m_eGridStatus = GRID_STATUS_MAXIUM;
			}
			else {
				m_eGridStatus = GRID_STATUS_GRIDS;
			}
			
			CGridUI * pGrid = (CGridUI *)pFindControl;
			pGrid->SwitchView(); 

			RefreshGridsPage();
			RefreshGridsSize();

			bHandled = TRUE;
			break;
		}
		pFindControl = pFindControl->GetParent();
	}
}

// 点击了“菜单”按钮
void   CDuiFrameWnd::OnBtnMenu(TNotifyUI& msg) {
	RECT r = m_btnMenu->GetPos();
	POINT pt = { r.left, r.bottom };
	CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);

	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

// 点击了“设置”
void  CDuiFrameWnd::OnSetting() {
	CDuiString  strText;
	DWORD  dwValue = 0;
	BOOL   bValue = FALSE;

	CfgData  oldData = g_data.m_CfgData;
	CSettingDlg * pSettingDlg = new CSettingDlg;
	pSettingDlg->m_data = g_data.m_CfgData;
	pSettingDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pSettingDlg->CenterWindow();
	int ret = pSettingDlg->ShowModal();

	// 如果不是click ok
	if (0 != ret) {
		delete pSettingDlg;
		return;
	}

	g_data.m_CfgData = pSettingDlg->m_data;

	dwValue = DEFAULT_MAIN_LAYOUT_COLUMNS;
	g_data.m_cfg->SetConfig(CFG_MAIN_LAYOUT_COLUMNS, g_data.m_CfgData.m_dwLayoutColumns, &dwValue);

	dwValue = DEFAULT_MAIN_LAYOUT_ROWS;
	g_data.m_cfg->SetConfig(CFG_MAIN_LAYOUT_ROWS, g_data.m_CfgData.m_dwLayoutRows, &dwValue);

	dwValue = DEFAULT_MAIN_LAYOUT_GRIDS_COUNT;
	g_data.m_cfg->SetConfig(CFG_MAIN_LAYOUT_GRIDS_COUNT, g_data.m_CfgData.m_dwLayoutGridsCnt, &dwValue);

	dwValue = 0;
	g_data.m_cfg->SetConfig(CFG_AREA_ID_NAME, g_data.m_CfgData.m_dwAreaNo, &dwValue);

	// 报警声音开关
	bValue = DEFAULT_ALARM_VOICE_SWITCH;
	g_data.m_cfg->SetBooleanConfig(CFG_ALARM_VOICE_SWITCH, g_data.m_CfgData.m_bAlarmVoiceOff, &bValue);

	// 自动保存excel
	bValue = FALSE;
	g_data.m_cfg->SetBooleanConfig(CFG_AUTO_SAVE_EXCEL, g_data.m_CfgData.m_bAutoSaveExcel, &bValue);

	// 十字锚
	bValue = FALSE;
	g_data.m_cfg->SetBooleanConfig(CFG_CROSS_ANCHOR, g_data.m_CfgData.m_bCrossAnchor, &bValue);

	// 手持最低显示温度
	dwValue = DEFAULT_MIN_TEMP_IN_SHOW;
	g_data.m_cfg->SetConfig(CFG_HAND_MIN_TEMP_IN_SHOW, g_data.m_CfgData.m_dwHandReaderMinTemp, &dwValue);
	// 手持最高显示温度
	dwValue = DEFAULT_MAX_TEMP_IN_SHOW;
	g_data.m_cfg->SetConfig(CFG_HAND_MAX_TEMP_IN_SHOW, g_data.m_CfgData.m_dwHandReaderMaxTemp, &dwValue);

	// 手持低温报警
	dwValue = DEFAULT_LOW_TEMP_ALARM;
	g_data.m_cfg->SetConfig(CFG_HAND_LOW_TEMP_ALARM, g_data.m_CfgData.m_dwHandReaderLowTempAlarm, &dwValue);
	// 手持高温报警
	dwValue = DEFAULT_HIGH_TEMP_ALARM;
	g_data.m_cfg->SetConfig(CFG_HAND_HIGH_TEMP_ALARM, g_data.m_CfgData.m_dwHandReaderHighTempAlarm, &dwValue);

	for (DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
		SaveGrid(i);
	}

	// 多余的grid配置删除
	for (DWORD i = g_data.m_CfgData.m_dwLayoutGridsCnt; i < MAX_GRID_COUNT; i++) {
		RemoveGridCfg(i);
	}

	// 格子数目改变
	if ( oldData.m_dwLayoutGridsCnt != g_data.m_CfgData.m_dwLayoutGridsCnt ) {
		g_data.m_cfg->RemoveConfig(CFG_GRIDS_ORDER);

		// 多余的格子的数据要删除
		for (DWORD i = oldData.m_dwLayoutGridsCnt; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
			m_pGrids[i]->ResetData();
		}

		m_dwCurSelGridIndex = 0;
		m_eGridStatus = GRID_STATUS_GRIDS;
		ResetGridOrder();
		RefreshGridsPage();
		RefreshGridsSize();

		CBusiness::GetInstance()->RestartLaunchAsyn();
	}
	// 格子数目不变
	else {
		// 行，列改变
		if (oldData.m_dwLayoutColumns != g_data.m_CfgData.m_dwLayoutColumns
			|| oldData.m_dwLayoutRows != g_data.m_CfgData.m_dwLayoutRows) {
			m_eGridStatus = GRID_STATUS_GRIDS;
			RefreshGridsPage();
			RefreshGridsSize();
		}		

		DWORD  dwDelay = 0;
		for ( DWORD i = 0; i < g_data.m_CfgData.m_dwLayoutGridsCnt; i++ ) {
			// 如果定时采集间隔改变
			if (oldData.m_GridCfg[i].m_dwCollectInterval != g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval) {
				CBusiness::GetInstance()->GetGridTemperatureAsyn(i, dwDelay);
				dwDelay += 200;
			}
		}
	}

	g_data.m_cfg->Save();
	::InvalidateRect(GetHWND(), 0, TRUE);
	delete pSettingDlg;


	// 多余的grid删除手持tag绑定
	for ( DWORD i = g_data.m_CfgData.m_dwLayoutGridsCnt; i < oldData.m_dwLayoutGridsCnt; i++ ) {
		CBusiness::GetInstance()->RemoveGridBindingAsyn(i+1);		
	}

	// 界面上删除绑定
	int nTagCnt = m_layTags->GetCount();
	for (int i = 0; i < nTagCnt; i++) {
		CTagUI * pTagUI = (CTagUI *)m_layTags->GetItemAt(i);
		if ( pTagUI->m_nBindingGridIndex > (int)g_data.m_CfgData.m_dwLayoutGridsCnt ) {
			pTagUI->SetBindingGridIndex(0);
			break;
		}
	}
}

// 保存格子配置
void  CDuiFrameWnd::SaveGrid(DWORD  i) {
	CDuiString  strText;
	BOOL  bValue = FALSE;
	DWORD  dwValue = 0;

	strText.Format("%s %lu", CFG_COLLECT_INTERVAL, i + 1);
	dwValue = 0;
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwCollectInterval, &dwValue);
	
	//strText.Format("%s %lu", CFG_GRID_MIN_TEMP, i + 1);
	//dwValue = DEFAULT_MIN_TEMP_IN_SHOW;
	//g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwMinTemp, &dwValue);

	//strText.Format("%s %lu", CFG_GRID_MAX_TEMP, i + 1);
	//dwValue = DEFAULT_MAX_TEMP_IN_SHOW;
	//g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_dwMaxTemp, &dwValue);

	for (DWORD j = 0; j < MAX_READERS_PER_GRID; j++) {
		strText.Format("%s %lu %lu", CFG_LOW_TEMP_ALARM, i + 1, j + 1);
		dwValue = DEFAULT_LOW_TEMP_ALARM;
		g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwLowTempAlarm, &dwValue);

		strText.Format("%s %lu %lu", CFG_HIGH_TEMP_ALARM, i + 1, j + 1);
		dwValue = DEFAULT_HIGH_TEMP_ALARM;
		g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_ReaderCfg[j].m_dwHighTempAlarm, &dwValue);
	}

	strText.Format("%s %lu", CFG_HAND_READER_LOW_TEMP_ALARM, i + 1);
	dwValue = DEFAULT_LOW_TEMP_ALARM;
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwLowTempAlarm, &dwValue);
	
	strText.Format("%s %lu", CFG_HAND_READER_HIGH_TEMP_ALARM, i + 1);
	dwValue = DEFAULT_HIGH_TEMP_ALARM;
	g_data.m_cfg->SetConfig(strText, g_data.m_CfgData.m_GridCfg[i].m_HandReaderCfg.m_dwHighTempAlarm, &dwValue);	
}

// 删除格子配置
void  CDuiFrameWnd::RemoveGridCfg(DWORD  i) {
	CDuiString  strText;

	strText.Format("%s %lu", CFG_COLLECT_INTERVAL, i + 1);
	g_data.m_cfg->RemoveConfig(strText);

	strText.Format("%s %lu", CFG_GRID_MIN_TEMP, i + 1);
	g_data.m_cfg->RemoveConfig(strText);

	strText.Format("%s %lu", CFG_GRID_MAX_TEMP, i + 1);
	g_data.m_cfg->RemoveConfig(strText);

	strText.Format("%s %lu", CFG_GRID_MODE, i + 1);
	g_data.m_cfg->RemoveConfig(strText);

	for (int j = 0; j < MAX_READERS_PER_GRID; j++) {
		strText.Format("%s %lu %lu", CFG_READER_SWITCH, i + 1, j + 1);
		g_data.m_cfg->RemoveConfig(strText);

		strText.Format("%s %lu %lu", CFG_LOW_TEMP_ALARM, i + 1, j + 1);
		g_data.m_cfg->RemoveConfig(strText);

		strText.Format("%s %lu %lu", CFG_HIGH_TEMP_ALARM, i + 1, j + 1);
		g_data.m_cfg->RemoveConfig(strText);
	}
}
 
// 点击了“关于”
void  CDuiFrameWnd::OnAbout() {   
	CAboutDlg * pAboutDlg = new CAboutDlg;

	pAboutDlg->Create(this->m_hWnd, _T("关于"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAboutDlg->CenterWindow();
	pAboutDlg->ShowModal();

	delete pAboutDlg;
}

// 硬件设备变动，可能有串口变动
void  CDuiFrameWnd::CheckDevice() {
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

	CBusiness::GetInstance()->CheckLaunchAsyn();
}

// 接收器状态
void   CDuiFrameWnd::OnLaunchStatus(WPARAM wParam, LPARAM  lParam) {
	CLmnSerialPort::PortStatus e = (CLmnSerialPort::PortStatus)wParam;
	if (e == CLmnSerialPort::OPEN) {
		m_lblLaunchStatus->SetText("发射器连接成功");
	}
	else {
		m_lblLaunchStatus->SetText("发射器连接断开");
	}
}

// 术中读卡器正在读数的提示
void    CDuiFrameWnd::OnTrySurReader(WPARAM wParam, LPARAM  lParam) {
	DWORD  i = (wParam - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (wParam - 1) % MAX_READERS_PER_GRID;
	BOOL   bStart = (BOOL)lParam;

	CDuiString  strText;
	if (bStart) {
		strText.Format("%lu%c读卡器正在测温...", i + 1, (char)(j + 'A'));
		m_lblSurReaderTry->SetText(strText);
	}
	else {
		m_lblSurReaderTry->SetText("");
	}
}

// 术中读卡器状态
void  CDuiFrameWnd::OnSurReaderStatus(WPARAM wParam, LPARAM  lParam) {
	DWORD  i = (wParam - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (wParam - 1) % MAX_READERS_PER_GRID;
	BOOL   bConnected = (BOOL)lParam;

	m_pGrids[i]->SetSurReaderStatus(j, bConnected);
}

// 术中读卡器温度
void   CDuiFrameWnd::OnSurReaderTemp(WPARAM wParam, LPARAM  lParam) {
	DWORD  i = (wParam - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (wParam - 1) % MAX_READERS_PER_GRID;
	TempItem * pItem = (TempItem *)lParam;
	m_pGrids[i]->OnSurReaderTemp(j, *pItem);
	delete pItem;
}

// 设置温度字体大小
void   CDuiFrameWnd::OnGridSizeChanged(const SIZE & s) {
	// 修改字体大小
	if (s.cx >= 950 && s.cy >= 425) {
		for (int i = 0; i < MAX_GRID_COUNT; i++) {
			m_pGrids[i]->SetFont(27);
		}
	}
	else if (s.cx >= 816 && s.cy >= 365) {
		for (int i = 0; i < MAX_GRID_COUNT; i++) {
			m_pGrids[i]->SetFont(26);
		}
	}
	else if (s.cx > 680 && s.cy > 304) {
		for (int i = 0; i < MAX_GRID_COUNT; i++) {
			m_pGrids[i]->SetFont(25);
		}
	}
	else if (s.cx > 572 && s.cy > 256) {
		for (int i = 0; i < MAX_GRID_COUNT; i++) {
			m_pGrids[i]->SetFont(24);
		}
	}
	else if (s.cx > 466 && s.cy > 208) {
		for (int i = 0; i < MAX_GRID_COUNT; i++) {
			m_pGrids[i]->SetFont(23);
		}
	}
	else if (s.cx > 350 && s.cy > 157) {
		for (int i = 0; i < MAX_GRID_COUNT; i++) {
			m_pGrids[i]->SetFont(22);
		}
	}
	else if (s.cx > 280 && s.cy > 125) {
		for (int i = 0; i < MAX_GRID_COUNT; i++) {
			m_pGrids[i]->SetFont(21);
		}
	}
	else {
		for (int i = 0; i < MAX_GRID_COUNT; i++) {
			m_pGrids[i]->SetFont(20);
		}
	}

	if (s.cy >= 295) {
		g_data.m_nVMargin = 40;
	}
	else if (s.cy >= 245) {
		g_data.m_nVMargin = 30;
	}
	else if (s.cy >= 195) {
		g_data.m_nVMargin = 20;
	}
	else{
		g_data.m_nVMargin = 10;
	}
}

//  1分钟定时器( 1. 定时更新逝去时间; 2. 删除一周前的温度数据 )
void   CDuiFrameWnd::On60SecondsTimer() {
	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		m_pGrids[i]->UpdateElapsed();
		m_pGrids[i]->PruneData();
	}

	m_cstHandImg->PruneData();

	CDuiString  strCurHandTag = m_cstHandImg->GetCurTagId();
	if ( strCurHandTag.GetLength() > 0 ) {
		std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find((const char *)strCurHandTag);
		// 如果该tag已经删除
		if ( it == m_tags_ui.end() ) {
			// 如果还有tag
			if ( m_layTags->GetCount() > 0 ) {
				CControlUI * pChildUI = m_layTags->GetItemAt(0);
				OnHandTagSelected(pChildUI);
			}
			// 如果没有tag
			else {
				m_cstHandImg->SetCurTag("");
			}
		}
	}

	/*********** 定时保存excel *************/
	time_t now = time(0);
	struct tm  tmp;
	localtime_s(&tmp, &now);

	// CBusiness::GetInstance()->SaveExcelAsyn();   用于调试用

	// 实际用法
	if ((tmp.tm_hour == 8 || tmp.tm_hour == 12 || tmp.tm_hour == 18 || tmp.tm_hour == 0)
		&& (tmp.tm_min >= 0 && tmp.tm_min <= 1)) {		
		// 两次间隔时间最少30分钟
		if (now - m_LastSaveExcelTime >= 1800) {
			// 保存
			CBusiness::GetInstance()->SaveExcelAsyn();
			m_LastSaveExcelTime = now;
		}
	}	
}

// 查询温度结果
void   CDuiFrameWnd::OnQueryTempRet(WPARAM wParam, LPARAM  lParam) {
	void ** pParam = (void **)wParam;

	char * pTagId = (char *)pParam[0];
	WORD   wBedNo = (WORD)(pParam[1]);
	std::vector<TempItem*> * pvRet = (std::vector<TempItem*> *)pParam[2];

	DWORD  i = (wBedNo - 1) / MAX_READERS_PER_GRID;
	DWORD  j = (wBedNo - 1) % MAX_READERS_PER_GRID;
	assert(i < MAX_GRID_COUNT);

	m_pGrids[i]->OnQueryTempRet( j, pTagId, *pvRet );

	delete[] pTagId;
	//ClearVector(*pvRet);
	delete pvRet;
	delete[] pParam;
}

// 右键弹出菜单
void   CDuiFrameWnd::OnImageMenu(DWORD  dwGridIndex, const POINT & pt, CControlUI * pParent ) {
	CDuiMenu *pMenu = new CDuiMenu(_T("menu_image.xml"), pParent, dwGridIndex, 0 );
	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE); 
}

// 导出Excel
void   CDuiFrameWnd::OnExportExcel(DWORD  dwIndex) {
	assert(dwIndex < MAX_GRID_COUNT);
	assert(dwIndex < g_data.m_CfgData.m_dwLayoutGridsCnt);
	m_pGrids[dwIndex]->ExportExcel();
}

// 打印Excel图表
void   CDuiFrameWnd::OnPrintExcel(DWORD  dwIndex) {
	assert(dwIndex < MAX_GRID_COUNT);
	assert(dwIndex < g_data.m_CfgData.m_dwLayoutGridsCnt);
	m_pGrids[dwIndex]->PrintExcel();
}
 
// 手持读卡器温度
void   CDuiFrameWnd::OnHandReaderTemp(WPARAM wParam, LPARAM  lParam) {
	TempItem * pItem            = (TempItem*)wParam;
	char *     tag_patient_name = (char *)lParam;
	assert(pItem);
	assert(tag_patient_name);

	BOOL  bNewTag = FALSE;
	m_cstHandImg->OnHandTemp(pItem, bNewTag);

	CTagUI * pTagUI = 0;

	assert(pItem->m_szTagId[0] != '\0');
	// 新Tag
	if ( bNewTag ) {
		pTagUI = new CTagUI;  
		m_layTags->AddAt(pTagUI,0); 
		pTagUI->SetFixedHeight(TAG_UI_HEIGHT);
		pTagUI->OnHandTemp(pItem, tag_patient_name); 

		m_tags_ui.insert( std::make_pair(pItem->m_szTagId, pTagUI) );
	}
	else {		
		pTagUI = m_tags_ui[pItem->m_szTagId];
		assert(pTagUI);
		pTagUI->OnHandTemp(pItem, tag_patient_name);

		// 是否时间排序
		HandTagSortType eSortType = GetHandTagSortType();
		if ( eSortType == HandTagSortType_Time ) {
			CTagUI * pItem = (CTagUI *) m_layTags->GetItemAt(0);
			// 如果不是第一位
			if (pItem != pTagUI) {
				m_layTags->Remove(pTagUI, true);
				m_layTags->AddAt(pTagUI, 0);
			}			
		}
	}

	// 如果绑定了床位号
	int nBindingGridIndex = pTagUI->GetBindingGridIndex();
	if ( nBindingGridIndex > 0 ) {
		assert(nBindingGridIndex <= MAX_GRID_COUNT );
		m_pGrids[nBindingGridIndex - 1]->OnHandReaderTemp(*pItem);
	}

	// 已经保存在m_cstHandImg对象内
	// delete pItem;
	delete[] tag_patient_name;
}

// 数据库的相关数据查询完毕
void   CDuiFrameWnd::OnPrepared(WPARAM wParam, LPARAM  lParam) {
	CheckDevice();
}

// 获得当前的手持Tag排序
CDuiFrameWnd::HandTagSortType CDuiFrameWnd::GetHandTagSortType() {
	if (m_optDefaultSort->IsSelected())
		return HandTagSortType_Default;
	else
		return HandTagSortType_Time;
}

// 获得所有手持Tag温度数据
void   CDuiFrameWnd::OnAllHandTagTempData(WPARAM wParam, LPARAM  lParam) {
	std::vector<HandTagResult *> * pvRet = (std::vector<HandTagResult *> *)wParam;
	std::vector<HandTagResult *>::iterator it;

	assert( m_layTags->GetCount() == 0 );
	assert(m_tags_ui.size() == 0);

	for (it = pvRet->begin(); it != pvRet->end(); ++it) {
		HandTagResult * pItem = *it;
		// 没有数据
		if (0 == pItem->m_pVec || pItem->m_pVec->size() == 0)
			continue;

		// 保存数据
		m_cstHandImg->OnHandTempVec(pItem->m_pVec, pItem->m_szTagId);

		CTagUI * pTagUI = new CTagUI;
		m_layTags->Add(pTagUI);
		pTagUI->SetFixedHeight(TAG_UI_HEIGHT);

		TempItem * pSubItem = pItem->m_pVec->at(pItem->m_pVec->size() - 1);
		pTagUI->OnHandTemp(pSubItem, pItem->m_szTagPName);
		pTagUI->SetBindingGridIndex(pItem->m_nBindingGridIndex);

		// 如果绑定了床位号
		if (pItem->m_nBindingGridIndex > 0) {
			assert(pItem->m_nBindingGridIndex <= MAX_GRID_COUNT);
			CBusiness::GetInstance()->QueryTempByHandTagAsyn(pItem->m_szTagId, pItem->m_nBindingGridIndex);
			m_pGrids[pItem->m_nBindingGridIndex - 1]->SetPatientNameInHandMode(pItem->m_szTagPName);
		}

		assert(pItem->m_szTagId[0] != '\0');
		m_tags_ui.insert(std::make_pair(pItem->m_szTagId, pTagUI));
	}

	if ( m_layTags->GetCount() > 0 ) {
		OnHandTagSelected(m_layTags->GetItemAt(0));
	}


	/**** 回收内存 ****/
	for ( it = pvRet->begin(); it != pvRet->end(); ++it) {
		HandTagResult * pItem = *it;
		if ( 0 == pItem->m_pVec )
			continue;
		// 已经保存在m_cstHandImg
		//ClearVector(*pItem->m_pVec);
		//delete pItem->m_pVec;
	}
	ClearVector(*pvRet);
	delete pvRet;
}

// 选中了手持Tag
void  CDuiFrameWnd::OnHandTagSelected(CControlUI * pTagUI) {	
	int nCnt = m_layTags->GetCount();
	for ( int i = 0; i < nCnt; i++ ) {
		CTagUI* pUI = (CTagUI *)m_layTags->GetItemAt(i);
		if ( pUI == pTagUI ) {
			pUI->SetBkColor(0xFF555555);
			m_cstHandImg->SetCurTag(pUI->GetTagId());
		}
		else {
			pUI->SetBkColor(0);
		}
	}	
}

// 点击了按时间排序
void  CDuiFrameWnd::OnHandTagTimeOrder() {
	int nCnt = m_layTags->GetCount();
	for ( int i = 0; i < nCnt; i++ ) {
		m_layTags->RemoveAt(0, true);
	}

	std::map<std::string, CTagUI *>::iterator it;
	for ( it = m_tags_ui.begin(); it != m_tags_ui.end(); ++it ) {
		CTagUI * pTagUI = it->second;
		assert(pTagUI);
		OnHandTagTimeOrder(pTagUI);
	}
}

void   CDuiFrameWnd::OnHandTagTimeOrder(CTagUI * pTagUI) {
	int nCnt = m_layTags->GetCount();

	int i = 0;
	for ( i = 0; i < nCnt; i++ ) {
		CTagUI * pItem = (CTagUI *)m_layTags->GetItemAt(i);
		if ( pTagUI->m_item.m_time > pItem->m_item.m_time ) {
			break;
		}
	}

	m_layTags->AddAt(pTagUI, i);
}

// 移动Tag UI，绑定窗格
void   CDuiFrameWnd::MoveTagUI(const POINT & pt) {
	if (!m_dragdropGrid->IsVisible()) {
		m_dragdropGrid->SetVisible(true);
		m_hand_tabs->SelectItem(1);

		ResetDragdropGrids();		
	}

	int x = 100 / 2;
	int y = 100 / 2;

	RECT r;
	r.left = pt.x - x;
	r.right = r.left + x * 2;
	r.top = pt.y - y;
	r.bottom = r.top + y * 2;
	m_dragdropGrid->SetPos(r);

	// TagUI 高亮经过的格子
	OnMoveTagUI(pt);
}

// 停止移动Tag UI，确定最终位置
void   CDuiFrameWnd::StopMoveTagUI() {
	if ( m_dragdrop_tag_dest_index >= 0 ) {
		std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find((const char *)m_dragdrop_tag);
		if ( it != m_tags_ui.end() ) {
			CTagUI * pTag = it->second;
			assert( 0 == strcmp( pTag->m_item.m_szTagId, m_dragdrop_tag) );
			// 如果绑定的grid index改变
			if ( pTag->GetBindingGridIndex() != m_dragdrop_tag_dest_index + 1) {
				// 等到答复后，再设置界面
				CBusiness::GetInstance()->TagBindingGridAsyn(m_dragdrop_tag, m_dragdrop_tag_dest_index + 1);
			}			
		}
	}

	m_dragdrop_tag = "";
	m_dragdrop_tag_dest_index = -1;
	m_hand_tabs->SelectItem(0);
	m_dragdropGrid->SetVisible(false);
}

// Tag UI移动过程中，经过的格子要高亮
void   CDuiFrameWnd::OnMoveTagUI(const POINT & pt) {
	RECT rect   = m_hand_tabs->GetPos();
	int  width  = rect.right - rect.left;
	int  height = rect.bottom - rect.top;

	int  nColumns = 0;
	int  nRows = 0;
	GetDragDropGridsParams(nColumns, nRows);
	assert(nColumns > 0 && nRows > 0);

	SIZE s = m_layDragDropGrids->GetItemSize();

	if (pt.x <= rect.left || pt.x >= rect.right || pt.y <= rect.top || pt.y >= rect.bottom) {
		for ( int i = 0; i < (int)g_data.m_CfgData.m_dwLayoutGridsCnt; i++ ) {
			CLabelUI* pChild = (CLabelUI*)m_layDragDropGrids->GetItemAt(i);
			pChild->SetBorderColor(0xFFFFFFFF);
			pChild->SetTextColor(0xFFFFFFFF);
			pChild->SetBkColor(0xFF192431);
		}
		m_dragdrop_tag_dest_index = -1;
	}

	m_dragdrop_tag_dest_index = -1;
	for (int i = 0; i < nColumns; i++) {
		for (int j = 0; j < nRows; j++) {

			int nIndex = j * nColumns + i;
			// 在总Grids范围内
			if ( nIndex < (int)g_data.m_CfgData.m_dwLayoutGridsCnt ) {
				CLabelUI* pChild = (CLabelUI*)m_layDragDropGrids->GetItemAt(nIndex);
				// 在矩形范围内
				if ((pt.x > rect.left + s.cx * i) && (pt.x < rect.left + s.cx * (i + 1))
					&& (pt.y > rect.top + s.cy * j) && (pt.y < rect.top + s.cy * (j + 1))) {
					pChild->SetBorderColor(0xFFCAF100);
					pChild->SetTextColor(0xFF000000);
					pChild->SetBkColor(0xFFCCCCCC);
					m_dragdrop_tag_dest_index = nIndex;
				}
				else {
					pChild->SetBorderColor(0xFFFFFFFF);
					pChild->SetTextColor(0xFFFFFFFF);
					pChild->SetBkColor(0xFF192431);
				}
			}

		}
	}
}

// 重新设置LayoutGrids视图
void   CDuiFrameWnd::ResetDragdropGrids() {
	assert(m_layDragDropGrids);

	RECT rect   = m_hand_tabs->GetPos();
	int  width  = rect.right - rect.left;
	int  height = rect.bottom - rect.top;

	m_layDragDropGrids->RemoveAll();

	int  nColumns = 0;
	int  nRows = 0;
	GetDragDropGridsParams(nColumns, nRows);
	assert(nColumns > 0 && nRows > 0);
	m_layDragDropGrids->SetFixedColumns(nColumns);

	SIZE s;
	s.cx = (width - 1) / nColumns + 1;
	s.cy = (height - 1) / nRows + 1;
	m_layDragDropGrids->SetItemSize(s);

	CDuiString  strText;
	for (int i = 0; i < (int)g_data.m_CfgData.m_dwLayoutGridsCnt; i++) {
		CLabelUI * pLabel = new CLabelUI;
		strText.Format("%d", i + 1);
		pLabel->SetText(strText);
		pLabel->SetTextColor(0xFFFFFFFF);
		pLabel->SetBorderColor(0xFFFFFFFF);
		pLabel->SetBorderSize(1);
		pLabel->SetAttribute("align", "center");
		pLabel->SetFont(GetFontBySize(s));
		m_layDragDropGrids->Add(pLabel);
	}
}

// 获取DragdropGrids视图的行，列
void  CDuiFrameWnd::GetDragDropGridsParams(int & nCol, int & nRow) {
	for ( int i = 0; i < 8; i++ ) {
		if ( (i + 1) * (i + 1) >= (int)g_data.m_CfgData.m_dwLayoutGridsCnt ) {
			nCol = nRow = i + 1;
			return;
		}
	}
	nCol = nRow = 8;
}

// 根据Rect大小获取Label字体大小
int  CDuiFrameWnd::GetFontBySize(const SIZE & s) {
	
	if (s.cx >= 950 && s.cy >= 425) {
		return 27;
	}
	else if (s.cx >= 816 && s.cy >= 365) {
		return 26;
	}
	else if (s.cx > 680 && s.cy > 304) {
		return 25;
	}
	else if (s.cx > 572 && s.cy > 256) {
		return 24;
	}
	else if (s.cx > 466 && s.cy > 208) {
		return 23;
	}
	else if (s.cx > 350 && s.cy > 157) {
		return 22;
	}
	else if (s.cx > 280 && s.cy > 125) {
		return 21;
	}
	else {
		return 20;
	}
}

// tag 绑定 grid index结果
void   CDuiFrameWnd::OnTagBindingGridRet(WPARAM wParam, LPARAM  lParam) {
	TagBindingGridRet * pParam = (TagBindingGridRet*)wParam;

	std::map<std::string, CTagUI *>::iterator it;
	it = m_tags_ui.find(pParam->m_szTagId);
	if ( it != m_tags_ui.end() ) {
		CTagUI * pTagUI = it->second;
		if (pTagUI) {
			pTagUI->SetBindingGridIndex(pParam->m_nGridIndex);
			assert(pParam->m_nGridIndex > 0);
			CDuiString  strPName = pTagUI->GetPTagName();
			m_pGrids[pParam->m_nGridIndex - 1]->SetPatientNameInHandMode(strPName);
		}			
	}

	it = m_tags_ui.find(pParam->m_szOldTagId);
	if (it != m_tags_ui.end()) {
		CTagUI * pTagUI = it->second;
		if ( pTagUI )
			pTagUI->SetBindingGridIndex(0);
	}

	CBusiness::GetInstance()->QueryTempByHandTagAsyn(pParam->m_szTagId, pParam->m_nGridIndex);	

	delete pParam;
}

// 查询温度结果(手持Tag温度数据)
void   CDuiFrameWnd::OnQueryHandTempRet(WPARAM wParam, LPARAM  lParam) {
	void ** pParam = (void **)wParam;

	char * pTagId     = (char *)pParam[0];
	int    nGridIndex = (int)(pParam[1]);
	std::vector<TempItem*> * pvRet = (std::vector<TempItem*> *)pParam[2];

	assert(nGridIndex <= MAX_GRID_COUNT && nGridIndex >= 1 );

	m_pGrids[nGridIndex-1]->OnQueryHandTempRet(pTagId, *pvRet);

	delete[] pTagId;
	// ClearVector(*pvRet);
	delete pvRet;
	delete[] pParam;
}

// tag name changed
void   CDuiFrameWnd::OnTagNameChanged( WPARAM wParam, LPARAM  lParam ) {
	CTagUI * pTagUI = (CTagUI *)wParam;
	assert(pTagUI);
	assert(pTagUI->m_nBindingGridIndex > 0);

	CDuiString  strName = pTagUI->GetPTagName();
	m_pGrids[pTagUI->m_nBindingGridIndex-1]->SetPatientNameInHandMode(strName);
}

// 右键弹出菜单
void   CDuiFrameWnd::OnHandImageMenu(const POINT & pt, CControlUI * pParent) {
	CDuiMenu *pMenu = new CDuiMenu(_T("menu_hand_image.xml"), pParent);
	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

// 导出Excel
void   CDuiFrameWnd::OnHanxExportExcel() {
	CDuiString strTagId = m_cstHandImg->GetCurTagId();
	std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find( (const char *)strTagId );
	if ( it != m_tags_ui.end() ) {
		CTagUI * pTagUI = it->second;
		if ( pTagUI ) {
			CDuiString strPName = pTagUI->GetPTagName();
			m_cstHandImg->ExportExcel(strPName);
		}		
	}	
}

// 打印Excel图表
void   CDuiFrameWnd::OnHandPrintExcel() {
	CDuiString strTagId = m_cstHandImg->GetCurTagId();
	std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find((const char *)strTagId);
	if (it != m_tags_ui.end()) {
		CTagUI * pTagUI = it->second;
		if (pTagUI) {
			CDuiString strPName = pTagUI->GetPTagName();
			m_cstHandImg->PrintExcel(strPName);
		}
	}
}


// 接收器连接状态通知
void   CDuiFrameWnd::OnLauchStatusNotify(CLmnSerialPort::PortStatus e) {
	::PostMessage( GetHWND(), UM_LAUNCH_STATUS, (WPARAM)e, 0);
}      

// 接收器尝试读取术中读卡器的温度数据
void   CDuiFrameWnd::OnTrySurReaderNotify(WORD wBed, BOOL bStart) {
	::PostMessage(GetHWND(), UM_TRY_SUR_READER, (WPARAM)wBed, bStart);
}

// 接收到术中读卡器连接状态的通知
void   CDuiFrameWnd::OnSurReaderStatusNotify(WORD wBed, BOOL bConnected) {
	::PostMessage(GetHWND(), UM_SUR_READER_STATUS, (WPARAM)wBed, bConnected);
}

// 术中读卡器的温度数据
void   CDuiFrameWnd::OnSurReaderTempNotify(WORD wBed, const TempItem & item) {
	TempItem * pItem = new TempItem;
	memcpy(pItem, &item, sizeof(TempItem));
	::PostMessage(GetHWND(), UM_SUR_READER_TEMP, (WPARAM)wBed, (LPARAM)pItem );
}

// 查询到温度结果(根据TagId)
void   CDuiFrameWnd::OnQueryTempRetNotify(const char * szTagId, WORD wBed, std::vector<TempItem*> * pvRet) {
	void ** pParam = new void *[3];

	char * pTagId = new char[MAX_TAG_ID_LENGTH];
	STRNCPY(pTagId, szTagId, MAX_TAG_ID_LENGTH);

	pParam[0] = (void *)pTagId;
	pParam[1] = (void *)wBed;
	pParam[2] = (void *)pvRet;

	::PostMessage(GetHWND(), UM_QUERY_TEMP_BY_TAG_ID_RET, (WPARAM)pParam, 0);
}

// 手持读卡器的温度数据
void   CDuiFrameWnd::OnHandReaderTempNotify(const TempItem & item, const char * tag_patient_name) {
	TempItem * pItem = new TempItem;
	memcpy(pItem, &item, sizeof(TempItem));

	assert(tag_patient_name);
	char * new_tag_patient_name = new char[MAX_TAG_PNAME_LENGTH];
	STRNCPY(new_tag_patient_name, tag_patient_name, MAX_TAG_PNAME_LENGTH);

	::PostMessage(GetHWND(), UM_HAND_READER_TEMP, (WPARAM)pItem, (LPARAM)new_tag_patient_name);
}

// 数据库需要的数据已经查询完毕
void  CDuiFrameWnd::OnPreparedNotify() {
	::PostMessage(GetHWND(), UM_PREPARED, 0, 0);
}

// 查询到的所有手持Tag温度数据
void   CDuiFrameWnd::OnAllHandTagTempDataNotify(std::vector<HandTagResult *> * pvRet) {
	::PostMessage(GetHWND(), UM_ALL_HAND_TAG_TEMP_DATA, (WPARAM)pvRet, 0);
}

// 删除掉过时的手持Tag
void   CDuiFrameWnd::OnHandTagErasedNotify(const char * szTagId) {
	assert(szTagId);

	std::map<std::string, CTagUI *>::iterator it = m_tags_ui.find(szTagId);
	// 如果找到了
	if (it != m_tags_ui.end()) {
		CTagUI * pTagUI = it->second;
		m_tags_ui.erase(it);

		if (pTagUI) {
			m_layTags->Remove(pTagUI);
		}
	}
}

// 绑定结果的通知
void   CDuiFrameWnd::OnBindingRetNotify(const TagBindingGridRet & item) {
	TagBindingGridRet * pParam = new TagBindingGridRet;
	memcpy( pParam, &item, sizeof(TagBindingGridRet) );
	::PostMessage( GetHWND(), UM_BINDING_TAG_GRID_RET, (WPARAM)pParam, 0 );
}

// 查询手持Tag结果通知
void   CDuiFrameWnd::OnQueryHandTagRetNotify(const char * szTagId, int nGridIndex, std::vector<TempItem*> * pvRet) {
	void ** pParam = new void *[3];

	char * pTagId = new char[MAX_TAG_ID_LENGTH];
	STRNCPY(pTagId, szTagId, MAX_TAG_ID_LENGTH);

	pParam[0] = (void *)pTagId;
	pParam[1] = (void *)nGridIndex;
	pParam[2] = (void *)pvRet;

	::PostMessage(GetHWND(), UM_QUERY_HAND_TEMP_BY_TAG_ID_RET, (WPARAM)pParam, 0);
}
                      

 
                 
void PrintStatus(int nCnt, void * args[]) {
	CBusiness::GetInstance()->PrintStatusAsyn();
}
             
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	SetUnhandledExceptionFilter(pfnUnhandledExceptionFilter);

	HANDLE handle = ::CreateMutex(NULL, FALSE, GLOBAL_LOCK_NAME);//handle为声明的HANDLE类型的全局变量 
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		::MessageBox(0, "程序已经打开或没有关闭完全，请先关闭或等待完全关闭!", "错误", 0);
		return 0;
	}

	CoInitialize(NULL);
	JTelSvrRegCommand("status", "print status", PrintStatus, 0);
	DWORD  dwPort = 2019;
	JTelSvrStart((unsigned short)dwPort, 10);


	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

	CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
	duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame->CenterWindow();

	CBusiness::GetInstance()->InitSigslot(duiFrame);


	// 设置icon
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	duiFrame->ShowModal();
	delete duiFrame;

	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");
	g_data.m_bClosing = TRUE;

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();
	CoUninitialize();

	return 0;
}
            

