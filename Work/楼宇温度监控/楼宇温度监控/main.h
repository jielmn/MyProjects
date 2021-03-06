#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "MyControls.h"
#include "resource.h"



class CAddFloorDlg : public DuiLib::WindowImplBase
{
public:
	CAddFloorDlg();

	virtual LPCTSTR    GetWindowClassName() const { return "DUIAddFloorFrame"; }
	virtual DuiLib::CDuiString GetSkinFile() { return "AddFloorDlg.xml"; }
	virtual DuiLib::CDuiString GetSkinFolder() { return SKIN_FOLDER; }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

private:
	CEditUI *                   m_edFloor;      
	COptionUI *                 m_opUnderGround;

private:
	void   OnBtnOk();

public:
	int                          m_nFloor;
};

class CAddDeviceDlg : public DuiLib::WindowImplBase
{
public:
	CAddDeviceDlg();

	virtual LPCTSTR    GetWindowClassName() const { return "DUIAddDeviceFrame"; }
	virtual DuiLib::CDuiString GetSkinFile() { return "AddDeviceDlg.xml"; }
	virtual DuiLib::CDuiString GetSkinFolder() { return SKIN_FOLDER; }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

private:
	void  OnBtnOk();

private:
	CLabelUI *                    m_lblFloor;
	CEditUI *                     m_edDeviceId;
	CEditUI *                     m_edDeviceAddress;

public:
	int     m_nFloor;
	int     m_nDeviceId;
	char    m_szDeviceAddr[32];
};

 

 

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
	CDeviceUI *                      m_pHighlightDevice;
	std::vector<int>                 m_vFloors;

	CLabelUI *                       m_lblDevicesTitle;
	CLabelUI *                       m_lblBarTips;

private:
	void   AddFloor(int nFloor);
	void   OnFloorSelected(int nOldSelected);
	bool   OnLayoutDevicesSize(void * pParam);

	void   OnDeviceHighlight(CDeviceUI * pDevice);
	void   OnDeviceUnHighlight(CDeviceUI * pDevice);

	void   OnAddFloor();
	void   OnDelFloor();
	void   UpdateFloors();
	void   SetFloor(CListContainerElementUI * pItem, int nFloor);
	void   SetWarning(CListContainerElementUI * pItem, BOOL bWarning);

	void   UpdateDevices();
	void   UpdateDevicesByFloor(std::vector<DeviceItem*> vRet, int nFloor);

	void   OnAddDevice();
	void   OnDelDevice(CDeviceUI * pDevice);

	// 硬件设备变动，可能有串口变动
	void   CheckDeviceCom();

	int    GetCurrentFloor();
};
