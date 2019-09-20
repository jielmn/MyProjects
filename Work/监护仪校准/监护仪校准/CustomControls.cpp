#include "CustomControls.h"

const int CTempItemUI::WIDTH = 210;
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
	m_bHighlight = FALSE;
}

CTempItemUI::~CTempItemUI() {
	m_pManager->RemoveNotifier(this);
}

LPCTSTR  CTempItemUI::GetClass()  const {
	return "TempItem";
}

void  CTempItemUI::DoInit() {
	m_pManager->AddNotifier(this);

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

	m_btnDown->SetVisible(false);
	m_btnUp->SetVisible(false);
	m_btnAdjust->SetVisible(false);
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
	strText.Format("%.1f��", m_nTemp / 100.0); 
	m_lblTemp->SetText(strText);
}

void  CTempItemUI::SetDutyCycle() {
	if ( 0 == m_lblDutyCycle)
		return;

	CDuiString  strText;
	strText.Format("%d", m_nDutyCycle);
	m_lblDutyCycle->SetText(strText);
}

void CTempItemUI::DoEvent(DuiLib::TEventUI& event) {
	if ( event.Type == UIEVENT_MOUSEENTER ) {
		if ( !m_bHighlight ) {
			bool bSelected = m_opBasePoint->IsSelected();
			if (bSelected) {
				m_btnDown->SetVisible(true);
				m_btnUp->SetVisible(true);
			}
			m_btnAdjust->SetVisible(true);
			this->SetBkColor(0x30000000);
			m_bHighlight = TRUE;
		}		
	}
	else if ( event.Type == UIEVENT_MOUSELEAVE ) {
		if (m_bHighlight) {
			m_btnDown->SetVisible(false);
			m_btnUp->SetVisible(false);
			m_btnAdjust->SetVisible(false);
			this->SetBkColor(0x00000000);
			m_bHighlight = FALSE;
		}		
	}
	CContainerUI::DoEvent(event);
}

void CTempItemUI::Notify(TNotifyUI& msg) {
	if ( msg.sType == "selectchanged" ) {
		if (msg.pSender == m_opBasePoint) {
			bool bSelected = m_opBasePoint->IsSelected();
			if (bSelected) {
				m_lblTemp->SetTextColor(0xFFFF0000);
				m_lblDutyCycle->SetTextColor(0xFFFF0000);
				m_lblTemp->SetFont(3);
				m_lblDutyCycle->SetFont(3);
				if (m_bHighlight) {
					m_btnUp->SetVisible(true);
					m_btnDown->SetVisible(true);
				}
			}
			else {
				m_lblTemp->SetTextColor(0xFF386382);
				m_lblDutyCycle->SetTextColor(0xFF386382);
				m_lblTemp->SetFont(2);
				m_lblDutyCycle->SetFont(2);
				if (m_bHighlight) {
					m_btnUp->SetVisible(false);
					m_btnDown->SetVisible(false);
				}
			}			
		}
	}
}