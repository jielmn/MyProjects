#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "MyControls.h"
#include "lua.hpp"

#include "resource.h"

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
	CComboUI*                      m_cmbComPorts;
	CComboUI *                     m_cmbLuaFiles;
	CComboUI *                     m_cmbBaud;
	CMyChartUI *                   m_chart;
	BOOL                           m_bStartPaint;
	BOOL                           m_bBusy;
	BOOL                           m_bConnected;
	CButtonUI *                    m_btnPaint;

	lua_State*                     m_L;        // lua
	BOOL                           m_bCorrectLua;
	CDataBuf                       m_buf;

	COptionUI *                    m_opChannels[MAX_CHANNEL_COUNT];

private:
	void  OnDeviceChanged();
	void  InitCmbLuaFiles();
	void  OnStartPaint();
	void  SetBusy(BOOL bBusy = TRUE);
	void  OnOpenComRet(BOOL bRet);
	void  OnLuaFileSelected();
	void  OnReceive();

};
