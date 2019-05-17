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
	}

	g_data.m_DragDropCtl = m_PaintManager.FindControl(DRAG_DROP_CTL); 
	g_data.m_edRemark    = static_cast<CEditUI *>( m_PaintManager.FindControl(EDIT_REMARK) );
	/*************  end 获取控件 *****************/

	RefreshGridsPage();

	CheckDevice();

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
	}
	else if (msg.sType == "menu") {
		if (name == CST_IMG_LBL_TEMP) { 
			OnImageMenu(msg.pSender->GetTag(), msg.ptMouse, msg.pSender);
		}
		else if (name == CST_IMAGE) {
			OnImageMenu(msg.pSender->GetTag(), msg.ptMouse, msg.pSender);
		}
	}
	else if ( msg.sType == "menu_export_excel" ) {
		OnExportExcel(msg.wParam);
	}
	else if ( msg.sType == "menu_print_excel" ) {
		OnPrintExcel(msg.wParam);
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
	
	return WindowImplBase::OnLButtonDown(uMsg, wParam, lParam, bHandled);
}

LRESULT  CDuiFrameWnd::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	if (m_nDgSourceIndex >= 0) {
		StopMoveGrid();
	}
	return WindowImplBase::OnLButtonUp(uMsg, wParam, lParam, bHandled);
}

LRESULT  CDuiFrameWnd::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	if (m_nDgSourceIndex >= 0) {
		DWORD dwCurTick = LmnGetTickCount();
		if (dwCurTick - m_dwDgStartTick < DRAG_DROP_DELAY_TIME) {
			return WindowImplBase::OnMouseMove(uMsg, wParam, lParam, bHandled);
		}

		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		MoveGrid(pt);
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
		OnSetTempFont(s);
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
void   CDuiFrameWnd::OnSetTempFont(const SIZE & s) {
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
}

//  1分钟定时器( 1. 定时更新逝去时间; 2. 删除一周前的温度数据 )
void   CDuiFrameWnd::On60SecondsTimer() {
	for (int i = 0; i < MAX_GRID_COUNT; i++) {
		m_pGrids[i]->UpdateElapsed();
		m_pGrids[i]->PruneData();
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
}

// 打印Excel图表
void   CDuiFrameWnd::OnPrintExcel(DWORD  dwIndex) {
	assert(dwIndex < MAX_GRID_COUNT);
	assert(dwIndex < g_data.m_CfgData.m_dwLayoutGridsCnt);
}

// 手持读卡器温度
void   CDuiFrameWnd::OnHandReaderTemp(WPARAM wParam, LPARAM  lParam) {
	TempItem * pItem = (TempItem*)wParam;
	delete pItem;
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
void   CDuiFrameWnd::OnHandReaderTempNotify(const TempItem & item) {
	TempItem * pItem = new TempItem;
	memcpy(pItem, &item, sizeof(TempItem));
	::PostMessage(GetHWND(), UM_HAND_READER_TEMP, (WPARAM)pItem, 0);
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
            

