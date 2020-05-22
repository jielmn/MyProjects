#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

class CDeviceUI : public CContainerUI, INotifyUI
{
public:
	CDeviceUI();
	~CDeviceUI();

	static int FIXED_WIDTH;
	static int FIXED_HEIGHT;

private:
	LPCTSTR GetClass() const;
	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void DoEvent(DuiLib::TEventUI& event);
	void Notify(TNotifyUI& msg);
};