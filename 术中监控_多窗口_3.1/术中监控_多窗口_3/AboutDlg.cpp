#include "AboutDlg.h"

CAboutDlg::CAboutDlg() {

}

void   CAboutDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::WindowImplBase::Notify(msg);
}

void   CAboutDlg::InitWindow() {
	DuiLib::WindowImplBase::InitWindow();
}

LRESULT  CAboutDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}