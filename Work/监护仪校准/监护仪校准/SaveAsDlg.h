#pragma once

#include "common.h"
#include "business.h"

class CSaveAsDlg : public DuiLib::WindowImplBase
{
public:
	CSaveAsDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUISaveAsFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("SaveAs.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

public:
	CDuiString     m_strFoldName;
	CDuiString     m_strFileName;
};