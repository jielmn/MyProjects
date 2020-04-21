#include "CustomControls.h"

const int CTempItemUI::WIDTH = 220;
const int CTempItemUI::HEIGHT = 24;                       

CTempItemUI::CTempItemUI() : m_callback(m_pManager) {
	m_opBasePoint = 0;
	m_lblTemp = 0;
	m_edDutyCycle = 0;
	m_btnUp = 0;
	m_btnDown = 0;
	m_btnAdjust = 0;

	m_nTemp = 0;
	m_nDutyCycle = 0;
	m_bHighlight = FALSE;
	m_bChecked = FALSE;
}

CTempItemUI::~CTempItemUI() {
	if ( m_pManager )
		m_pManager->RemoveNotifier(this);
}

LPCTSTR  CTempItemUI::GetClass()  const {
	return "TempItem";
}

void  CTempItemUI::DoInit() {
	m_pManager->AddNotifier(this);

	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(_T("TempItemUI.xml"), (UINT)0, &m_callback, m_pManager));
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
	m_edDutyCycle = static_cast<CEditUI*>(m_pManager->FindControl("edDutyCycle"));
	m_btnUp = static_cast<CMyButtonUI*>(m_pManager->FindControl("btnUp"));
	m_btnDown = static_cast<CMyButtonUI*>(m_pManager->FindControl("btnDown"));
	m_btnAdjust = static_cast<CButtonUI*>(m_pManager->FindControl("btnAdjust")); 

	SetTemp();
	SetDutyCycle();
	SetChecked();

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
	strText.Format("%.1f¡æ", m_nTemp / 100.0); 
	m_lblTemp->SetText(strText);
}

void  CTempItemUI::SetDutyCycle() {
	if ( 0 == m_edDutyCycle)
		return;

	CDuiString  strText;
	strText.Format("%d", m_nDutyCycle);
	m_edDutyCycle->SetText(strText);
}

BOOL  CTempItemUI::IsChecked() const {
	return m_bChecked;
}

void  CTempItemUI::SetChecked(BOOL bChecked) {
	m_bChecked = bChecked;
	SetChecked();
}

void  CTempItemUI::SetChecked() {
	if (0 == m_opBasePoint)
		return;
	if (m_bChecked)
		m_opBasePoint->Selected(true);
	else
		m_opBasePoint->Selected(false);
}

void  CTempItemUI::SetHighlight(BOOL bHighlight) {
	if (bHighlight) {
		if (!m_bHighlight) {
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
	else {
		if (m_bHighlight) {
			m_btnDown->SetVisible(false);
			m_btnUp->SetVisible(false);
			m_btnAdjust->SetVisible(false);
			this->SetBkColor(0x00000000);
			m_bHighlight = FALSE;
		}
	}
}

void CTempItemUI::DoEvent(DuiLib::TEventUI& event) {
	if ( event.Type == UIEVENT_MOUSEENTER ) {
		if ( !m_bHighlight ) {
			m_pManager->SendNotify(this, "myselected");
		}
		//SetHighlight(TRUE);		
	}
	else if ( event.Type == UIEVENT_MOUSELEAVE ) {
		// SetHighlight(FALSE);
		if (m_bHighlight) {
			m_pManager->SendNotify(this, "myunselected");
		}
	}	
	CContainerUI::DoEvent(event);
}

void CTempItemUI::SetSelected() {
	bool bSelected = !m_opBasePoint->IsSelected();

	if ( bSelected ) {
		m_opBasePoint->Selected(true);
	}
	else {
		m_opBasePoint->Selected(false);
	}
}

void  CTempItemUI::Upper() {
	if ( !m_btnUp->IsVisible() ) {
		return;
	}

	int nDutyCycle = GetDutyCycle();
	nDutyCycle++;
	SetDutyCycle(nDutyCycle);
}

void  CTempItemUI::Downer() {
	if (!m_btnDown->IsVisible()) {
		return;
	}

	int nDutyCycle = GetDutyCycle();
	nDutyCycle--;
	SetDutyCycle(nDutyCycle);
}

void CTempItemUI::Notify(TNotifyUI& msg) {
	if ( msg.sType == "selectchanged" ) {
		if (msg.pSender == m_opBasePoint) {
			bool bSelected = m_opBasePoint->IsSelected();
			if (bSelected) {
				m_lblTemp->SetTextColor(0xFFFF0000);
				m_edDutyCycle->SetTextColor(0xFFFF0000);
				m_lblTemp->SetFont(3);
				m_edDutyCycle->SetFont(3);
				m_edDutyCycle->SetEnabled(true);
				if (m_bHighlight) {
					m_btnUp->SetVisible(true);
					m_btnDown->SetVisible(true);
				}
			}
			else {
				m_lblTemp->SetTextColor(0xFF386382);
				m_edDutyCycle->SetTextColor(0xFF386382);
				m_lblTemp->SetFont(2);
				m_edDutyCycle->SetFont(2);
				m_edDutyCycle->SetEnabled(false);
				if (m_bHighlight) {
					m_btnUp->SetVisible(false);
					m_btnDown->SetVisible(false);
				}
			}	
			m_bChecked = bSelected ? TRUE : FALSE;
		}
	}
	else if (msg.sType == "click") {
		if (msg.pSender == m_btnUp) {
			int nDutyCycle = GetDutyCycle();
			nDutyCycle++;
			SetDutyCycle(nDutyCycle);
		}
		else if (msg.pSender == m_btnDown) {
			int nDutyCycle = GetDutyCycle();
			nDutyCycle--;
			SetDutyCycle(nDutyCycle);
		}
		else if (msg.pSender == m_btnAdjust) {
			m_pManager->SendNotify(this, "adjust");     
		}
	}
	else if (msg.sType == "killfocus") {
		if (msg.pSender == m_edDutyCycle) {
			CDuiString strText = m_edDutyCycle->GetText();
			int nDutyCycle = 0;
			int ret = sscanf(strText, "%d", &nDutyCycle);
			if ( 1 == ret )
				SetDutyCycle(nDutyCycle);
		}
	}
}


LPCTSTR CMyButtonUI::GetClass() const {
	return "MyButton";
}

void CMyButtonUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_BUTTONDOWN) {
		//JTelSvrPrint("set timer 10");
		m_pManager->SetTimer(this, 10, 1000);
	}
	else if (event.Type == UIEVENT_BUTTONUP) {
		//JTelSvrPrint("kill timer 10 and 11");
		m_pManager->KillTimer(this, 10);
		m_pManager->KillTimer(this, 11);
	}
	else if (event.Type == UIEVENT_TIMER) {
		if (event.wParam == 10) {
			m_pManager->SendNotify(this, "click");
			m_pManager->KillTimer(this, 10);
			//JTelSvrPrint("set timer 11");
			m_pManager->SetTimer(this, 11, 50);
		}
		else if (event.wParam == 11) {
			//JTelSvrPrint("on timer 11");
			m_pManager->SendNotify(this, "click");
		}
		
	}
	CButtonUI::DoEvent(event);
}