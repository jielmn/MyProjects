#include "CustomControls.h"

const int CTempItemUI::WIDTH = 100;
const int CTempItemUI::HEIGHT = 30;

CTempItemUI::CTempItemUI() {

}

CTempItemUI::~CTempItemUI() {

}

LPCTSTR  CTempItemUI::GetClass()  const {
	return "TempItem";
}

void  CTempItemUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(_T("TempItemUI.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}

	this->SetFixedWidth(WIDTH);
	this->SetFixedHeight(HEIGHT);
}