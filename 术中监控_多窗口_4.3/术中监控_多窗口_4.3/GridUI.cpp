#include "GridUI.h"

CGridUI::CGridUI() :m_callback(m_pManager) {
	m_btnBedNo = 0;
	m_dwBedNo = 0;

	m_lblReaderNo = 0;
	m_dwReaderNo = 0;
}

CGridUI::~CGridUI() {

}

LPCTSTR CGridUI::GetClass() const {
	return "GridUI";
}

void CGridUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(GRID_XML_FILE, (UINT)0, &m_callback, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
		return;
	}

	CDuiString  strText;

	m_btnBedNo = static_cast<DuiLib::CButtonUI *>(m_pManager->FindControl(BTN_BED_NO));
	strText.Format("%02u", m_dwBedNo);
	m_btnBedNo->SetText(strText);

	m_lblReaderNo = static_cast<DuiLib::CLabelUI *>(m_pManager->FindControl(LBL_READER_NO));
	strText.Format("%c", ('A' + (char)m_dwReaderNo) );
	m_lblReaderNo->SetText(strText);
}

void CGridUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void  CGridUI::SetBedNo(DWORD dwIndex) {
	m_dwBedNo = dwIndex;

	if (m_btnBedNo) {
		CDuiString  strText;
		strText.Format("%02u", m_dwBedNo);
		m_btnBedNo->SetText(strText);
	}
}