#pragma once

#include "UIlib.h"

class CDuiFrameWnd : public DuiLib::WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("mainframe.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void    InitWindow();
	virtual void    Notify(DuiLib::TNotifyUI& msg);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

private:
	DuiLib::CTabLayoutUI *   m_tabs;
	DuiLib::CButtonUI    *   m_btnNext;
	DuiLib::CButtonUI    *   m_btnPrevius;

	DuiLib::CHorizontalLayoutUI *  m_navigator;

	DuiLib::CButtonUI    *   m_btnStart0;
	DuiLib::CButtonUI    *   m_btnStop0;

	DuiLib::CButtonUI    *   m_btnStart1;
	DuiLib::CButtonUI    *   m_btnStop1;

};