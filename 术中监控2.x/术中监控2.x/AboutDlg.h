#pragma once

#include "common.h"

class CAboutDlg : public DuiLib::WindowImplBase
{
public:
	CAboutDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIAboutFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("About.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("wrong_folder"); }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();

	LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CLabelUI *                     m_lblVersion;
};

