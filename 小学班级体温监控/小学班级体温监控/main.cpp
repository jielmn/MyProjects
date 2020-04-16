// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"

#define   TEMP_FADE_TIMER              1001
#define   TEMP_FADE_TIMER_ELAPSE       10000

CDuiFrameWnd::CDuiFrameWnd() {
	m_lstClasses = 0;
	m_layDesks = 0;
	m_layRows = 0;
	m_layCols = 0;
	m_dwHighlightIndex = 0;
	m_layRoom = 0;
	memset(&m_dragdrop_desk, 0, sizeof(m_dragdrop_desk));
	m_DragdropUI = 0;
	m_lblBarTips = 0;
	memset(&m_tNewTag, 0, sizeof(m_tNewTag));
	m_lblNewTagTemp = 0;
	m_lblNewTagTime = 0;
	memset(&m_dragdrop_tag, 0, sizeof(m_dragdrop_tag));	
	m_lblNewTagId = 0;
}
            
CDuiFrameWnd::~CDuiFrameWnd() {
	m_vClasses.clear();
	ClearVector(m_vDeskes);
}
          
void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = this->GetHWND();

	m_lstClasses = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("lstClasses"));
	m_layDesks   = static_cast<DuiLib::CTileLayoutUI*>(m_PaintManager.FindControl("layDesks"));
	m_layRows    = static_cast<DuiLib::CVerticalLayoutUI*>(m_PaintManager.FindControl("layRows"));
	m_layCols    = static_cast<DuiLib::CTileLayoutUI*>(m_PaintManager.FindControl("layCols"));
	m_layRoom    = static_cast<DuiLib::CContainerUI*>(m_PaintManager.FindControl("layRoom"));
	m_DragdropUI = m_PaintManager.FindControl("DragDropControl");
	m_lblBarTips = static_cast<CLabelUI*>(m_PaintManager.FindControl("lblStatus")); 
	m_lblNewTagTemp = static_cast<CLabelUI*>(m_PaintManager.FindControl("lblTemp"));
	m_lblNewTagTime = static_cast<CLabelUI*>(m_PaintManager.FindControl("lblTime"));
	m_layNewTag     = static_cast<CContainerUI*>(m_PaintManager.FindControl("layNewTag"));
	m_lblNewTagId   = static_cast<CLabelUI*>(m_PaintManager.FindControl("lblTagId"));

#if !SHOW_TAG_ID
	m_lblNewTagId->SetVisible(false);
