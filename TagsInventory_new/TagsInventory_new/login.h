#pragma once
#include "UIlib.h"
using namespace DuiLib;

class CLoginWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUILogin"); }
	virtual CDuiString GetSkinFile()              { return _T("login.xml"); }
	virtual CDuiString GetSkinFolder()            { return _T("res"); }

	virtual void    Notify(TNotifyUI& msg);
};