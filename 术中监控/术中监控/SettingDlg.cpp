#include "SettingDlg.h"


CSettingDlg::CSettingDlg() {

}

void   CSettingDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::WindowImplBase::Notify(msg);
}

void   CSettingDlg::InitWindow() {
	DuiLib::WindowImplBase::InitWindow();
}

LRESULT  CSettingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}