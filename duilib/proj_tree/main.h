#pragma once

#include "Windows.h"
#include "UIlib.h"
using namespace DuiLib;

#include "MyTreeUI.h"

class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile() { return _T("mainframe_tree.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res\\proj_tree_res"); }

	virtual void    Notify(TNotifyUI& msg);
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);      

	virtual void      InitWindow();

private:
	CMyTreeUI  *      m_tree;
	CLabelUI*         m_view;
};
