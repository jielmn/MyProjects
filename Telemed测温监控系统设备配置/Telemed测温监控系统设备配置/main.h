#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "resource.h"
#include "MyProgress.h"
#include "MyProgress.h"

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
	void  OnSettingHandReader();
	void  OnHandReaderSettingRet(WPARAM wParm, LPARAM  lParam);
	void  SetBusy(BOOL bBusy = TRUE);
	void  OnSetHandReaderSn();
	void  OnSetHandReaderSnRet(WPARAM wParm, LPARAM  lParam);
	void  OnAreaListMenu(TNotifyUI& msg);
	void  OnAddArea();
	void  OnModifyArea();
	void  OnDeleteArea();
	void  OnAreasChanged();
	void  OnChannelAChanged();
	void  OnSetReceiverChannel();
	void  OnSetReceiverChannelRet(WPARAM wParm, LPARAM  lParam);
	void  OnSetSurgencyReader();
	void  OnSetSurgencyReaderRet(WPARAM wParm, LPARAM  lParam);

private:
	DuiLib::CTabLayoutUI *              m_tabs;
	// tab 1
	DuiLib::CComboUI *                  m_cmbHandReaderCom;
	DuiLib::CComboUI *                  m_cmbHandReaderAddr;
	DuiLib::CComboUI *                  m_cmbHandReaderChannel;
	DuiLib::CButtonUI *                 m_btnHandReader_1;
	DuiLib::CButtonUI *                 m_btnHandReader_2;
	DuiLib::CEditUI *                   m_edHandReaderSn;
	// tab 2
	DuiLib::CComboUI *                  m_cmbSurgencyReaderCom;
	DuiLib::CComboUI *                  m_cmbArea_1;
	DuiLib::CComboUI *                  m_cmbBed_1;
	DuiLib::CComboUI *                  m_cmbBed_2;
	DuiLib::CEditUI *                   m_edSurgencyReaderSn;
	DuiLib::CButtonUI *                 m_btnSurgencyReader_1;
	DuiLib::CButtonUI *                 m_btnSurgencyReader_2;
	// tab 3
	DuiLib::CComboUI *                  m_cmbReceiverCom;
	DuiLib::CComboUI *                  m_cmbArea_2;
	DuiLib::CButtonUI *                 m_btnReceiver_1;
	DuiLib::CButtonUI *                 m_btnReceiver_2;
	DuiLib::CComboUI *                  m_cmbChannel_a;
	DuiLib::CEditUI *                   m_edtChannel_b;
	DuiLib::CEditUI *                   m_edtChannel_c;
	// tab 4
	DuiLib::CListUI *                   m_lstArea;
	// progress
	CMyProgress *                       m_progress;

};
