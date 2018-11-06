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
	DuiLib::CTabLayoutUI *              m_tabs;
	DuiLib::CLabelUI *                  m_lblStatus;
	DuiLib::CComboUI *                  m_reader_com_ports;
	DuiLib::CComboUI *                  m_gw_com_ports;
	DuiLib::CListUI *                   m_lstArea;
	DuiLib::CComboUI *                  m_cmbArea1;
	DuiLib::CComboUI *                  m_cmbArea2;

private:
	void  OnFreshComPort_Reader();
	void  OnFreshComPort_Gw();
	void  OnDeviceChanged(WPARAM wParm, LPARAM  lParam);
	void  OnAreaListMenu(TNotifyUI& msg);
	void  OnAddArea();
	void  OnModifyArea();
	void  OnDeleteArea();
	void  OnAreasChanged();
};
