#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

class CDuiFrameWnd : public WindowImplBase
{
public:
	CDuiFrameWnd();
	~CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_CLASS_WINDOW_NAME); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_FILE); }
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	DuiLib::CTabLayoutUI *   m_tabs;
};
 

class CDuiPopupWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("PopupWnd"); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_POPUP_FILE); }
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }
	virtual void       OnFinalMessage(HWND hWnd) { 
		delete this; 
	}
};

class CDuiModeWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("ModeWnd"); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_MODE_FILE); }
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }
	virtual void       OnFinalMessage(HWND hWnd) {
		delete this;
	}
};
