// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "LmnTelSvr.h"


CControlUI*  CDialogBuilderCallbackEx::CreateControl(LPCTSTR pstrClass) {
	if ( 0 == strcmp(pstrClass, MYIMAGE_CLASS_NAME) ) {
		return new CMyImageUI(m_pManager);
	}
	return NULL;
}


CDuiFrameWnd::CDuiFrameWnd() : m_callback(&m_PaintManager) {
	memset( m_pGrids, 0, sizeof(m_pGrids) );
	memset(m_pLblIndexes_small, 0, sizeof(m_pLblIndexes_small));
	memset(m_pLblBed_small, 0, sizeof(m_pLblBed_small));
	memset(m_pLblName_small, 0, sizeof(m_pLblName_small));
	memset(m_pLblCurTemp_small, 0, sizeof(m_pLblCurTemp_small));	
	memset(m_pLblBedTitle_small, 0, sizeof(m_pLblBedTitle_small));
	memset(m_pLblNameTitle_small, 0, sizeof(m_pLblNameTitle_small));
	memset(m_pLblCurTempTitle_small, 0, sizeof(m_pLblCurTempTitle_small));
	memset(m_pMyImage, 0, sizeof(m_pMyImage));
	
}

void  CDuiFrameWnd::InitWindow() {
	CDuiString strText;

	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	
	m_layWindow = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(LAYOUT_WINDOW_NAME));
	m_layMain = static_cast<CTileLayoutUI*>(m_PaintManager.FindControl(LAYOUT_MAIN_NAME));
	m_layMain->SetFixedColumns(g_dwLayoutColumns);
	
	for (DWORD i = 0; i < g_dwLayoutRows; i++) {
		for (DWORD j = 0; j < g_dwLayoutColumns; j++) {
			CDialogBuilder builder;  
			int nIndex = i*g_dwLayoutColumns + j;
			m_pGrids[nIndex] = builder.Create(MYCHART_XML_FILE, (UINT)0, &m_callback, &m_PaintManager);
			m_pGrids[nIndex]->SetBorderSize(1);

			m_pLblIndexes_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_INDEX_SMALL_NAME, 0));
			if (m_pLblIndexes_small[nIndex]) {
				strText.Format("%lu", i * g_dwLayoutColumns + j + 1);
				m_pLblIndexes_small[nIndex]->SetText(strText);
			}

			m_pLblBed_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_BED_SMALL_NAME, 0));
			m_pLblName_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_NAME_SMALL_NAME, 0));
			m_pLblCurTemp_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_CUR_TEMP_SMALL_NAME, 0));

			m_pLblBedTitle_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_BED_TITLE_SMALL_NAME, 0));
			m_pLblNameTitle_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_NAME_TITLE_SMALL_NAME, 0));
			m_pLblCurTempTitle_small[nIndex] = static_cast<CLabelUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, LABEL_CUR_TEMP_TITLE_SMALL_NAME, 0));

			m_pMyImage[nIndex] = static_cast<CMyImageUI*>(m_pGrids[nIndex]->FindControl(MY_FINDCONTROLPROC, MYIMAGE_NAME, 0));
			m_pMyImage[nIndex]->SetIndex(nIndex);

			m_layMain->Add(m_pGrids[nIndex]); 
		}
	}

	m_layStatus = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(LAYOUT_STATUS_NAME));
	
	OnChangeSkin();
	WindowImplBase::InitWindow();    
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == WM_SIZE ) {
		OnSize(wParam, lParam);		 
	}
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

// main frame events
void  CDuiFrameWnd::OnSize(WPARAM wParam, LPARAM lParam) {
	DWORD dwWidth  = LOWORD(lParam) - 10 * 2;
	DWORD dwHeight = HIWORD(lParam) - 30 - 1 * 2 - 32;

	if (m_layMain) {
		SIZE s;
		s.cx = ( dwWidth - 1 ) / g_dwLayoutColumns + 1;
		s.cy = ( dwHeight - 1 ) / g_dwLayoutRows + 1;
		m_layMain->SetItemSize(s);

		m_layStatus->SetFixedHeight( 30 - ( s.cy * g_dwLayoutRows - dwHeight ) );
	}
}

void   CDuiFrameWnd::OnChangeSkin() {
	m_layMain->SetBkColor(g_skin[LAYOUT_MAIN_BK_COLOR_INDEX]);
	m_layStatus->SetBkColor(g_skin[LABEL_STATUS_BK_COLOR_INDEX]);

	for (DWORD i = 0; i < g_dwLayoutRows; i++) {
		for (DWORD j = 0; j < g_dwLayoutColumns; j++) {
			int nIndex = i*g_dwLayoutColumns + j;
			m_pGrids[nIndex]->SetBorderColor(g_skin[GRID_BORDER_COLOR_INDEX]);
			m_pLblIndexes_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);

			m_pLblBed_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
			m_pLblName_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
			m_pLblCurTemp_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
			m_pLblBedTitle_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
			m_pLblNameTitle_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
			m_pLblCurTempTitle_small[nIndex]->SetTextColor(g_skin[COMMON_TEXT_COLOR_INDEX]);
		}
	}
}
  
 

 
 
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HANDLE handle = ::CreateMutex(NULL, FALSE, GLOBAL_LOCK_NAME);//handle为声明的HANDLE类型的全局变量 
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		HWND hWnd = ::FindWindow(MAIN_FRAME_NAME, 0);
		::ShowWindow(hWnd, SW_SHOWMAXIMIZED);
		::InvalidateRect(hWnd, 0, TRUE);
		return 0;
	}

	int ret = 0;
	LmnToolkits::ThreadManager::GetInstance();
	ret = CBusiness::GetInstance()->Init();
	if (0 != ret) {
		goto LABEL_ERROR;
	}

	g_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

	DWORD  dwPort = 0;
	g_cfg->GetConfig(CFG_TELNET_PORT, dwPort, 1135);
	JTelSvrStart((unsigned short)dwPort, 10);

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

	g_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

LABEL_ERROR:
	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}

         
             