#endif

	m_layDesks->SetFixedColumns(g_data.m_dwRoomCols);
	m_layDesks->SetItemSize(g_data.m_DeskSize);
 
	SIZE s = { g_data.m_DeskSize.cx, 20 };
	m_layCols->SetFixedColumns(g_data.m_dwRoomCols);
	m_layCols->SetItemSize(s);

	CDuiString strText;
	for ( DWORD i = 0; i < g_data.m_dwRoomRows; i++ ) {
		CLabelUI * pRowUi = new CLabelUI;
		m_layRows->AddAt(pRowUi,0);
		strText.Format("%lu", i + 1);
		pRowUi->SetText(strText);
		pRowUi->SetFixedHeight(g_data.m_DeskSize.cy);
		pRowUi->SetFont(2);
		pRowUi->SetTextColor(0xFF447AA1);
		pRowUi->SetAttribute("align", "center"); 
	}

	assert(g_data.m_dwRoomCols <= MAX_ROOM_COLUMN);
	for (DWORD i = 0; i < g_data.m_dwRoomCols && i < 10; i++) {
		CLabelUI * pRowUi = new CLabelUI;
		m_layCols->Add(pRowUi);
		pRowUi->SetText(g_data.m_aszChNo[i]);
		pRowUi->SetFont(2);
		pRowUi->SetTextColor(0xFF447AA1);
		pRowUi->SetAttribute("align", "center");
	}

	for (DWORD i = 0; i < g_data.m_dwRoomRows; i++) {
		for (DWORD j = 0; j < g_data.m_dwRoomCols; j++) {
			CDeskUI * pDesk = new CDeskUI;
			pDesk->SetTag( MAKEWORD(j+1, i+1) );
			//SNPRINTF(pDesk->m_data.szName, 12, "%d,%d", i + 1, j + 1);
			//pDesk->m_data.bValid = TRUE;
			m_layDesks->AddAt(pDesk, j);
		}
	}

	m_layRoom->SetVisible(false);

	CBusiness::GetInstance()->GetAllClassesAsyn();
	WindowImplBase::InitWindow(); 
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	DuiLib::CDialogBuilder builder;
	DuiLib::CDuiString  strText;
	DuiLib::CControlUI * pUI = 0;

	if ( 0 == strcmp(pstrClass, "Classes") ) {
		strText.Format("%s.xml", pstrClass);
		pUI = builder.Create((const char *)strText, (UINT)0, this, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(pstrClass, "Newtag")) {
		strText.Format("%s.xml", pstrClass); 
		pUI = builder.Create((const char *)strText, (UINT)0, this, &m_PaintManager);
		return pUI;     
	}
	else if (0 == strcmp(pstrClass, "Classroom")) {
		strText.Format("%s.xml", pstrClass);
		pUI = builder.Create((const char *)strText, (UINT)0, this, &m_PaintManager);
		return pUI;         
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString  strName = msg.pSender->GetName();

	if (msg.sType == "myselected") {
		OnDeskHighlight(msg.pSender->GetTag());
	}
	else if (msg.sType == "myunselected") {
		OnDeskUnHighlight(msg.pSender->GetTag());
	}
	else if ( msg.sType == "click" ) {
		if (strName == "btnAddClass") {
			OnAddClass();
		}
		else if (strName == "btnDelClass") {
			OnDelClass();
		}
	}
	else if (msg.sType == "itemselect") {
		if ( msg.pSender == m_lstClasses ) {
			UpdateRoom();
		}		
	}
	else if (msg.sType == "mydbclick") {
		OnDeskDbClick((CDeskUI*)msg.pSender);
	}
	else if (msg.sType == "emptydesk") {
		int nSel = m_lstClasses->GetCurSel();
		assert(nSel >= 0);
		DWORD  dwNo  = m_lstClasses->GetItemAt(nSel)->GetTag();
		DWORD  dwPos = msg.pSender->GetTag();

		CDeskUI * pDeskUI = (CDeskUI *)msg.pSender;
		if (pDeskUI->m_data.bValid) {
			CBusiness::GetInstance()->EmptyDeskAsyn(dwNo, dwPos);
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_GET_ALL_CLASSES_RET ) {
		m_vClasses.clear();
		std::vector<DWORD> * pvRet = (std::vector<DWORD>*)wParam;
		std::copy(pvRet->begin(), pvRet->end(), std::back_inserter(m_vClasses));
		delete pvRet;

		std::sort(m_vClasses.begin(), m_vClasses.end(), sortDWord );
		UpdateClasses();

		// 检查串口
		CheckDevice();
	}
	else if ( uMsg == UM_GET_ROOM_RET ) {
		std::vector<DeskItem*> * pvRet = (std::vector<DeskItem*> *)wParam;
		DWORD   dwNo = lParam;
		UpdateRoom1(*pvRet, dwNo);
		ClearVector(*pvRet);
		delete pvRet;
	}
	else if (uMsg == UM_MODIFY_DESK_RET) {
		DeskItem* pItem = (DeskItem*)wParam;
		assert(pItem->nGrade > 0 && pItem->nGrade <= 6);
		assert(pItem->nClass > 0 && pItem->nClass <= MAX_CLASS_NUM);
		assert(pItem->nRow   > 0 && pItem->nRow <= (int)g_data.m_dwRoomRows);
		assert(pItem->nCol   > 0 && pItem->nCol <= (int)g_data.m_dwRoomCols);

		DWORD  dwUiRow = g_data.m_dwRoomRows - pItem->nRow;
		CDeskUI * pDeskUI = (CDeskUI *)m_layDesks->GetItemAt(dwUiRow * g_data.m_dwRoomCols + pItem->nCol - 1);
		assert(pDeskUI);

		memset(&pDeskUI->m_data, 0, sizeof(DeskItem));
		STRNCPY(pDeskUI->m_data.szName, pItem->szName, sizeof(pDeskUI->m_data.szName));
		pDeskUI->m_data.nSex = pItem->nSex;
		pDeskUI->m_data.bValid = TRUE;
		pDeskUI->UpdateUI();

		delete pItem;
	}
	else if (uMsg == UM_EMPTY_DESK_RET) {
		DWORD  dwNo  = wParam;
		DWORD  dwPos = lParam;
		DWORD  dwRow = HIBYTE(dwPos);
		DWORD  dwCol = LOBYTE(dwPos);
		assert(dwRow > 0 && dwRow <= g_data.m_dwRoomRows);
		assert(dwCol > 0 && dwCol <= g_data.m_dwRoomCols);

		DWORD  dwUiRow = g_data.m_dwRoomRows - dwRow;
		CDeskUI * pDeskUI = (CDeskUI *)m_layDesks->GetItemAt(dwUiRow * g_data.m_dwRoomCols + dwCol - 1);
		assert(pDeskUI);

		if (pDeskUI->m_data.bValid) {
			pDeskUI->SetValid(FALSE);
			pDeskUI->UpdateUI();
		}
	}
	else if (uMsg == UM_DELETE_CLASS_RET) {
		DWORD  dwNo = wParam;
		std::vector<DWORD>::iterator it_find = std::find(m_vClasses.begin(), m_vClasses.end(), dwNo);
		if ( it_find != m_vClasses.end() ) {
			m_vClasses.erase(it_find);
			UpdateClasses();
		}
	}
	else if (uMsg == UM_EXCHANGE_DESK_RET) {
		DeskItem* pDesk1 = (DeskItem*)wParam;
		DeskItem* pDesk2 = (DeskItem*)lParam;

		assert(pDesk1->nRow > 0 && pDesk1->nRow <= (int)g_data.m_dwRoomRows);
		assert(pDesk1->nCol > 0 && pDesk1->nCol <= (int)g_data.m_dwRoomCols);
		assert(pDesk2->nRow > 0 && pDesk2->nRow <= (int)g_data.m_dwRoomRows);
		assert(pDesk2->nCol > 0 && pDesk2->nCol <= (int)g_data.m_dwRoomCols);

		DWORD  dwUiRow = g_data.m_dwRoomRows - pDesk1->nRow;
		CDeskUI * pDeskUI = (CDeskUI *)m_layDesks->GetItemAt(dwUiRow * g_data.m_dwRoomCols + pDesk1->nCol - 1);
		assert(pDeskUI);
		memcpy(&pDeskUI->m_data, pDesk1, sizeof(DeskItem));
		pDeskUI->UpdateUI();

		dwUiRow = g_data.m_dwRoomRows - pDesk2->nRow;
		pDeskUI = (CDeskUI *)m_layDesks->GetItemAt(dwUiRow * g_data.m_dwRoomCols + pDesk2->nCol - 1);
		assert(pDeskUI);
		memcpy(&pDeskUI->m_data, pDesk2, sizeof(DeskItem));
		pDeskUI->UpdateUI();

		delete pDesk1;
		delete pDesk2;
	}
	else if (uMsg == WM_DEVICECHANGE) {
		CheckDevice();
	}
	else if (uMsg == UM_CHECK_COM_PORTS_RET) {
		std::vector<int> * pvRet = (std::vector<int> *)wParam; 
		assert(pvRet);
		if (pvRet->size() == 0) {
			m_lblBarTips->SetText("没有打开任何接收基站");
		}
		else {
			std::vector<int>::iterator it;
			CDuiString strText = "打开了";
			int i = 0;
			for (it = pvRet->begin(); it != pvRet->end(); ++it, i++) {
				if (i > 0) {
					strText += ",";
				}
				CDuiString item;
				item.Format("com%d", *it);
				strText += item;
			}
			m_lblBarTips->SetText(strText);
		}
		if (pvRet) {
			delete pvRet;
		}
	}
	else if (UM_NEW_TAG_TEMPERATURE == uMsg) {
		TempItem * pItem = (TempItem *)wParam;
		memcpy(&m_tNewTag, pItem, sizeof(TempItem));
		delete pItem;

		UpdateNewTag();
		::SetTimer(GetHWND(), TEMP_FADE_TIMER, TEMP_FADE_TIMER_ELAPSE, 0);
	}
	else if (WM_TIMER == uMsg) {               
		if (wParam == TEMP_FADE_TIMER) {
			m_lblNewTagTemp->SetTextColor(FADE_TEMP_COLOR);
			m_lblNewTagTime->SetTextColor(FADE_TEMP_COLOR);
			::KillTimer(GetHWND(), wParam);
		}
	}
	else if (UM_BINDING_TAG_RET == uMsg) {
		memset(&m_tNewTag, 0, sizeof(m_tNewTag));
		UpdateNewTag();
		::KillTimer(GetHWND(), TEMP_FADE_TIMER);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
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

	while (pCtl) {
		if (0 == strcmp(pCtl->GetClass(), "Desk")) {
			// 不是点击了按钮
			if (0 != strcmp(pOriginalCtl->GetClass(), "Button")) {
				m_dragdrop_desk.m_source      = (CDeskUI *)pCtl;
				m_dragdrop_desk.m_highlight   = 0;
				m_dragdrop_desk.m_dwStartTick = LmnGetTickCount();				
				m_dragdrop_desk.m_bStarted    = TRUE;
			}
		}
		else if (pCtl->GetName() == "layNewTag") {
			if ( m_tNewTag.m_szTagId[0] != '\0' ) {
				m_dragdrop_tag.m_highlight = 0;
				m_dragdrop_tag.m_dwStartTick = LmnGetTickCount();
				m_dragdrop_tag.m_bStarted = TRUE;
			}			
		}
		pCtl = pCtl->GetParent();
	}

	return WindowImplBase::OnLButtonDown(uMsg, wParam, lParam, bHandled);
}

LRESULT  CDuiFrameWnd::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	if (m_dragdrop_desk.m_bStarted) {
		OnStopMoveDesk();
	}
	else if (m_dragdrop_tag.m_bStarted) {
		OnStopMoveTag();
	}
	return WindowImplBase::OnLButtonUp(uMsg, wParam, lParam, bHandled);
}

void  CDuiFrameWnd::OnStopMoveDesk() {
	m_DragdropUI->SetVisible(false);
	m_dragdrop_desk.m_bStarted = FALSE;

	if (m_dragdrop_desk.m_highlight) {
		m_dragdrop_desk.m_highlight->SetHighlightBorder(FALSE);

		int nSel = m_lstClasses->GetCurSel();
		assert(nSel >= 0);
		DWORD  dwNo   = m_lstClasses->GetItemAt(nSel)->GetTag();
		DWORD  dwPos1 = m_dragdrop_desk.m_source->GetTag();
		DWORD  dwPos2 = m_dragdrop_desk.m_highlight->GetTag();

		CBusiness::GetInstance()->ExchangeDeskAsyn(dwNo, dwPos1, dwPos2);

		m_dragdrop_desk.m_highlight = 0;
	}
}

LRESULT  CDuiFrameWnd::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	POINT  pt;
	DWORD  dwCurTick = 0;

	if (m_dragdrop_desk.m_bStarted) {
		dwCurTick = LmnGetTickCount();
		if (dwCurTick - m_dragdrop_desk.m_dwStartTick < 100) {
			return WindowImplBase::OnMouseMove(uMsg, wParam, lParam, bHandled);
		}
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		OnMoveDesk(pt);
	}
	else if (m_dragdrop_tag.m_bStarted) {
		dwCurTick = LmnGetTickCount();
		if (dwCurTick - m_dragdrop_tag.m_dwStartTick < 100) {
			return WindowImplBase::OnMouseMove(uMsg, wParam, lParam, bHandled);
		}
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		OnMoveTag(pt);
	}

	return WindowImplBase::OnMouseMove(uMsg, wParam, lParam, bHandled);
}

void  CDuiFrameWnd::OnMoveDesk(const POINT & pt) {
	if (!m_DragdropUI->IsVisible()) {
		m_DragdropUI->SetVisible(true);
	}

	int x = 60 / 2;
	int y = 60 / 2;

	RECT r;
	r.left = pt.x - x;
	r.right = r.left + x * 2;
	r.top = pt.y - y;
	r.bottom = r.top + y * 2;
	m_DragdropUI->SetPos(r);

	
	// 高亮经过的desk
	CControlUI* pCtl = m_PaintManager.FindSubControlByPoint(m_layDesks, pt);
	CDeskUI * pHighLight = 0;

	// 如果没有移动到任何控件上
	if (0 == pCtl) {
		if (m_dragdrop_desk.m_highlight) {
			m_dragdrop_desk.m_highlight->SetHighlightBorder(FALSE);
		}
		m_dragdrop_desk.m_highlight = 0;
		return;
	}

	while (pCtl) {
		if (pCtl->GetClass() == "Desk") {
			pHighLight = (CDeskUI *)pCtl;
			break;
		}
		pCtl = pCtl->GetParent();
	}

	if (pHighLight) {
		if (m_dragdrop_desk.m_highlight) {
			if (m_dragdrop_desk.m_highlight != pHighLight) {
				pHighLight->SetHighlightBorder(TRUE);
				m_dragdrop_desk.m_highlight->SetHighlightBorder(FALSE);
			}
		}
		else {
			pHighLight->SetHighlightBorder(TRUE);
		}
		m_dragdrop_desk.m_highlight = pHighLight;
	}
	else {
		if (m_dragdrop_desk.m_highlight) {
			m_dragdrop_desk.m_highlight->SetHighlightBorder(FALSE);
		}
		m_dragdrop_desk.m_highlight = 0;
	}
}

void  CDuiFrameWnd::OnDeskHighlight(DWORD dwTag) {
	if ( m_dwHighlightIndex > 0 ) {
		CDeskUI * pDesk = GetDeskUI(m_dwHighlightIndex);
		pDesk->SetHighlight(FALSE);
	}
	m_dwHighlightIndex = dwTag;
	CDeskUI * pDesk = GetDeskUI(dwTag);
	pDesk->SetHighlight(TRUE);
}

void  CDuiFrameWnd::OnDeskUnHighlight(DWORD dwTag) {
	CDeskUI * pDesk = GetDeskUI(dwTag);
	pDesk->SetHighlight(FALSE);
	if (dwTag == m_dwHighlightIndex) {
		m_dwHighlightIndex = 0;
	}
}

CDeskUI *  CDuiFrameWnd::GetDeskUI(DWORD  dwTag) {
	DWORD i = HIBYTE(dwTag);
	DWORD j = LOBYTE(dwTag);
	assert(i <= g_data.m_dwRoomRows && i > 0 );
	assert(j <= g_data.m_dwRoomCols && j > 0 );
	CDeskUI * pDesk = (CDeskUI *)m_layDesks->GetItemAt( (g_data.m_dwRoomRows - i) * g_data.m_dwRoomCols + j - 1 );
	return pDesk;
}

void   CDuiFrameWnd::OnAddClass() {
	RECT rect;
	::GetWindowRect(GetHWND(), &rect);

	CAddClassDlg * pDlg = new CAddClassDlg;
	pDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	::MoveWindow(pDlg->GetHWND(), rect.left + 50, rect.top + 100, 300, 200, TRUE);  
	int ret = pDlg->ShowModal();  

	// 如果不是click ok
	if (0 != ret) {
		delete pDlg;
		return;     
	}

	m_vClasses.push_back(pDlg->m_dwClassNo);
	std::sort(m_vClasses.begin(), m_vClasses.end(), sortDWord);
	UpdateClasses();

	delete pDlg;
}

void  CDuiFrameWnd::OnDelClass() {
	int nSel = m_lstClasses->GetCurSel();
	if ( nSel < 0 ) {
		MessageBox(GetHWND(), "没有选中班级", "错误", 0);
		return;
	}

	if ( IDNO == MessageBox(GetHWND(), "确定要删除该班级吗？", "删除", MB_YESNO | MB_DEFBUTTON2) ) {
		return;
	}

	DWORD  dwNo = m_lstClasses->GetItemAt(nSel)->GetTag();
	CBusiness::GetInstance()->DeleteClassAsyn(dwNo);
}

void  CDuiFrameWnd::UpdateClasses() {
	int nCnt = (int)m_vClasses.size();
	int nUiCnt = m_lstClasses->GetCount();
	int nDiff = 0;

	DWORD dwOldSelNo = 0;
	int nOldSel = m_lstClasses->GetCurSel();
	if ( nOldSel >= 0 ) {
		dwOldSelNo = m_lstClasses->GetItemAt(nOldSel)->GetTag();
	}

	if ( nCnt > nUiCnt ) {
		nDiff = nCnt - nUiCnt;
		for (int i = 0; i < nDiff; i++) {
			CListTextElementUI * pItem = new CListTextElementUI;
			m_lstClasses->Add(pItem);
		}
	}
	else if (nCnt < nUiCnt) {
		nDiff = nUiCnt - nCnt;
		for (int i = 0; i < nDiff; i++) {
			m_lstClasses->RemoveAt(0);
		}
	}

	CDuiString strText;
	for (int i = 0; i < nCnt; i++) {
		DWORD  dwNo    = m_vClasses[i];
		DWORD  dwGrade = HIBYTE(dwNo);
		DWORD  dwClass = LOBYTE(dwNo);

		assert(dwGrade > 0 && dwGrade <= 6);
		assert(dwClass > 0 && dwClass <= MAX_CLASS_NUM);

		strText.Format("%s年级%d班", g_data.m_aszChNo[dwGrade-1], dwClass);
		CListTextElementUI * pItem = (CListTextElementUI *)m_lstClasses->GetItemAt(i);
		pItem->SetText(0, strText);
		pItem->SetTag(dwNo);
	}

	if ( dwOldSelNo > 0 ) {
		int i = 0;
		for (i = 0; i < nCnt; i++) {
			CListTextElementUI * pItem = (CListTextElementUI *)m_lstClasses->GetItemAt(i);
			if (pItem->GetTag() == dwOldSelNo) {
				m_lstClasses->SelectItem(i, false, false);
				break;
			}
		}

		// 如果没有找到
		if ( i >= nCnt ) {
			if (nCnt > 0) {
				m_lstClasses->SelectItem(0, false, false);
			}
		}
	}
	else {
		if ( nCnt > 0 ) {
			m_lstClasses->SelectItem(0, false, false);
		}
	}

	UpdateRoom();
}

void   CDuiFrameWnd::UpdateRoom() {
	int nSel = m_lstClasses->GetCurSel();
	if (nSel < 0) {
		m_layRoom->SetVisible(false);
	}
	else {
		DWORD  dwNo = m_lstClasses->GetItemAt(nSel)->GetTag();
		CBusiness::GetInstance()->GetRoomDataAsyn(dwNo);
	}
}

void   CDuiFrameWnd::UpdateRoom1(std::vector<DeskItem*> vRet, DWORD   dwNo) {
	m_layRoom->SetVisible(true);

	int nCnt = m_layDesks->GetCount();
	assert(nCnt == g_data.m_dwRoomCols * g_data.m_dwRoomRows);
	std::vector<DeskItem*>::iterator it;

	for (int i = 0; i < nCnt; i++) {
		CDeskUI * pDesk = (CDeskUI * )m_layDesks->GetItemAt(i);
		DWORD dwPos = pDesk->GetTag();		
		it = std::find_if(vRet.begin(), vRet.end(), CFindDeskObj(dwPos) );
		if (it == vRet.end()) {
			pDesk->SetValid(FALSE);
			pDesk->UpdateUI();
		}
		else {
			memcpy(&pDesk->m_data, *it, sizeof(DeskItem));
			pDesk->UpdateUI();
		}		
	}
}

void   CDuiFrameWnd::OnDeskDbClick(CDeskUI * pDeskUI) {

	CDeskDlg * pDlg = new CDeskDlg;
	memcpy(&pDlg->m_data, &pDeskUI->m_data, sizeof(DeskItem));

	pDlg->Create(this->m_hWnd, _T("学生"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
	pDlg->CenterWindow();
	int ret = pDlg->ShowModal();          

	// 如果不是click ok
	if (0 != ret) {
		delete pDlg;
		return;
	}

	int nSel = m_lstClasses->GetCurSel();
	assert(nSel >= 0);
	DWORD  dwNo = m_lstClasses->GetItemAt(nSel)->GetTag();
	DWORD  dwPos = pDeskUI->GetTag();

	// 清除Desk
	if (pDlg->m_data.szName[0] == '\0') {
		if ( pDeskUI->m_data.bValid ) {
			CBusiness::GetInstance()->EmptyDeskAsyn(dwNo, dwPos);
		}
	}
	// 增加学生
	else {
		// 如果名字不同，认为修改
		if ( 0 != strcmp(pDeskUI->m_data.szName, pDlg->m_data.szName) ) {
			CBusiness::GetInstance()->ModifyDeskAsyn(dwNo, dwPos, pDlg->m_data.szName, pDlg->m_data.nSex);
		}		
	}

	delete pDlg;
}

void   CDuiFrameWnd::CheckDevice() {
	char szComPort[16];
	int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));

	// 单个串口
	if (!g_data.m_bMultipleComport) {
		if (nFindCount > 1) {
			m_lblBarTips->SetText("存在多个USB-SERIAL CH340串口，请只连接一个发射器");
		}
		else if (0 == nFindCount) {
			m_lblBarTips->SetText("没有找到USB-SERIAL CH340串口，请连接发射器的USB线");
		}
		else {
			m_lblBarTips->SetText("");
		}
	}
	else {
		if (0 == nFindCount) {
			m_lblBarTips->SetText("没有找到USB-SERIAL CH340串口，请连接发射器的USB线");
		}
		else {
			m_lblBarTips->SetText("");
		}
	}

	CBusiness::GetInstance()->CheckLaunchAsyn();    
}

void   CDuiFrameWnd::UpdateNewTag() {
	CDuiString  strText;

	if (m_tNewTag.m_szTagId[0] != '\0') {
		strText.Format("%.1f℃", m_tNewTag.m_dwTemp / 100.0f);
		m_lblNewTagTemp->SetText(strText);
		m_lblNewTagTemp->SetTextColor(FRESH_TEMP_COLOR);

		char  szTime[256];
		LmnFormatTime(szTime, sizeof(szTime), m_tNewTag.m_time, "%Y-%m-%d %H:%M:%S");
		m_lblNewTagTime->SetText(szTime);
		m_lblNewTagTime->SetTextColor(FRESH_TEMP_COLOR);

		m_lblNewTagId->SetText(m_tNewTag.m_szTagId);
		m_lblNewTagId->SetTextColor(FRESH_TEMP_COLOR);
	}
	else {
		m_lblNewTagTemp->SetText("");
		m_lblNewTagTemp->SetTextColor(FADE_TEMP_COLOR);
		m_lblNewTagTime->SetText("");
		m_lblNewTagTime->SetTextColor(FADE_TEMP_COLOR);
		m_lblNewTagId->SetText("");
		m_lblNewTagId->SetTextColor(FADE_TEMP_COLOR);
	}
}

void  CDuiFrameWnd::OnMoveTag(const POINT & pt) {
	if (!m_DragdropUI->IsVisible()) {
		m_DragdropUI->SetVisible(true);
	}

	int x = 60 / 2;
	int y = 60 / 2;

	RECT r;
	r.left = pt.x - x;
	r.right = r.left + x * 2;
	r.top = pt.y - y;
	r.bottom = r.top + y * 2;
	m_DragdropUI->SetPos(r);


	// 高亮经过的desk
	CControlUI* pCtl = m_PaintManager.FindSubControlByPoint(m_layDesks, pt);
	CDeskUI * pHighLight = 0;

	// 如果没有移动到任何控件上
	if (0 == pCtl) {
		if (m_dragdrop_tag.m_highlight) {
			m_dragdrop_tag.m_highlight->SetHighlightBorder(FALSE);
		}
		m_dragdrop_tag.m_highlight = 0;
		return;
	}

	while (pCtl) {
		if (pCtl->GetClass() == "Desk") {
			pHighLight = (CDeskUI *)pCtl;
			break;
		}
		pCtl = pCtl->GetParent();
	}

	if (pHighLight) {
		if (m_dragdrop_tag.m_highlight) {
			if (m_dragdrop_tag.m_highlight != pHighLight) {
				pHighLight->SetHighlightBorder(TRUE);
				m_dragdrop_tag.m_highlight->SetHighlightBorder(FALSE);
			}
		}
		else {
			pHighLight->SetHighlightBorder(TRUE);
		}
		m_dragdrop_tag.m_highlight = pHighLight;
	}
	else {
		if (m_dragdrop_tag.m_highlight) {
			m_dragdrop_tag.m_highlight->SetHighlightBorder(FALSE);
		}
		m_dragdrop_tag.m_highlight = 0;
	}
}

void  CDuiFrameWnd::OnStopMoveTag() {
	m_DragdropUI->SetVisible(false);
	m_dragdrop_tag.m_bStarted = FALSE;

	if (m_dragdrop_tag.m_highlight) {
		m_dragdrop_tag.m_highlight->SetHighlightBorder(FALSE);

		if ( m_dragdrop_tag.m_highlight->m_data.bValid ) {
			int nSel = m_lstClasses->GetCurSel();
			assert(nSel >= 0);
			DWORD  dwNo = m_lstClasses->GetItemAt(nSel)->GetTag();
			DWORD  dwPos = m_dragdrop_tag.m_highlight->GetTag();
			CBusiness::GetInstance()->BindingTag2DeskAsyn(m_tNewTag.m_szTagId, dwNo, dwPos);
		}	

		m_dragdrop_tag.m_highlight = 0;
	}
}
                
 
CAddClassDlg::CAddClassDlg() {
	m_cmbGrade = 0;
	m_cmbClass = 0;
	m_dwClassNo = 0;    // 年级 << 8 | 班级
}

void  CAddClassDlg::Notify(DuiLib::TNotifyUI& msg) {
	CDuiString  strName = msg.pSender->GetName();
	if (msg.sType == "click") {
		if (strName == "btnOK") {
			int nSel = m_cmbGrade->GetCurSel();
			DWORD i  = m_cmbGrade->GetItemAt(nSel)->GetTag();
			nSel = m_cmbClass->GetCurSel();
			DWORD j = m_cmbGrade->GetItemAt(nSel)->GetTag();
			CBusiness::GetInstance()->AddClassAsyn(GetHWND(), i, j);
		}
	}
	WindowImplBase::Notify(msg);
}

void  CAddClassDlg::InitWindow() {
	m_cmbGrade = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbGrade"));
	m_cmbClass = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbClass"));

	for (int i = 0; i < 6; i++) {
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(g_data.m_aszChNo[i]);
		pElement->SetTag(i+1);
		m_cmbGrade->Add(pElement);
	}
	m_cmbGrade->SelectItem(0); 

	CDuiString  strText;
	for (int i = 0; i < 10; i++) {
		CListLabelElementUI * pElement = new CListLabelElementUI;
		strText.Format("%d", i + 1);
		pElement->SetText(strText);
		pElement->SetTag(i+1);
		m_cmbClass->Add(pElement); 
	}  
	m_cmbClass->SelectItem(0);

	WindowImplBase::InitWindow();
}

LRESULT  CAddClassDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_ADD_CLASS_RET ) {
		BOOL bRet = (BOOL)wParam;
		if (bRet) {
			int nSel = m_cmbGrade->GetCurSel();
			DWORD i = m_cmbGrade->GetItemAt(nSel)->GetTag();
			nSel = m_cmbClass->GetCurSel();
			DWORD j = m_cmbClass->GetItemAt(nSel)->GetTag();
			m_dwClassNo = MAKEWORD(j, i);
			this->PostMessage(WM_CLOSE);
		}
		else {
			MessageBox(GetHWND(), "添加班级失败！", "错误", 0);
		}
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CAddClassDlg::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}




