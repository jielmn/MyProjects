#pragma once
#include "UIlib.h"
//using namespace DuiLib;

class CLoginWnd : public DuiLib::WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUILogin"); }
	virtual DuiLib::CDuiString GetSkinFile()              { return _T("login.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder()            { return _T("res"); }

	virtual void    Notify(DuiLib::TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void   InitWindow();

private:
	DuiLib::CLabelUI *     m_lblDbStatus;
	DuiLib::CLabelUI *     m_lblReaderStatus;
};