#pragma once

#include "exhCommon.h"
#include "Business.h"
#include "UIlib.h"

class CAllDataImageDlg : public DuiLib::WindowImplBase
{
public:
	CAllDataImageDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIAboutFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("AddDataImage.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

	LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

};