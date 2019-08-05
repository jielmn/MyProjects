#pragma once

#include "UIlib.h"
using namespace DuiLib;

class CMyProgress : public CProgressUI
{
public:
	CMyProgress();
	~CMyProgress();

	void DoEvent(TEventUI& event);
	void Stop();
	void Start();

private:
	int               m_nPos;
	CDuiString        m_sForeImage;
};