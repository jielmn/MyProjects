#include "CustomControls.h"

const int CTempItemUI::WIDTH = 200;
const int CTempItemUI::HEIGHT = 32;                       

CTempItemUI::CTempItemUI() {
	m_opBasePoint = 0;
	m_lblTemp = 0;
	m_lblDutyCycle = 0;
	m_btnUp = 0;
	m_btnDown = 0;
	m_btnAdjust = 0;

	m_nTemp = 0;
	m_nDutyCycle = 0;
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

	m_opBasePoint = static_cast<COptionUI*>(m_pManager->FindControl("optBasePoint"));
	m_lblTemp = static_cast<CLabelUI*>(m_pManager->FindControl("lblTemp"));
	m_lblDutyCycle = static_cast<CLabelUI*>(m_pManager->FindControl("lblDutyCycle"));
	m_btnUp = static_cast<CButtonUI*>(m_pManager->FindControl("btnUp"));
	m_btnDown = static_cast<CButtonUI*>(m_pManager->FindControl("btnDown"));
	m_btnAdjust = static_cast<CButtonUI*>(m_pManager->FindControl("btnAdjust"));

	SetTemp();
	SetDutyCycle();
}

void  CTempItemUI::SetTemp(int nTemp) {
	m_nTemp = nTemp;
	SetTemp();
}
 
int   CTempItemUI::GetTemp() const {
	return m_nTemp;
}

void  CTempItemUI::SetDutyCycle(int nDutyCycle) {
	m_nDutyCycle = nDutyCycle;
	SetDutyCycle();
}

int   CTempItemUI::GetDutyCycle() const {
	return m_nDutyCycle;
}

void  CTempItemUI::SetTemp() {
	if ( 0 == m_lblTemp)
		return;

	CDuiString  strText;
	strText.Format("%.2f", m_nTemp / 100.0);
	m_lblTemp->SetText(strText);
}

void  CTempItemUI::SetDutyCycle() {
	if ( 0 == m_lblDutyCycle)
		return;

	CDuiString  strText;
	strText.Format("%d", m_nDutyCycle);
	m_lblDutyCycle->SetText(strText);
}