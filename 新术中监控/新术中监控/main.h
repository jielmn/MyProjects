#pragma once

#include "UIlib.h"
#include "ImageAll.h"

class CTempDataBuilderCallback : public DuiLib::IDialogBuilderCallback
{
public:
	virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);

	DuiLib::CPaintManagerUI*     m_PaintManager;
};

class CDuiFrameWnd : public DuiLib::WindowImplBase
{
public:
	CDuiFrameWnd();
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("mainframe.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void    InitWindow();
	virtual void    Notify(DuiLib::TNotifyUI& msg);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);
	// 处理自定义信息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	DuiLib::CTabLayoutUI *   m_tabs;
	DuiLib::CButtonUI    *   m_btnNext;
	DuiLib::CButtonUI    *   m_btnPrevius;

	DuiLib::CHorizontalLayoutUI *  m_navigator;

	DuiLib::CButtonUI    *   m_btnStart0;
	DuiLib::CButtonUI    *   m_btnStop0;

	DuiLib::CButtonUI    *   m_btnStart1;
	DuiLib::CButtonUI    *   m_btnStop1;

	CImageAll            *   m_image0;

	DuiLib::CButtonUI *      m_btnMenu;

private:
	CTempDataBuilderCallback  m_callback;

private:
	void  OnSetting();
	void  OnAbout();
};