// win32_1.cpp : 定义应用程序的入口点。
//
#include <afx.h>
#include "login.h"
#include "main.h"
#include "resource.h"
#include "Business.h"


void  CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_lblDbStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl("lblDatabaseStatus");
	m_lblReaderStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl("lblReaderStatus");
	m_lblUser = (DuiLib::CLabelUI *)m_PaintManager.FindControl("lblUser");
	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
	

	CInvDatabase::DATABASE_STATUS eDbStatus     = CBusiness::GetInstance()->GetDbStatus();
	CInvReader::READER_STATUS     eReaderStatus = CBusiness::GetInstance()->GetReaderStatus();

	if (eDbStatus == CInvDatabase::STATUS_OPEN) {
		SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_OK_TEXT);
	}
	else {
		SET_CONTROL_TEXT(m_lblDbStatus, DB_STATUS_CLOSE_TEXT);
	}

	if (eReaderStatus == CInvReader::STATUS_OPEN) {
		SET_CONTROL_TEXT(m_lblReaderStatus, READER_STATUS_OK_TEXT);
	}
	else {
		SET_CONTROL_TEXT(m_lblReaderStatus, READER_STATUS_CLOSE_TEXT);
	}

	CString strText;
	strText = "欢迎您，";
	SET_CONTROL_TEXT(m_lblUser, strText + CBusiness::GetInstance()->GetUserName() );

	WindowImplBase::InitWindow();
}

DuiLib::CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp("Inventory_small", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("Inventory_small.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("Inventory_big", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("Inventory_big.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	} else if (0 == strcmp("Query", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("Query.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	return 0;
}

void CDuiFrameWnd::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();

	if (msg.sType == _T("selectchanged"))
	{
		if (0 == m_tabs) {
			return;
		}

		if (name == _T("Inventory_small")) {
			m_tabs->SelectItem(0);
		} else if (name == _T("Inventory_big")) {
			m_tabs->SelectItem(1);			
		} else if (name == _T("Query")) {
			m_tabs->SelectItem(2);
		}
	}
	WindowImplBase::Notify(msg);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	int ret = 0;
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->sigInit.emit(&ret);

	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	
	CLoginWnd loginFrame;
	loginFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	loginFrame.CenterWindow();
	g_hWnd = loginFrame.GetHWND();
	::SendMessage(loginFrame.GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(loginFrame.GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	// 连接数据库
	CBusiness::GetInstance()->ReconnectDatabaseAsyn();
	// 连接Reader
	CBusiness::GetInstance()->ReconnectReaderAsyn();

	// 显示登录界面
	loginFrame.ShowModal();

	// 如果登录成功
	if ( CBusiness::GetInstance()->IfLogined() ) {
		// 主界面	
		CDuiFrameWnd duiFrame;
		duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		duiFrame.CenterWindow();
		g_hWnd = duiFrame.GetHWND();
		::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		duiFrame.ShowModal();
	}

	CBusiness::GetInstance()->sigDeinit.emit(&ret);
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}


