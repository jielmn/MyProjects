#include "BindingDlg.h"

CBindingDlg::CBindingDlg() {

}

void   CBindingDlg::Notify(DuiLib::TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}

void   CBindingDlg::InitWindow() {
	WindowImplBase::InitWindow();
} 

LRESULT  CBindingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CBindingDlg::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}