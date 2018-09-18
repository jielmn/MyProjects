#include "AlarmImage.h"

CAlarmImageUI::CAlarmImageUI() {
	m_alarm = ALARM_OK;
	m_bSetBkImage = FALSE;
}

CAlarmImageUI::~CAlarmImageUI() {

}

bool CAlarmImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	return CControlUI::DoPaint(hDC, rcPaint, pStopControl);
}

void CAlarmImageUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == DuiLib::UIEVENT_TIMER && event.wParam == MYIMAGE_TIMER_ID) {

		if (m_bSetBkImage) {
			if (HIGH_TEMP == m_alarm) {
				this->SetBkImage("alarm_high_temp_2.png");
			}
			else if (LOW_TEMP == m_alarm) {
				this->SetBkImage("alarm_low_temp_2.png");
			}
			else if (DISCONNECTED == m_alarm) {
				this->SetBkImage("alarm_fail_2.png");
			}
			else if (CHILD_ALARM == m_alarm) {
				this->SetBkImage("alarm_fail_2.png");
			}
			else {
				assert(0);
				this->SetBkImage("");
			}
		}
		else {
			this->SetBkImage("");
		}

		// ������ǶϿ��澯 
		if (DISCONNECTED != m_alarm) {
			m_bSetBkImage = !m_bSetBkImage;
		}
	}

	CControlUI::DoEvent(event);
}

void  CAlarmImageUI::StartAlarm(CAlarmImageUI::ENUM_ALARM e) {
	assert(e != ALARM_OK);

	// ����͵�ǰAlarmֵ��ȣ����ô���
	if (e == m_alarm) {
		return;
	}

	m_alarm = e;
	m_bSetBkImage = TRUE;
	m_pManager->SetTimer(this, MYIMAGE_TIMER_ID, 500);
}

void  CAlarmImageUI::StopAlarm() {
	m_alarm = ALARM_OK;
	this->SetBkImage("");
	m_pManager->KillTimer(this, MYIMAGE_TIMER_ID);
}