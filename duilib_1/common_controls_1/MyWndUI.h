#pragma once

#include "common.h"

class CMyWndUI : public DuiLib::CControlUI
{
public:
	CMyWndUI();
	~CMyWndUI();

	virtual LPCTSTR GetClass() const {
		return "MyWnd";
	}

private:
	void  DoInit();
	bool  OnMySize(void * pParam);

private:
	HWND  m_hWnd;
};
