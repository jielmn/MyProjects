#pragma once

#include "common.h"

#define  MYIMAGE_TIMER_ID             30
class CAlarmImageUI : public DuiLib::CControlUI
{
public:
	enum ENUM_ALARM {
		ALARM_OK = 0,
		HIGH_TEMP,
		LOW_TEMP,
		DISCONNECTED,
		CHILD_ALARM,
	};

	CAlarmImageUI();
	~CAlarmImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);

	void  StartAlarm(ENUM_ALARM e);
	void  StopAlarm();

private:
	ENUM_ALARM     m_alarm;
	BOOL           m_bSetBkImage;                     // Í¼Æ¬½»Ìæ
};