CDeskDlg::CDeskDlg() {
	memset( &m_data, 0, sizeof(m_data) );
	m_data.nSex = 1;

	m_edName = 0;
	m_opBoy = 0;
	m_opGirl = 0;
}

void   CDeskDlg::Notify(DuiLib::TNotifyUI& msg) {
	CDuiString  strName = msg.pSender->GetName();
	if (msg.sType == "click") {
		if (strName == "btnOK") {
			CDuiString  strText;
			strText = m_edName->GetText();
			strText.Trim();
			STRNCPY(m_data.szName, strText, sizeof(m_data.szName));
			if (m_opBoy->IsSelected()) {
				m_data.nSex = 1;
			}
			else {
				m_data.nSex = 0;
			}
			this->PostMessage(WM_CLOSE);
		}
	}
	WindowImplBase::Notify(msg);
}

void   CDeskDlg::InitWindow() {
	m_edName = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edName"));
	m_opBoy  = static_cast<DuiLib::COptionUI*>(m_PaintManager.FindControl("radio1"));
	m_opGirl = static_cast<DuiLib::COptionUI*>(m_PaintManager.FindControl("radio2"));

	if (m_data.bValid) {
		m_edName->SetText(m_data.szName);
		if (1 == m_data.nSex)
			m_opBoy->Selected(true);
		else
			m_opGirl->Selected(true);
	}
	
	m_edName->SetFocus();
	WindowImplBase::InitWindow();
}

LRESULT  CDeskDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CDeskDlg::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
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


