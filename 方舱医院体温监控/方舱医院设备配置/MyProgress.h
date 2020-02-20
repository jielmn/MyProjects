#pragma once
#include "UIlib.h"

class CMyProgress : public DuiLib::CProgressUI
{
public:
	CMyProgress(DuiLib::CPaintManagerUI *p, DuiLib::CDuiString sForeImage);
	~CMyProgress();

	void DoEvent(DuiLib::TEventUI& event);
	void Stop();
	void Start();

private:
	DuiLib::CPaintManagerUI * m_pManager;
	int                       m_nPos;
	DuiLib::CDuiString        m_sForeImage;
};