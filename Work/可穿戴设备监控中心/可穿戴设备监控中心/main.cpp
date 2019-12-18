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

#define   GRID_WIDTH      160
#define   GRID_HEIGHT     160                                

CDuiFrameWnd::CDuiFrameWnd() {
	m_layGrids = 0;
	m_layList = 0;
	m_btnExpand = 0;
	m_btnMenu = 0;
	m_layGridsPages = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}
   
void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = m_hWnd;
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	m_layGrids = (DuiLib::CTileLayoutUI *)m_PaintManager.FindControl("layGrids");
	m_layList = (DuiLib::CVerticalLayoutUI *)m_PaintManager.FindControl("layList");
	m_btnExpand = (DuiLib::CButtonUI *)m_PaintManager.FindControl("btnExpand");
	m_btnMenu = (DuiLib::CButtonUI *)m_PaintManager.FindControl("menubtn");
	m_layGridsPages = (DuiLib::CHorizontalLayoutUI *)m_PaintManager.FindControl("layGridsPages");

	m_layList->SetVisible(false);
	m_btnExpand->SetText("<"); 
	m_btnExpand->SetTag(FALSE);

	for (int i = 0; i < 300; i++) {
		CGridUI * p = new CGridUI;
		m_layGrids->Add(p); 
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
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
	pMenu->Init(*this, pt);
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

	return true;   
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


