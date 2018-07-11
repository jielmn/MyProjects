#pragma once

#include "exhCommon.h"
#include "Business.h"
#include "UIlib.h"
#include "MyTreeCfgUI.h"

class CSettingDlg : public DuiLib::WindowImplBase
{
public:
	CSettingDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUISettingFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("Setting.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();

	LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

private:
	DuiLib::CEditUI *           m_edInteval;
	DuiLib::CEditUI *           m_edLowAlarm;
	DuiLib::CEditUI *           m_edHighAlarm;
	DuiLib::CEditUI *           m_edAlarmPath;

	CMyTreeCfgUI  *             m_tree;

	CComboUI *                  m_cmbComPort;
};
