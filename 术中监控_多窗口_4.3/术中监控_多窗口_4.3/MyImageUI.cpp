#include "MyImageUI.h"

CMyImageUI::CMyImageUI() {

}

CMyImageUI::~CMyImageUI() {

}

bool CMyImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);
	return true;
}

void CMyImageUI::DoEvent(DuiLib::TEventUI& event) {
	CControlUI::DoEvent(event);
}

LPCTSTR CMyImageUI::GetClass() const {
	return "MyImage";
}