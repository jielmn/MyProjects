#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

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
	void  OnDeviceChanged(WPARAM wParm, LPARAM  lParam);
	void  OnMyOpen();
	void  SetBusy(BOOL bBusy = TRUE);
	void  SaveData();

private:
	CComboUI *             m_cmbComPorts;
	CButtonUI *            m_btnOpen;
	CButtonUI *            m_btnBrowse;
	BOOL                   m_bOpend;
	BOOL                   m_bBusy;
	CDataBuf               m_buf;
	CRichEditUI *          m_rich;
};
