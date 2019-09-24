#include "SaveAsDlg.h"

CSaveAsDlg::CSaveAsDlg() {

}

void  CSaveAsDlg::Notify(DuiLib::TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}

void  CSaveAsDlg::InitWindow() {
	WindowImplBase::InitWindow();
}

LRESULT  CSaveAsDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CSaveAsDlg::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);           
}