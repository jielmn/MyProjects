#pragma once

#include "common.h"
#include "MyTreeCfgUI.h"
#include "MyControls.h"

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
	CMyTreeCfgUI  *             m_tree;
	CEdtComboUI *               m_comport;

private:
	void  OnMyInited();
	void  OnMyOk();
	int   GetComPort(CDuiString strCom);

public:
	int                       m_nComPort;
};

