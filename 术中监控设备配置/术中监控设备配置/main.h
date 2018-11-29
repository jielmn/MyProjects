#pragma once

#include "common.h"
#include "MyProgress.h"

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
	void  OnFreshComPort_Reader();
	void  OnFreshComPort_Gw();
	void  OnSettingReader();
	void  OnSettingGw();
	void  SetBusy(BOOL bBusy);
	void  OnQueryGw();
	void  OnAddArea();
	void  OnModifyArea();
	void  OnDeleteArea();
	void  OnAreasChanged();

	void  OnAreaListMenu(TNotifyUI& msg);

	void  OnSettingReaderRet(WPARAM wParm, LPARAM  lParam);
	void  OnDeviceChanged(WPARAM wParm, LPARAM  lParam);
	void  OnSettingGwRet(WPARAM wParm, LPARAM  lParam);
	void  OnQueryGwRet(WPARAM wParm, LPARAM  lParam);

private:
	DuiLib::CTabLayoutUI *              m_tabs;
	DuiLib::CComboUI *                  m_reader_com_ports;
	DuiLib::CComboUI *                  m_gw_com_ports;
	DuiLib::CEditUI *                   m_edtBedNo;
	DuiLib::CComboUI *                  m_cmbArea1;
	DuiLib::CComboUI *                  m_cmbArea2;
	CMyProgress *                       m_progress;
	DuiLib::CButtonUI *                 m_btnSetting1;
	DuiLib::CButtonUI *                 m_btnSetting2;
	DuiLib::CButtonUI *                 m_btnQuery2;
	DuiLib::CEditUI *                   m_edtQuery2Ret;
	DuiLib::CListUI *                   m_lstArea;

	DuiLib::CHorizontalLayoutUI *       m_lay1;
	DuiLib::CHorizontalLayoutUI *       m_lay2;
	DuiLib::CComboUI *                  m_cmbBedNo1;
	DuiLib::CComboUI *                  m_cmbBedNo2;

	BOOL                                m_bBusy;

};
