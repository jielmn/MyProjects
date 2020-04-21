#pragma once

#include "common.h"
#include "business.h"
#include "CustomControls.h"

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
	CTempItemUI *  m_temp_items[MAX_TEMP_ITEMS_CNT];

private:
	CListUI *      m_lst;
	CEditUI *      m_edFileName;

private:
	void  OnMyOk();
};