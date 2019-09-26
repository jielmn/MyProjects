#pragma once

#include "common.h"

class CSettingDlg : public DuiLib::WindowImplBase
{
public:
	CSettingDlg();

	virtual LPCTSTR    GetWindowClassName() const { return "DUISettingFrame"; }
	virtual DuiLib::CDuiString GetSkinFile() { return "Setting.xml"; }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

private:
	CComboUI *                 m_cmbComPorts;
	CEditUI *                  m_edComPort;
public:
	int                        m_nComPort;

private:
	void   CheckDevice();
};

