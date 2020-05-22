#include "MyControls.h"

// 348,212

int CDeviceUI::FIXED_WIDTH = 338;
int CDeviceUI::FIXED_HEIGHT = 212;

CDeviceUI::CDeviceUI() {

}

CDeviceUI::~CDeviceUI() {
	m_pManager->RemoveNotifier(this);                          
}

LPCTSTR CDeviceUI::GetClass() const {
	return "Device";
}

void CDeviceUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(_T("DeviceUI.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
		m_pManager->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		return;
	}

	this->SetFixedWidth(FIXED_WIDTH);
	this->SetFixedHeight(FIXED_HEIGHT);
}

void CDeviceUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CDeviceUI::DoEvent(DuiLib::TEventUI& event) {
	CContainerUI::DoEvent(event);
}

void CDeviceUI::Notify(TNotifyUI& msg) {

}