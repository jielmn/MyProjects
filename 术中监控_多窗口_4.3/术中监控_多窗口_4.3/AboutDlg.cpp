#include "AboutDlg.h"

CAboutDlg::CAboutDlg() {
	m_lblVersion = 0;
}

void   CAboutDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::WindowImplBase::Notify(msg);
}

void   CAboutDlg::InitWindow() {
	CDuiString  strText;
	char buf[256];
	time_t now = time(0);
	DateTime2String(buf, sizeof(buf), &now );

	m_lblVersion = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl(LBL_VERSION));
	strText.Format("°æ±¾ºÅ£º%s(%s)", VERSION, buf);
	m_lblVersion->SetText(strText);

	DuiLib::WindowImplBase::InitWindow(); 
}

LRESULT  CAboutDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}