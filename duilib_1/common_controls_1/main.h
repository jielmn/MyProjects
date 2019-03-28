#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "resource.h"
#include "DragDropUI.h"
#include "MyControlUI.h"
#include "PatientCardUI.h"

#define TIMER_ID_PROGRESS 200
//#define TIMER_ID_REPAINT  201
#define UM_AFTER_PAINT   (WM_USER+1)
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
	void OnFinalMessage(HWND hWnd);

#ifndef _DEBUG
	virtual CDuiString GetSkinFolder() { return _T(""); }
#else
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }
#endif

#ifndef _DEBUG
	virtual UILIB_RESOURCETYPE GetResourceType() const {
		return UILIB_ZIPRESOURCE;
	}

	virtual LPCTSTR GetResourceID() const {
		return MAKEINTRESOURCE(IDR_ZIPRES1);
	}
#endif

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void OnWndInit();
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void MoveBrowser();

private:
	DuiLib::CTabLayoutUI *   m_tabs;
	CDialogBuilderCallbackEx m_callback;
	CIPAddressExUI *         m_ip;
	CEditUI *                m_edIpAddr;
	CHotKeyUI *              m_hotkey;
	CEditUI *                m_edHotKey;
	CProgressUI *            m_progress;
	CEditUI *                m_edFileName;
	CFileBrowseUI *          m_filebrowse;
	CVerticalLayoutUI *      m_layBrowser;
	CListUI *                m_List1;
	CComboUI *               m_Combo1;

	HWND                     m_hWndBrowser;
	DWORD                    m_dwBrowerProcId;
	RECT                     m_BrowserRect;

	CMyControlUI *           m_mycontrol;
	CDragDropUI *            m_dragdrop;
	CDragDropUI *            m_dragdrop_1;
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




