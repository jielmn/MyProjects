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

#define   GRID_WIDTH      180                          
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
	m_nItemsPerPage = 0;
	memset(&m_rcLayGrids, 0, sizeof(m_rcLayGrids));
	memset(m_Sort, 0, sizeof(m_Sort));
	memset(m_Header, 0, sizeof(m_Header));
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
	m_Header[0] = (DuiLib::CListHeaderItemUI *)m_PaintManager.FindControl("hdName");
	m_Header[1] = (DuiLib::CListHeaderItemUI *)m_PaintManager.FindControl("hdHeartBeat");
	m_Header[2] = (DuiLib::CListHeaderItemUI *)m_PaintManager.FindControl("hdOxy");
	m_Header[3] = (DuiLib::CListHeaderItemUI *)m_PaintManager.FindControl("hdTemp");

	m_layList->SetVisible(false); 
	m_btnExpand->SetText("<");   
	m_btnExpand->SetTag(FALSE); 

	for (int i = 0; i < 120; i++) {
		CWearItem * p = new CWearItem;
		SNPRINTF( p->m_szDeviceId, sizeof(p->m_szDeviceId), "%d", i + 1);
		if ( 0 == i )
			SNPRINTF(p->m_szName, sizeof(p->m_szName), "张三");
		else if ( 1== i )
			SNPRINTF(p->m_szName, sizeof(p->m_szName), "李四");
		else if (2 == i)
			SNPRINTF(p->m_szName, sizeof(p->m_szName), "王五");
		else if (3 == i)
			SNPRINTF(p->m_szName, sizeof(p->m_szName), "张麻子");
		else if (4 == i)
			SNPRINTF(p->m_szName, sizeof(p->m_szName), "李宝");
		else if (5 == i)
			SNPRINTF(p->m_szName, sizeof(p->m_szName), "阿万");
		else if (6 == i)
			SNPRINTF(p->m_szName, sizeof(p->m_szName), "方大同");
		else if (7 == i)
			SNPRINTF(p->m_szName, sizeof(p->m_szName), "阿狸");
		else if (8 == i)
			SNPRINTF(p->m_szName, sizeof(p->m_szName), "阿猫");
		else if (9 == i)
			SNPRINTF(p->m_szName, sizeof(p->m_szName), "阿狗");
		else
			SNPRINTF( p->m_szName, sizeof(p->m_szName), "%d", i + 1); 

		p->m_nHearbeat = i + 1;
		p->m_nOxy = i + 100;
		p->m_nTemp = i + 1000;
		p->m_nPose = i + 60;

		m_data.push_back(p);
	}

	m_layGrids->OnSize += MakeDelegate(this, &CDuiFrameWnd::OnGridsLayoutSize);
	UpdateList();

	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == "click") {
		if (name == "btnExpand") {
			OnExpand();
		}
		else if (name == "menubtn") {
			OnMainMenu(msg);
		}
		else if (name == "btnPrevPage") {
			OnPrevPage();
		}
		else if (name == "btnNextPage") {
			OnNextPage();
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
	else if (msg.sType == "headerclick") {
		int nIndex = -1;
		if (name == "hdName") {
			nIndex = 0;			
		}
		else if (name == "hdHeartBeat") {
			nIndex = 1;
		}
		else if (name == "hdOxy") {
			nIndex = 2;
		}
		else if (name == "hdTemp") {
			nIndex = 3;
		}

		if (nIndex >= 0 && nIndex <= 3) {
			for ( int i = 0; i < 4; i++ ) {
				if ( i == nIndex )
					continue;
				m_Sort[i].bSorted = FALSE;
			}
			g_data.m_bWarningPrepose = FALSE; 

			if ( !m_Sort[nIndex].bSorted ) {
				m_Sort[nIndex].bSorted = TRUE;
				m_Sort[nIndex].bAscend = TRUE;
				Sort(nIndex);
			}
			else {
				m_Sort[nIndex].bAscend = !m_Sort[nIndex].bAscend;
				Sort(nIndex);
			}    

		}		
	}
	else if (msg.sType == "cancelsort") {
		CancelSort();
	}
	else if (msg.sType == "textchanged") {
		if (msg.pSender->GetClass() == "EditableButton" ) {
			if (name == "cstEdtBtnName") {
				CWearItem * pItem = (CWearItem *)msg.pSender->GetParent()->GetParent()->GetParent()->GetParent()->GetTag();
				assert(pItem);
				STRNCPY(pItem->m_szName, msg.pSender->GetText(), sizeof(pItem->m_szName));
				UpdateList();
			}
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

	if ( (width == m_rcLayGrids.right - m_rcLayGrids.left )
		&& (height == m_rcLayGrids.bottom - m_rcLayGrids.top) ) {
		return true;
	}

	memcpy(&m_rcLayGrids, &r, sizeof(RECT));

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
	
	// 如果没有数据
	if (m_data.size() == 0) {
		m_nCurPageFirstItemIndex = 0;
		return true;
	}

	// 每页多少格子
	m_nItemsPerPage = nRows * nColumns;
	// 总页数
	int nPagesCnt = ( m_data.size() - 1 ) / m_nItemsPerPage + 1;
	// 计算当前页
	int nCurPage = m_nCurPageFirstItemIndex / m_nItemsPerPage;
	// 重新计算 m_nCurPageFirstItemIndex
	m_nCurPageFirstItemIndex = nCurPage * m_nItemsPerPage;

	if (nCurPage > 0) {
		m_btnPrev->SetEnabled(true);
	}
	else {
		m_btnPrev->SetEnabled(false);
	}

	if ( nCurPage < nPagesCnt - 1 ) {
		m_btnNext->SetEnabled(true);
	}
	else {
		m_btnNext->SetEnabled(false);
	}

	UpdateGrids();
	return true;          
}

void  CDuiFrameWnd::UpdateGrids() {

	int nCurPageItemsCnt = (int)m_data.size() < m_nCurPageFirstItemIndex + m_nItemsPerPage
		? (m_data.size() - m_nCurPageFirstItemIndex) : m_nItemsPerPage;

	int nItemCnt = m_layGrids->GetCount();

	// 如果实际的items过多
	if ( nItemCnt > nCurPageItemsCnt ) {
		for ( int i = 0; i < (nItemCnt - nCurPageItemsCnt); i++ ) {
			m_layGrids->RemoveAt(0);
		}
	}
	// 如果实际的items比需要的少
	else {
		for ( int i = 0; i < (nCurPageItemsCnt - nItemCnt); i++ ) {
			CGridUI * pGrid = new CGridUI;
			m_layGrids->Add(pGrid);
		}
	}

	for ( int i = 0; i < nCurPageItemsCnt; i++ ) {
		CWearItem * pItem = m_data[m_nCurPageFirstItemIndex + i];
		CGridUI * pGrid = (CGridUI *)m_layGrids->GetItemAt(i);

		pGrid->SetIndex(m_nCurPageFirstItemIndex + i + 1);
		pGrid->SetUserName(pItem->m_szName);
		pGrid->SetDeviceId(pItem->m_szDeviceId);		      
		pGrid->SetHeartBeat(pItem);
		pGrid->SetOxy(pItem);		
		pGrid->SetTemp(pItem); 
		pGrid->SetPose(pItem->m_nPose);

		pGrid->SetTag((int)pItem);
	}
}

void  CDuiFrameWnd::OnPrevPage() {
	assert(m_nItemsPerPage > 0);
	m_nCurPageFirstItemIndex -= m_nItemsPerPage;
	assert(m_nCurPageFirstItemIndex >= 0);	

	if ( m_nCurPageFirstItemIndex == 0 ) {
		m_btnPrev->SetEnabled(false);
	}
	m_btnNext->SetEnabled(true);

	UpdateGrids();
}

void  CDuiFrameWnd::OnNextPage() {
	assert(m_nItemsPerPage > 0);
	m_nCurPageFirstItemIndex += m_nItemsPerPage;
	assert(m_nCurPageFirstItemIndex < (int)m_data.size());

	// 如果是最后一页
	if ( m_nCurPageFirstItemIndex + m_nItemsPerPage >= (int)m_data.size() ) {
		m_btnNext->SetEnabled(false);
	}
	m_btnPrev->SetEnabled(true);

	UpdateGrids();
}

CDuiString  CDuiFrameWnd::GetHeartBeatStr(CWearItem * pItem) {
	CDuiString  strText;
	if (pItem->m_nHearbeat > 0) {
		strText.Format("%d", pItem->m_nHearbeat);
	}
	return strText;
}

CDuiString  CDuiFrameWnd::GetOxyStr(CWearItem * pItem) {
	CDuiString  strText;
	if (pItem->m_nOxy > 0) {
		strText.Format("%d", pItem->m_nOxy);
	}
	return strText;
}

CDuiString  CDuiFrameWnd::GetTempStr(CWearItem * pItem) {
	CDuiString  strText;
	if (pItem->m_nTemp > 0) {
		strText.Format("%.1f", pItem->m_nTemp / 100.0f);
	}
	return strText;         
}

// 更新List
void  CDuiFrameWnd::UpdateList() {
	std::vector<CWearItem *>::iterator it;
	int nItemsCnt = m_lstItems->GetCount();
	int nSize = m_data.size();

	// 如果list items的个数不够
	if ( nSize > nItemsCnt ) {
		for ( int i = 0; i < nSize - nItemsCnt; i++ ) {
			CListTextElementUI * pListItem = new CListTextElementUI;
			m_lstItems->Add(pListItem);
		}
	}
	// 如果list items的个数过多
	else {
		for (int i = 0; i < nItemsCnt - nSize; i++) {
			m_lstItems->RemoveAt(0);
		}
	}

	CDuiString strText;
	for (int i = 0; i < nSize; i++) {
		CWearItem * pItem = m_data[i];
		CListTextElementUI * pListItem = (CListTextElementUI *)m_lstItems->GetItemAt(i);

		strText.Format("%d", i + 1);
		pListItem->SetText(0, strText);
		pListItem->SetText(1, pItem->m_szName);
		pListItem->SetText(2, GetHeartBeatStr(pItem));
		pListItem->SetText(3, GetOxyStr(pItem));
		pListItem->SetText(4, GetTempStr(pItem));
		pListItem->SetText(5, pItem->m_szDeviceId);  

		pListItem->SetTag((int)pItem);
	}
}

void  CDuiFrameWnd::OnCheckHistory() {
	g_data.m_bShowHistory = !g_data.m_bShowHistory;
}

void  CDuiFrameWnd::OnWarningPrepose() {
	g_data.m_bWarningPrepose = !g_data.m_bWarningPrepose;
	// 如果异常前置，取消排序
	if (g_data.m_bWarningPrepose) {
		CancelSort();
	}
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
			CWearItem * pItem = (CWearItem *)pListItem->GetTag();
			assert(pItem);
			STRNCPY(pItem->m_szName, m_edName->GetText(), sizeof(pItem->m_szName));
			UpdateGrids();
		}		
	}

	m_nEditingNameIndex = -1;
	m_edName->SetVisible(false);   
}

void  CDuiFrameWnd::Sort(int nIndex) {
	if (0 == nIndex) {
		std::sort( m_data.begin(), m_data.end(), CSortName(m_Sort[nIndex].bAscend) );
	}
	else if (1 == nIndex) {
		std::sort(m_data.begin(), m_data.end(), CSortHeartBeat(m_Sort[nIndex].bAscend));
	}
	else if (2 == nIndex) {
		std::sort(m_data.begin(), m_data.end(), CSortOxy(m_Sort[nIndex].bAscend));
	}
	else if (3 == nIndex) {
		std::sort(m_data.begin(), m_data.end(), CSortTemp(m_Sort[nIndex].bAscend));
	}

	for (int i = 0; i < 4; i++) {
		if ( i == nIndex ) {
			if (m_Sort[nIndex].bAscend) {
				m_Header[nIndex]->SetBkImage("file='list_header_bg_sort_1.png' corner='0,0,10,0'");
				m_Header[nIndex]->SetHotImage("file='list_header_hot_sort_1.png' corner='0,0,10,0'");
			}
			else {
				m_Header[nIndex]->SetBkImage("file='list_header_bg_sort.png' corner='0,0,10,0'");
				m_Header[nIndex]->SetHotImage("file='list_header_hot_sort.png' corner='0,0,10,0'");
			}
		}
		else {
			m_Header[i]->SetBkImage("file='list_header_bg.png' corner='0,0,10,0'");
			m_Header[i]->SetHotImage("file='list_header_hot.png' corner='0,0,10,0'");
		}
	}	

	UpdateList();
	UpdateGrids(); 
}

void  CDuiFrameWnd::CancelSort() {
	for (int i = 0; i < 4; i++) {
		m_Sort[i].bSorted = FALSE;
		m_Header[i]->SetBkImage("file='list_header_bg.png' corner='0,0,10,0'");
		m_Header[i]->SetHotImage("file='list_header_hot.png' corner='0,0,10,0'");
	}
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


