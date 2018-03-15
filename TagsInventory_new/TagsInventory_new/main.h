#pragma once

#include "UIlib.h"
using namespace DuiLib;

class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile()              { return _T("main.xml"); }
	virtual CDuiString GetSkinFolder()            { return _T("res"); }

	virtual void    Notify(TNotifyUI& msg);
};