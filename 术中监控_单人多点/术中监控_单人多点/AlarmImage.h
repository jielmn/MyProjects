#pragma once

#include "common.h"

#define  MYIMAGE_TIMER_ID             30
class CAlarmImageUI : public DuiLib::CControlUI
{
public:
	CAlarmImageUI(DuiLib::CPaintManagerUI *pManager);
	~CAlarmImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);

	void  HighTempAlarm();
	void  LowTempAlarm();
	void  FailureAlarm();
	void  StopAlarm();

private:
	DuiLib::CPaintManagerUI *    m_pManager;
	BOOL                         m_bSetBkImage;
	int                          m_nBkImageIndex;
};