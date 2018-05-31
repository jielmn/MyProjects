#pragma once

#include "Windows.h"
#include "UIlib.h"
using namespace DuiLib;

#include "MyTreeCfgUI.h"

class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile() { return _T("mainframe_tree_config.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }

	virtual void    Notify(TNotifyUI& msg);
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);

	virtual void      InitWindow();

private:
	CMyTreeCfgUI  *      m_tree;
	CLabelUI*            m_view;
};

