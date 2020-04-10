// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"

CDuiFrameWnd::CDuiFrameWnd() {
	m_lstClasses = 0;
	m_layDesks = 0;
	m_layRows = 0;
	m_layCols = 0;
	m_dwHighlightIndex = 0;
	m_layRoom = 0;
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

	assert(g_data.m_dwRoomCols <= 10);
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
			pDesk->SetTag( MAKELONG(j+1, i+1) );
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
		UpdateRoom();
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
	}
	else if ( uMsg == UM_GET_ROOM_RET ) {
		std::vector<DeskItem*> * pvRet = (std::vector<DeskItem*> *)wParam;
		DWORD   dwNo = lParam;

		UpdateRoom1(*pvRet, dwNo);

		ClearVector(*pvRet);
		delete pvRet;
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
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
	DWORD i = HIWORD(dwTag);
	DWORD j = LOWORD(dwTag);
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

	delete pDlg;
}

void  CDuiFrameWnd::OnDelClass() {

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
		DWORD  dwGrade = HIWORD(dwNo);
		DWORD  dwClass = LOWORD(dwNo);

		assert(dwGrade > 0 && dwGrade <= 6);
		assert(dwClass > 0 && dwClass <= 10);

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
			continue;
		}

		memcpy(&pDesk->m_data, *it, sizeof(DeskItem));
		pDesk->UpdateUI();
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
			DWORD j = m_cmbGrade->GetItemAt(nSel)->GetTag();
			m_dwClassNo = MAKELONG(j, i);
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


