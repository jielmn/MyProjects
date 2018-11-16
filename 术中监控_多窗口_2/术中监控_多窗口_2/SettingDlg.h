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
	void    AddComboItem(CComboUI * pCombo, const char * szItem, UINT_PTR tag);
	void    SetComboStyle(CComboUI * pCombo);
	void    InitGridCfg(CMyTreeCfgUI::Node* pTitleNode, DWORD dwIndex);
	void    OnBtnOk(DuiLib::TNotifyUI& msg);
	void    OnMyClick(DuiLib::TNotifyUI& msg);
	BOOL    GetCommonConfig();
	BOOL    GetConfig( int nIndex, DWORD & dwInterval, DWORD & dwLowAlarm,
		               DWORD & dwHighAlarm, DWORD & dwMinTemp, DWORD & dwBedNo,BOOL & bSwitch);


private:
	DWORD     m_dwLayoutColumns;
	DWORD     m_dwLayoutRows;
	DWORD     m_dwSkinIndex;
	DWORD     m_bAlarmVoiceOff;
	DWORD     m_dwAreaNo;
	DWORD     m_dwTempFont;

	DWORD     m_dwLowTempAlarm[MAX_GRID_COUNT];
	DWORD     m_dwHighTempAlarm[MAX_GRID_COUNT];
	DWORD     m_dwCollectInterval[MAX_GRID_COUNT];
	DWORD     m_dwMyImageMinTemp[MAX_GRID_COUNT];
	DWORD     m_dwBedNo[MAX_GRID_COUNT];
	BOOL      m_bGridReaderSwitch[MAX_GRID_COUNT];
};

