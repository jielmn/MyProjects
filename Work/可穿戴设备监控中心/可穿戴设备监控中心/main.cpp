// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "SettingDlg.h"
#include "DuiMenu.h"
#include "MyControls.h"
#include "AboutDlg.h"

#define   GRID_WIDTH      160
#define   GRID_HEIGHT     180                                  

CDuiFrameWnd::CDuiFrameWnd() {
	m_layGrids = 0;
	m_layList = 0;
	m_btnExpand = 0;
	m_btnMenu = 0;
	m_layGridsPages = 0;
	m_tabs = 0;
	m_lstItems = 0;
	m_edName = 0;
	m_nEditingNameIndex = -1;
	m_nCurPageFirstItemIndex = 0;
	m_btnPrev = 0;
	m_btnNext = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {
	ClearVector(m_data);
}
  
void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = m_hWnd;
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	m_layGrids = (DuiLib::CTileLayoutUI *)m_PaintManager.FindControl("layGrids");
	m_layList = (DuiLib::CVerticalLayoutUI *)m_PaintManager.FindControl("layList");
	m_btnExpand = (DuiLib::CButtonUI *)m_PaintManager.FindControl("btnExpand");
	m_btnMenu = (DuiLib::CButtonUI *)m_PaintManager.FindControl("menubtn");
	m_layGridsPages = (DuiLib::CHorizontalLayoutUI *)m_PaintManager.FindControl("layGridsPages");
	m_tabs = (DuiLib::CTabLayoutUI *)m_PaintManager.FindControl("switch");
	m_lstItems = (DuiLib::CListUI *)m_PaintManager.FindControl("lstItems");
	m_edName = (DuiLib::CEditUI *)m_PaintManager.FindControl("edName");
	m_btnPrev = (DuiLib::CButtonUI *)m_PaintManager.FindControl("btnPrevPage");
	m_btnNext = (DuiLib::CButtonUI *)m_PaintManager.FindControl("btnNextPage");

	m_layList->SetVisible(false);
	m_btnExpand->SetText("<");   
	m_btnExpand->SetTag(FALSE); 

	for (int i = 0; i < 120; i++) {
		CWearItem * p = new CWearItem;
		SNPRINTF( p->m_szDeviceId, sizeof(p->m_szDeviceId), "%d", i + 1);
		SNPRINTF( p->m_szName, sizeof(p->m_szName), "%d", i + 1);

		DataItem * q = 0;

		q = new DataItem;
		q->nData = i + 1;
		q->tTime = 0;
		p->m_vHearbeat.push_back(q);

		q = new DataItem;
		q->nData = i + 100;
		q->tTime = 0;
		p->m_vOxy.push_back(q);

		q = new DataItem;
		q->nData = i + 1000;
		q->tTime = 0;
		p->m_vTemp.push_back(q);

		m_data.push_back(p);
	}

	m_layGrids->OnSize += MakeDelegate(this, &CDuiFrameWnd::OnGridsLayoutSize);

	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	if (msg.sType == "click") {
		if (msg.pSender->GetName() == "btnExpand") {
			OnExpand();
		}
		else if (msg.pSender->GetName() == "menubtn") {
			OnMainMenu(msg);
		}
	}
	else if (msg.sType == "setting") {
		OnSetting();
	}
	else if (msg.sType == "historydata") {
		OnCheckHistory();
	}
	else if (msg.sType == "warningprepose") {
		OnWarningPrepose();		
	}
	else if (msg.sType == "recycle") {
		OnRecycle();
	}
	else if (msg.sType == "about") {
		OnAbout();
	}
	else if (msg.sType == "killfocus" ) {
		if (msg.pSender == m_edName) {
			OnEdNameKillFocus();
		}
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
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void CDuiFrameWnd::OnExpand() {
	BOOL  bExpanded = (BOOL)m_btnExpand->GetTag();
	if (bExpanded) {
		m_btnExpand->SetText("<");
		m_layList->SetVisible(false);
	}
	else {
		m_btnExpand->SetText(">");
		m_layList->SetVisible(true);
	}
	m_btnExpand->SetTag(!bExpanded);
}

void  CDuiFrameWnd::OnMainMenu(TNotifyUI& msg) {
	RECT r = m_btnMenu->GetPos();
	POINT pt = { r.left, r.bottom };
	CDuiMenu *pMenu = new CDuiMenu(_T("menu.xml"), msg.pSender);
	pMenu->Init(*this, pt, "checked.png");
	if (g_data.m_bShowHistory)
		pMenu->SetCheckedItem("historycheck");    
	if (g_data.m_bWarningPrepose)
		pMenu->SetCheckedItem("chwarningprepose");
	if (g_data.m_bRecycle)
		pMenu->SetCheckedItem("chrecycle");
	pMenu->ShowWindow(TRUE);           
}

void  CDuiFrameWnd::OnSetting() {
	CSettingDlg * pSettingDlg = new CSettingDlg;
	pSettingDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pSettingDlg->CenterWindow();
	int ret = pSettingDlg->ShowModal();
	if (0 == ret) {

	}
	delete pSettingDlg;
}

bool CDuiFrameWnd::OnGridsLayoutSize(void * pParam) {
	// 设置每个格子大小
	RECT r = m_layGrids->GetPos();
	int width = r.right - r.left;
	int height = r.bottom - r.top;

	int nColumns = width / GRID_WIDTH;
	if (nColumns <= 0)
		nColumns = 1;

	int nRows = height / GRID_HEIGHT;
	if (nRows <= 0)
		nRows = 1;

	m_layGrids->SetFixedColumns(nColumns); 

	SIZE  s;
	s.cx = width / nColumns;
	s.cy = height / nRows;

	m_layGrids->SetItemSize(s);

	// 创建格子满足显示
	m_layGrids->RemoveAll();
	m_lstItems->RemoveAll();
	
	// 如果没有数据
	if (m_data.size() == 0) {
		m_nCurPageFirstItemIndex = 0;
		return true;
	}

	// 每页多少格子
	int nItemsCntPerPage = nRows * nColumns;
	// 总页数
	int nPagesCnt = ( m_data.size() - 1 ) / nItemsCntPerPage + 1;
	// 计算当前页
	int nCurPage = m_nCurPageFirstItemIndex / nItemsCntPerPage;
	// 重新计算 m_nCurPageFirstItemIndex
	m_nCurPageFirstItemIndex = nCurPage * nPagesCnt;

	int nMaxItemIndex = (int)m_data.size() < (nCurPage + 1) * nItemsCntPerPage 
		? m_data.size() : (nCurPage + 1) * nItemsCntPerPage;

	for ( int i = m_nCurPageFirstItemIndex; i < nMaxItemIndex; i++ ) {
		CGridUI * pGrid = new CGridUI;
		m_layGrids->Add(pGrid);

		CListTextElementUI * pListItem = new CListTextElementUI;
		m_lstItems->Add(pListItem);
	}

	if (nCurPage > 0) {
		m_btnPrev->SetEnabled(true);
	}

	if ( nCurPage < nPagesCnt - 1 ) {
		m_btnNext->SetEnabled(true);
	}

	FillData();
	return true;          
}

void  CDuiFrameWnd::FillData() {

}

void  CDuiFrameWnd::OnCheckHistory() {
	g_data.m_bShowHistory = !g_data.m_bShowHistory;
}

void  CDuiFrameWnd::OnWarningPrepose() {
	g_data.m_bWarningPrepose = !g_data.m_bWarningPrepose;
}

void  CDuiFrameWnd::OnRecycle() {
	g_data.m_bRecycle = !g_data.m_bRecycle;
}

void  CDuiFrameWnd::OnAbout() {
	CAboutDlg * pAboutDlg = new CAboutDlg;

	pAboutDlg->Create(this->m_hWnd, _T("关于"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pAboutDlg->CenterWindow();
	pAboutDlg->ShowModal();

	delete pAboutDlg;
}

// 双击事件
void  CDuiFrameWnd::OnDbClick(BOOL & bHandled) {
	bHandled = FALSE;

	POINT point;
	GetCursorPos(&point);
	::ScreenToClient(m_hWnd, &point);

	// 没有点击到控件
	CControlUI * pFindControl = m_PaintManager.FindSubControlByPoint(0, point);
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

	DuiLib::CDuiString  strName;
	while (pFindControl) {
		clsName = pFindControl->GetClass();
		strName = pFindControl->GetName();
		if ( clsName == "Grid" ) {
			m_tabs->SelectItem(1);
			bHandled = TRUE;
			break;
		}
		else if (strName == "layCurve") {
			m_tabs->SelectItem(0);
			bHandled = TRUE;
			break;
		}
		else if (clsName == "ListTextElement") {
			if (pFindControl->GetParent()->GetParent()->GetName() == "lstItems") {
				POINT cursor_point;
				GetCursorPos(&cursor_point);
				::ScreenToClient(m_hWnd, &cursor_point);

				CListHeaderUI * header = m_lstItems->GetHeader();
				CControlUI * colName = header->GetItemAt(1);
				CListTextElementUI * pListItem = (CListTextElementUI *)pFindControl;
				
				RECT rcName  = colName->GetPos();
				// 如果点击了名字列
				if ( cursor_point.x >= rcName.left && cursor_point.x <= rcName.right ) {					
					RECT rcFindCtl = pFindControl->GetPos();
					RECT rcList = m_lstItems->GetPos();
					RECT rcNew = { rcName.left - rcList.left, rcFindCtl.top - rcList.top, 
						rcName.right - rcList.left, rcFindCtl.bottom - rcList.top };

					m_edName->SetVisible(true);
					m_edName->SetPos(rcNew); 
					m_edName->SetFocus();   
					m_edName->SetText(pListItem->GetText(1));
					m_nEditingNameIndex = pListItem->GetIndex();
				}
			}
		}
		pFindControl = pFindControl->GetParent();
	}
}

void  CDuiFrameWnd::OnEdNameKillFocus() {
	if (m_nEditingNameIndex >= 0) {
		CListTextElementUI * pListItem = (CListTextElementUI *)m_lstItems->GetItemAt(m_nEditingNameIndex);
		if ( pListItem ) {
			pListItem->SetText(1, m_edName->GetText());
		}		
	}

	m_nEditingNameIndex = -1;
	m_edName->SetVisible(false);   
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


