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
	 
}

CDuiFrameWnd::~CDuiFrameWnd() {
	 
}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = m_hWnd;
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_layMain = static_cast<DuiLib::CTileLayoutUI*>(m_PaintManager.FindControl("layGrids"));
	m_lblStatus = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblDatabaseStatus"));
	m_lblStatus->SetText("没有打开串口");

	// 添加窗格
	for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
		m_grids[i] = new CGridUI;
		m_grids[i]->SetBorderSize(4);
		m_grids[i]->SetBorderColor(0xFF434248); 
		m_grids[i]->SetName("grid");
		m_grids[i]->SetTag(i);
		m_layMain->Add(m_grids[i]);    
	}

	m_layMain->OnSize += MakeDelegate(this, &CDuiFrameWnd::OnMainSize);

	CBusiness::GetInstance()->OpenComAsyn();
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == UM_COM_STATUS) {
		BOOL bRet = wParam;
		CDuiString strText;
		if (bRet) {
			strText.Format("打开串口com%d成功", g_data.m_nComPort);
			m_lblStatus->SetText(strText);
		}
		else {
			strText.Format("打开串口com%d失败", g_data.m_nComPort);
			m_lblStatus->SetText(strText);
		}
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

bool  CDuiFrameWnd::OnMainSize(void * pParam) {
	SIZE s;
	RECT rect = m_layMain->GetPos();
	int width  = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	s.cx = (width - 1) / 2 + 1;
	s.cy = (height - 1) / 2 + 1;
	m_layMain->SetItemSize(s);
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


