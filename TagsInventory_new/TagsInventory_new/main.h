#pragma once

#include "UIlib.h"
//using namespace DuiLib;

class CDuiFrameWnd : public DuiLib::WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual DuiLib::CDuiString GetSkinFile()              { return _T("main.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder()            { return _T("res"); }

	virtual void    InitWindow();
	virtual void    Notify(DuiLib::TNotifyUI& msg);

private:

	DuiLib::CLabelUI *     m_lblDbStatus;
	DuiLib::CLabelUI *     m_lblReaderStatus;
};