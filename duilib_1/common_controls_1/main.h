#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "UIIPAddressEx.h"
#include "UIHotKey.h"

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CDialogBuilderCallbackEx(DuiLib::CPaintManagerUI *pManager) {
		m_pManager = pManager;
	}
	CControlUI* CreateControl(LPCTSTR pstrClass);
private:
	DuiLib::CPaintManagerUI *  m_pManager;
};

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
	CDialogBuilderCallbackEx m_callback;
	CIPAddressExUI *         m_ip;
	CEditUI *                m_edIpAddr;
	CHotKeyUI *              m_hotkey;
	CEditUI *                m_edHotKey;
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
