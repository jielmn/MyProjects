#include <windows.h>
#include "main.h"
#include "resource.h"

void CDuiFrameWnd::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_lblDbStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl(DATABASE_STATUS_LABEL_ID);
	m_lblReaderStatus = (DuiLib::CLabelUI *)m_PaintManager.FindControl(READER_STATUS_LABEL_ID);

	WindowImplBase::InitWindow();
}

DuiLib::CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(TEMPERATURE_DATA_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(TEMPERATURE_DATA_FILE), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(PATIENTS_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(PATIENTS_FILE), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(NURSES_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(NURSES_FILE), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(SYNCHRONIZE_CONTROL_ID, pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T(SYNCHRONIZE_FILE), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	return 0;
}

void CDuiFrameWnd::Notify(DuiLib::TNotifyUI& msg) {
	if (msg.sType == _T("selectchanged"))
	{
		//if (0 == m_tabs) {
		//	return;
		//}

		//if (name == _T("Inventory_small")) {
		//	m_tabs->SelectItem(TABS_INDEX_INVENTORY_SMALL);
		//}
		//else if (name == _T("Inventory_big")) {
		//	m_tabs->SelectItem(TABS_INDEX_INVENTORY_BIG);
		//}
		//else if (name == _T("Query")) {
		//	m_tabs->SelectItem(TABS_INDEX_INVENTORY_QUERY);
		//}
		//else if (name = "Check") {
		//	m_tabs->SelectItem(TABS_INDEX_INVENTORY_CHECK);
		//	SET_CONTROL_TEXT(m_lblCheckTagId, "");
		//	SET_CONTROL_TEXT(m_lblCheckTagRet, "");
		//}
	}
	WindowImplBase::Notify(msg);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	DuiLib::CPaintManagerUI::SetInstance(hInstance);

	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	// 主界面	
	CDuiFrameWnd duiFrame;
	duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame.CenterWindow();

	// 设置icon
	::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame.GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	// show model
	duiFrame.ShowModal();

	return 0;
}