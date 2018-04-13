#pragma once

#include "exhCommon.h"
#include "Business.h"
#include "UIlib.h"

class CSettingDlg : public DuiLib::WindowImplBase
{
public:
	CSettingDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUISettingFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("Setting.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();

	LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	
};
