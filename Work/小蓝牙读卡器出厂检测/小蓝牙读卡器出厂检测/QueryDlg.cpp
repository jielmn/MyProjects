#include "QueryDlg.h"

CQueryDlg::CQueryDlg() {

}                
 
void  CQueryDlg::Notify(DuiLib::TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}

void  CQueryDlg::InitWindow() {
	WindowImplBase::InitWindow();
}

LRESULT  CQueryDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

CControlUI * CQueryDlg::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}