#include "ComDataUI.h"

CComDataUI::CComDataUI() {

}

CComDataUI::~CComDataUI() {

}

LPCTSTR CComDataUI::GetClass() const {
	return "ComData";
}

void CComDataUI::DoInit() {
	CContainerUI::DoInit();
}

void CComDataUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}