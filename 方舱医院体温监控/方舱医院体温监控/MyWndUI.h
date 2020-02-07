#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

class CMyWndUI : public DuiLib::CControlUI
{
public:
	CMyWndUI();
	~CMyWndUI();

	HWND GetHWnd() {
		return m_hWnd;
	}

	virtual LPCTSTR GetClass() const {
		return "MyWnd";
	}

	virtual void DoEvent(DuiLib::TEventUI& event);

private:
	void  DoInit();
	bool  OnMySize(void * pParam);

private:
	HWND  m_hWnd;
};
