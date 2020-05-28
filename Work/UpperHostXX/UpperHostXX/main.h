#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "resource.h"
#include "MyTreeCfgUI.h"
#include "lua.hpp"

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
	void  OnDeviceChanged(WPARAM wParm, LPARAM  lParam);
	void  InitCmbLuaFiles();
	void  OnLuaFileSelected();
	void  OnSend();
	void  OnOpen();
	void  OnClear();
	bool  OnGridsSize(void * pParam);
	void  OnClear1();
	void  OnFinalMessage(HWND hWnd);

private:
	CComboUI *             m_cmbComPorts;
	CComboUI *             m_cmbLuaFiles;
	CEditUI *              m_edDescription;
	CMyTreeCfgUI *         m_params;
	CButtonUI *            m_btnSend;
	CButtonUI *            m_btnOpen;
	CRichEditUI *          m_rich;
	CTabLayoutUI *         m_tabs;
	CTileLayoutUI *        m_layFormat;
	COptionUI *            m_opFormatData;

private:
	lua_State*             m_L;        // lua
	BOOL                   m_bOpend;
	CDataBuf               m_buf;
	CDataBuf               m_buf_rch;
	int                    m_nFormatType;
	int                    m_nMaxItemsCnt;
	int                    m_nItemWidth;
	int                    m_nItemHeight;
	BOOL                   m_bUtf8;
};
