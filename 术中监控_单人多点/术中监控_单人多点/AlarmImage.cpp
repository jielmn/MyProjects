#include "AlarmImage.h"

CAlarmImageUI::CAlarmImageUI(DuiLib::CPaintManagerUI *pManager) {
	m_pManager = pManager;
	m_bSetBkImage = FALSE;
	m_nBkImageIndex = -1;
}

CAlarmImageUI::~CAlarmImageUI() {

}

bool CAlarmImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	return CControlUI::DoPaint(hDC, rcPaint, pStopControl);
}

void CAlarmImageUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == DuiLib::UIEVENT_TIMER && event.wParam == MYIMAGE_TIMER_ID) {
		if (m_bSetBkImage) {
			if (0 == m_nBkImageIndex) {
				this->SetBkImage("alarm_high_temp_1.png");
			}
			else if (1 == m_nBkImageIndex) {
				this->SetBkImage("alarm_low_temp_1.png");
			}
			else if (2 == m_nBkImageIndex) {
				this->SetBkImage("alarm_fail_1.png");
			}
			else {
				assert(0);
				this->SetBkImage("");
			}
		}
		else {
			this->SetBkImage("");
		}

		// 如果不是FailureAlarm
		if (2 != m_nBkImageIndex) {
			m_bSetBkImage = !m_bSetBkImage;
		}

	}

	CControlUI::DoEvent(event);
}

void   CAlarmImageUI::HighTempAlarm() {
	if (0 != m_nBkImageIndex) {
		m_nBkImageIndex = 0;
		m_bSetBkImage = TRUE;
		m_pManager->SetTimer(this, MYIMAGE_TIMER_ID, 500);
	}
}

void   CAlarmImageUI::LowTempAlarm() {
	if (1 != m_nBkImageIndex) {
		m_nBkImageIndex = 1;
		m_bSetBkImage = TRUE;
		m_pManager->SetTimer(this, MYIMAGE_TIMER_ID, 500);
	}
}

void  CAlarmImageUI::FailureAlarm() {
	if (2 != m_nBkImageIndex) {
		m_nBkImageIndex = 2;
		m_bSetBkImage = TRUE;
		m_pManager->SetTimer(this, MYIMAGE_TIMER_ID, 500);
	}
}

void   CAlarmImageUI::StopAlarm() {
	if (-1 != m_nBkImageIndex) {
		m_nBkImageIndex = -1;
		this->SetBkImage("");
		m_pManager->KillTimer(this, MYIMAGE_TIMER_ID);
	}
}
