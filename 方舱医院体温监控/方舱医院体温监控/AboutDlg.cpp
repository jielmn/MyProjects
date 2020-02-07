#include "AboutDlg.h"

CAboutDlg::CAboutDlg() {
	m_lblVersion = 0;
}

void   CAboutDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::WindowImplBase::Notify(msg);
}

void   CAboutDlg::InitWindow() {
	CDuiString  strText;

	m_lblVersion = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(LBL_VERSION));
	strText.Format("°æ±¾ºÅ£º%s(%s)", VERSION, COMPILE_TIME);
	m_lblVersion->SetText(strText);

	DuiLib::WindowImplBase::InitWindow(); 
}

LRESULT  CAboutDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}