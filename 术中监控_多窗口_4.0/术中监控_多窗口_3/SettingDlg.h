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
public:
	CfgData                     m_data;

private:
	void    InitCommonCfg();
	void    AddComboItem(CComboUI * pCombo, const char * szItem, UINT_PTR tag);
	void    InitGridCfg(CMyTreeCfgUI::Node* pTitleNode, DWORD dwIndex);
	void    OnBtnOk(DuiLib::TNotifyUI& msg);
	BOOL    GetCommonConfig();
	BOOL    GetGridConfig(int nIndex);
};

