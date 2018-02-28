#pragma once
#include "UIlib.h"
using namespace DuiLib;

#include "Image.h"

class CMainWnd : public WindowImplBase
{
public:
	CMainWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIPatientFrame"); }
	virtual CDuiString GetSkinFile() { return _T("main.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }

	virtual void  Notify(TNotifyUI& msg);
	virtual void  InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);

private:
	CMyImageUI *   m_pImageUI;
}; 
