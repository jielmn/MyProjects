#pragma once

#include "common.h"
#include "business.h"
#include "UIlib.h"
#include "MyTreeCfgUI.h"

class CSettingDlg : public DuiLib::WindowImplBase
{
public:
	CSettingDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T(SETTING_FRAME_NAME); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T(SETTING_FILE); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

private:
	CMyTreeCfgUI  *             m_tree;

private:
	void    InitCommonCfg();
	void    SetEditStyle(CEditUI * pEdit);
	void    AddComboItem(CComboUI * pCombo, const char * szItem);
	void    SetComboStyle(CComboUI * pCombo);
	void    InitGridCfg(CMyTreeCfgUI::Node* pTitleNode, DWORD dwIndex);
	void    OnBtnOk(DuiLib::TNotifyUI& msg);
	void    OnMyClick(DuiLib::TNotifyUI& msg);
};

