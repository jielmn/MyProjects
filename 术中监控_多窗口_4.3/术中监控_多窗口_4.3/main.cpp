// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"

CDuiFrameWnd::CDuiFrameWnd() : m_callback(&m_PaintManager) {
	m_tabs = 0;
	m_layMain = 0;
	memset(m_pGrids, 0, sizeof(m_pGrids));
	m_dwCurGridIndex = 0;
	m_eGridStatus = GRID_STATUS_GRIDS;
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

	/*************  获取控件 *****************/
	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(TABS_ID));
	m_layMain = static_cast<DuiLib::CTileLayoutUI*>(m_PaintManager.FindControl(LAYOUT_MAIN_NAME));
	m_layPages = static_cast<DuiLib::CHorizontalLayoutUI *>(m_PaintManager.FindControl(LAYOUT_PAGES));
	m_btnPrevPage = static_cast<DuiLib::CButtonUI *>(m_PaintManager.FindControl(BUTTON_PREV_PAGE));
	m_btnNextPage = static_cast<DuiLib::CButtonUI *>(m_PaintManager.FindControl(BUTTON_NEXT_PAGE));

	// 添加窗格
	for ( DWORD i = 0; i < MAX_GRID_COUNT; i++ ) {
		m_pGrids[i] = new CGridUI;
		m_pGrids[i]->SetBorderSize(GRID_BORDER_SIZE);
		m_pGrids[i]->SetBorderColor(GRID_BORDER_COLOR);
		m_pGrids[i]->SetName(GRID_NAME);
		m_pGrids[i]->SetTag(i);

		m_pGrids[i]->SetBedNo(i + 1);
	}
	/*************  end 获取控件 *****************/

	RefreshGridsPage();

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
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
		DWORD   dwGridPageIndex = m_dwCurGridIndex / dwCntPerPage;
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

	}
	
}

// 更新grids size
void   CDuiFrameWnd::RefreshGridsSize(int width, int height) {
	if (0 == m_layMain)
		return;

	// 如果需要多页显示
	if ( g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows 
		< g_data.m_CfgData.m_dwLayoutGridsCnt ) {
		height -= 30;
	}

	SIZE s;
	CContainerUI * pParent = (CContainerUI *)m_layMain->GetParent();

	if ( GRID_STATUS_GRIDS == m_eGridStatus ) {

		s.cx = ( width - 1 ) / (int)g_data.m_CfgData.m_dwLayoutColumns + 1;
		s.cy = ( height - 1 ) / (int)g_data.m_CfgData.m_dwLayoutRows + 1;
		int margin_h = (width - s.cx * (int)g_data.m_CfgData.m_dwLayoutColumns) / 2;
		int margin_v = (height - s.cy * (int)g_data.m_CfgData.m_dwLayoutRows) / 2;

		RECT  inset;
		inset.left = inset.right  = margin_h;
		inset.top  = inset.bottom = margin_v;
		//pParent->SetInset(inset);
		m_layMain->SetItemSize(s);
	}
}

void   CDuiFrameWnd::NextPage() {
	// 每页多少grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// 当前页index
	DWORD   dwGridPageIndex = m_dwCurGridIndex / dwCntPerPage;
	// 当前页的第一个grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;
	// 下一页的第一grid index，并修改当前的grid index
	m_dwCurGridIndex = dwFirstGridIndexCurPage + dwCntPerPage;

	RefreshGridsPage();
}

void   CDuiFrameWnd::PrevPage() {
	// 每页多少grids
	DWORD   dwCntPerPage = g_data.m_CfgData.m_dwLayoutColumns * g_data.m_CfgData.m_dwLayoutRows;
	// 当前页index
	DWORD   dwGridPageIndex = m_dwCurGridIndex / dwCntPerPage;
	// 当前页的第一个grid index
	DWORD   dwFirstGridIndexCurPage = dwGridPageIndex * dwCntPerPage;

	// 上一页的第一grid index，并修改当前的grid index
	assert(dwFirstGridIndexCurPage >= dwCntPerPage);
	m_dwCurGridIndex = dwFirstGridIndexCurPage - dwCntPerPage;

	RefreshGridsPage();
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

	CBusiness::GetInstance()->InitSigslot(duiFrame);


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


