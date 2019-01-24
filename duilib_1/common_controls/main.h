#pragma once

#include "common.h"
#include "resource.h"
#include "UIlib.h"
using namespace DuiLib;

#define TIMER_ID_PROGRESS (1000)

class CDuiFrameWnd : public WindowImplBase
{
public:
	CDuiFrameWnd();
	~CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_CLASS_WINDOW_NAME); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_FILE); }

#ifndef _DEBUG
	virtual CDuiString GetSkinFolder() { return _T(""); }
#else
	virtual CDuiString GetSkinFolder() { return _T("res\\" SKIN_FOLDER); }
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
	CAnimationTabLayoutUI *   m_pStepTabLayout;
	CProgressUI	*             m_pProgressBar;
	CLabelUI	*             m_pInstallText;
	CButtonUI   *             m_pAgainBtn;

private:
	void    InstallStart();
	void    OnMyTimer();
	void    InstallFinished();
};
