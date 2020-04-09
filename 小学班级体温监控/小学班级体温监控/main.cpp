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
}
            
CDuiFrameWnd::~CDuiFrameWnd() {

}
          
void  CDuiFrameWnd::InitWindow() {
	m_lstClasses = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("lstClasses"));
	m_layDesks   = static_cast<DuiLib::CTileLayoutUI*>(m_PaintManager.FindControl("layDesks"));
	m_layRows    = static_cast<DuiLib::CVerticalLayoutUI*>(m_PaintManager.FindControl("layRows"));
	m_layCols    = static_cast<DuiLib::CTileLayoutUI*>(m_PaintManager.FindControl("layCols"));

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

	DWORD  dwCnt = g_data.m_dwRoomRows * g_data.m_dwRoomCols;
	for (DWORD i = 0; i < dwCnt; i++) {
		CDeskUI * pDesk = new CDeskUI;
		m_layDesks->Add(pDesk);   
	}

//#if TEST_FLAG
//	CListTextElementUI * pItem = new CListTextElementUI;
//	m_lstClasses->Add(pItem);
//	pItem->SetText(0, "六年级1班"); 
//	pItem = new CListTextElementUI;
//	m_lstClasses->Add(pItem);
//	pItem->SetText(0, "六年级2班"); 
//#endif

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
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
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


