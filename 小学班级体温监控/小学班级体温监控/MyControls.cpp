#include "MyControls.h"
#include <time.h>

CDeskUI::CDeskUI() {
	m_Sex = 0;
	m_lblName = 0;
	m_lblTemp = 0;
	m_lblDate = 0;
	m_lblTime = 0;
	m_Warning = 0;
	m_btnDel = 0;
	m_total = 0;

	memset(&m_data, 0, sizeof(m_data));
	m_bHighlight = FALSE;
}

CDeskUI::~CDeskUI() {
	m_pManager->RemoveNotifier(this);
}

LPCTSTR CDeskUI::GetClass() const {
	return "Desk";
}

void CDeskUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(_T("Desk.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
		m_pManager->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		return;
	}

	m_Sex = m_pManager->FindControl("ctlSex");
	m_lblName = static_cast<DuiLib::CLabelUI*>(m_pManager->FindControl("lblName"));
	m_lblTemp = static_cast<DuiLib::CLabelUI*>(m_pManager->FindControl("lblTemp"));
	m_lblDate = static_cast<DuiLib::CLabelUI*>(m_pManager->FindControl("lblDate"));
	m_lblTime = static_cast<DuiLib::CLabelUI*>(m_pManager->FindControl("lblTime"));
	m_Warning = m_pManager->FindControl("ctlWarning");
	m_btnDel = static_cast<DuiLib::CButtonUI*>(m_pManager->FindControl("btnDelDesk"));
	m_total = m_pManager->FindControl("layItem");

	UpdateUI();
}

void CDeskUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CDeskUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_MOUSEENTER) {
		if (!m_bHighlight) {
			m_pManager->SendNotify(this, "myselected");
		}
	}
	else if (event.Type == UIEVENT_MOUSELEAVE) {
		if (m_bHighlight) {
			m_pManager->SendNotify(this, "myunselected");
		}
	}
	else if (event.Type == UIEVENT_DBLCLICK) {
		m_pManager->SendNotify(this, "mydbclick");
	}
	CContainerUI::DoEvent(event);
}

void CDeskUI::Notify(TNotifyUI& msg) {
	if ( msg.sType == "click" ) {
		if (msg.pSender == m_btnDel) {
			m_pManager->SendNotify(this, "emptydesk");
		}
	}
}

void CDeskUI::UpdateUI() {
	if (!m_bInitiated)
		return;

	CDuiString  strText;
	char szBuf[256];
	time_t now = time(0);

	if (m_data.bValid) {
		if (m_data.nSex == 1) {
			m_Sex->SetBkImage("boy.png");
		}
		else {
			m_Sex->SetBkImage("girl.png");
		}
		
		m_lblName->SetText(m_data.szName);

		if (m_data.nTemp > 0) {
			strText.Format("%.2f¡æ", m_data.nTemp / 100.0f);
			m_lblTemp->SetText(strText);
		}
		else {
			m_lblTemp->SetText("");
		}

		if ( m_data.time > 0 ) {
			LmnFormatTime(szBuf, sizeof(szBuf), m_data.time, "%Y-%m-%d");
			m_lblDate->SetText(szBuf);
			LmnFormatTime(szBuf, sizeof(szBuf), m_data.time, "%H:%M");
			m_lblTime->SetText(szBuf);
		}
		else {
			m_lblDate->SetText("");
			m_lblTime->SetText("");
		}

		if ( m_data.time > 0 && now > m_data.time ) {
			time_t diff = now - m_data.time;
			if ( diff >= WARNING_TIME_ELAPSED ) {
				m_Warning->SetVisible(true);
			}
			else {
				m_Warning->SetVisible(false);
			}
		}
		else {
			m_Warning->SetVisible(false);
		}

		m_btnDel->SetVisible(false);
	}
	else {
		m_Sex->SetBkImage("");
		m_lblName->SetText("");
		m_lblTemp->SetText("");
		m_lblDate->SetText("");
		m_lblTime->SetText("");
		m_Warning->SetVisible(false);
		m_btnDel->SetVisible(false);
	}
}

void  CDeskUI::SetHighlight(BOOL bHighlight) {
	if (bHighlight) {
		if (!m_bHighlight) {
			this->SetBkColor(0x30000000); 
			if (m_data.bValid) {
				m_btnDel->SetVisible(true); 
			}			
			m_bHighlight = TRUE;
		}
	}
	else {
		if (m_bHighlight) {
			this->SetBkColor(0x00000000);
			m_btnDel->SetVisible(false);
			m_bHighlight = FALSE;
		}
	}
}

BOOL  CDeskUI::IsHighlight() {
	return m_bHighlight;
}

void  CDeskUI::SetValid(BOOL bValid) {
	m_data.bValid = bValid;
	if ( !bValid ) {
		memset(&m_data, 0, sizeof(m_data));
	}
}

// ¸ßÁÁ±ß¿ò
void  CDeskUI::SetHighlightBorder(BOOL bHighlight) {
	if (bHighlight) {
		m_total->SetBorderColor(0xFFCAF100);
	}
	else {
		m_total->SetBorderColor(0xFF000000); 
	}
}