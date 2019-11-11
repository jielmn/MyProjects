#pragma once

#include "UIlib.h"
using namespace DuiLib;

class CWaitingBarUI : public CProgressUI
{
public:
	CWaitingBarUI();
	~CWaitingBarUI();

	void DoEvent(TEventUI& event);
	void Stop();
	void Start();

private:
	int               m_nPos;
	CDuiString        m_sForeImage;
};