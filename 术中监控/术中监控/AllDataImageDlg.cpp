#include "AllDataImageDlg.h"
#include "ImageAll.h"


CAllDataImageDlg::CAllDataImageDlg() {

}

void   CAllDataImageDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::WindowImplBase::Notify(msg);
}

DuiLib::CControlUI * CAllDataImageDlg::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp("AllDataImage", pstrClass)) {
		return new CImageAll(&m_PaintManager);    
	}
	  
	return DuiLib::WindowImplBase::CreateControl(pstrClass);
}

void   CAllDataImageDlg::InitWindow() {
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	DuiLib::WindowImplBase::InitWindow();
}

LRESULT  CAllDataImageDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

