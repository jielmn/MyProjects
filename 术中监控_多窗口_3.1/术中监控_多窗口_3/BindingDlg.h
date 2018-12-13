#pragma once

#include "common.h"
#include "business.h"
#include "UIlib.h"
#include "MyTreeCfgUI.h"

class CBindingDlg : public DuiLib::WindowImplBase
{
public:
	CBindingDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T(BINDING_FRAME_NAME); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T(BINDING_FILE); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);
};

