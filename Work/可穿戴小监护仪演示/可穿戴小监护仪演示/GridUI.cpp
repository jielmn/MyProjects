#include "GridUI.h"
#include "business.h"
#include "LmnExcel.h"

CGridUI::CGridUI() {
}

CGridUI::~CGridUI() {
}

LPCTSTR CGridUI::GetClass() const {
	return "GridUI";
}

void CGridUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create("grid.xml", (UINT)0, 0, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
		m_pManager->AddNotifier(this);
	}
	else {
		this->RemoveAll();                   
		return;
	}


}

void CGridUI::Notify(TNotifyUI& msg) {

}