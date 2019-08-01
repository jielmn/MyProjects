#include "ModeButtonUI.h"


CModeButton::CModeButton() {
	m_mode = Mode_Hand;
}

CModeButton::~CModeButton() {

}

void CModeButton::DoEvent(DuiLib::TEventUI& event) {
	if ( event.Type == UIEVENT_BUTTONDOWN ) {
		SwitchMode();
	}
	else if (event.Type == UIEVENT_DBLCLICK) {
		SwitchMode();
	}
	CButtonUI::DoEvent(event); 
}

LPCTSTR CModeButton::GetClass() const {
	return "ModeButton";
}

void CModeButton::SwitchMode() {
	switch (m_mode)
	{
	case CModeButton::Mode_Hand:
		m_mode = Mode_Single;
		this->SetBkImage("file='single.png' dest='2,2,28,28'");
		break;
	case CModeButton::Mode_Single:
		m_mode = Mode_Multiple;
		this->SetBkImage("file='multiple.png' dest='2,2,28,28'");
		break;
	case CModeButton::Mode_Multiple:
		m_mode = Mode_Hand;
		this->SetBkImage("file='hand.png' dest='2,2,28,28'");
		break;
	default:
		break;
	}

	m_pManager->SendNotify(this, "ModeChanged");;
}

CModeButton::Mode CModeButton::GetMode() const {
	return m_mode;
}

void CModeButton::SetMode(CModeButton::Mode e) {
	m_mode = e;

	switch (m_mode)
	{
	case CModeButton::Mode_Hand:
		this->SetBkImage("file='hand.png' dest='2,2,28,28'");
		break;
	case CModeButton::Mode_Single:
		this->SetBkImage("file='single.png' dest='2,2,28,28'");
		break;
	case CModeButton::Mode_Multiple:
		this->SetBkImage("file='multiple.png' dest='2,2,28,28'");
		break;
	default:
		break;
	}
}