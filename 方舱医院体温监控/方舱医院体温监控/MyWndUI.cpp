#include "MyWndUI.h"

CMyWndUI::CMyWndUI() {
	m_hWnd = 0;

	this->OnSize += MakeDelegate(this, &CMyWndUI::OnMySize);
}

CMyWndUI::~CMyWndUI() {
	if (m_hWnd) {
		DestroyWindow(m_hWnd);
	}
}

void CMyWndUI::DoInit() {
	m_hWnd = CreateWindow(_T("STATIC"), _T(""), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 
		0, 0, 0, 0, m_pManager->GetPaintWindow(), NULL, NULL, NULL);
	EnableWindow(m_hWnd, FALSE);
}

bool CMyWndUI::OnMySize(void * pParam) {
	if (m_hWnd) {
		int width = m_rcItem.right - m_rcItem.left;
		int height = m_rcItem.bottom - m_rcItem.top;
		::MoveWindow(m_hWnd, m_rcItem.left, m_rcItem.top, width, height, TRUE);
	}	
	return true;
}

void CMyWndUI::DoEvent(DuiLib::TEventUI& event) {
	if ( event.Type == UIEVENT_MOUSEENTER ) {
		OutputDebugString("== mouse enter! \n");
	}
	else if (event.Type == UIEVENT_MOUSELEAVE) {
		OutputDebugString("== mouse leave!!! \n");
	}
	CControlUI::DoEvent(event);
}