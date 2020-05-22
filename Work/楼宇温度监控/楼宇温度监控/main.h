#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "MyControls.h"

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
	CListUI *                        m_lstFloors;
	CTileLayoutUI *                  m_layDevices;

private:
	void   AddFloor(int nFloor);
	void   OnFloorSelected(int nOldSelected);
	bool   OnLayoutDevicesSize(void * pParam);

	void   OnDeviceHighlight(CDeviceUI * pDevice);
	void   OnDeviceUnHighlight(CDeviceUI * pDevice);
};
