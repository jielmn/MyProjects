#include "BindingDlg.h"

CBindingDlg::CBindingDlg() {
	memset(m_tTagItem, 0, size(m_tTagItem));
	m_dwPatientId = 0;
	m_dwIndex = 0;
	m_dwTagItemCnt = 0;
}

void   CBindingDlg::Notify(DuiLib::TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}

void   CBindingDlg::InitWindow() {
	m_lstTags = static_cast<CListUI *>(m_PaintManager.FindControl("lstTags"));

	for ( DWORD i = 0; i < m_dwTagItemCnt; i++ ) {

	}
	WindowImplBase::InitWindow();         
} 

LRESULT  CBindingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CBindingDlg::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}