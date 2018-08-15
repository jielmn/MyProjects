#pragma once

#include "common.h"
#include "MyProgress.h"

#include "UIlib.h"
using namespace DuiLib;

class CDuiFrameWnd : public WindowImplBase
{
public:
	CDuiFrameWnd();
	~CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_CLASS_WINDOW_NAME); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_FILE); }
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

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

	void  OnSettingReaderRet(WPARAM wParm, LPARAM  lParam);
	void  OnDeviceChanged(WPARAM wParm, LPARAM  lParam);
	void  OnSettingGwRet(WPARAM wParm, LPARAM  lParam);
	void  OnQueryGwRet(WPARAM wParm, LPARAM  lParam);

private:
	DuiLib::CTabLayoutUI *              m_tabs;
	DuiLib::CComboUI *                  m_reader_com_ports;
	DuiLib::CComboUI *                  m_gw_com_ports;
	DuiLib::CEditUI *                   m_edtArea1;
	DuiLib::CEditUI *                   m_edtBedNo;
	DuiLib::CEditUI *                   m_edtArea2;
	CMyProgress *                       m_progress;
	DuiLib::CButtonUI *                 m_btnSetting1;
	DuiLib::CButtonUI *                 m_btnSetting2;
	DuiLib::CButtonUI *                 m_btnQuery2;
	DuiLib::CEditUI *                   m_edtQuery2Ret;

	BOOL                                m_bBusy;

};
