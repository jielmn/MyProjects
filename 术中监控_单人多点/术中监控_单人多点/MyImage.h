#pragma once

#include "common.h"

class CMyImageUI : public DuiLib::CControlUI
{
public:
	CMyImageUI();
	~CMyImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	void  AddTemp(DWORD dwIndex, DWORD dwTemp);

private:
	time_t    GetFirstTime();
	time_t    GetLastTime();

private:
	vector<TempData *>           m_vTempData[MAX_READERS_COUNT];

private:
	HPEN                         m_hCommonThreadPen;
	HPEN                         m_hBrighterThreadPen;
};