#pragma once

#include "common.h"
#include "business.h"
#include "UIlib.h"
#include "MyTreeCfgUI.h"

class CPatientDataDlg : public DuiLib::WindowImplBase
{
public:
	CPatientDataDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T(PATIENT_DATA_FRAME_NAME); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T(PATIENT_DATA_FILE); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

private:
	void  OnMyInited();
	void  InitInfo();
	void  InitData();
	void  AddComboItem(CComboUI * pCombo, const char * szItem, UINT_PTR tag);
	void  OnPrintPreview();
	void  OnReturn();
	void  GetSevenDayStr( CDuiString * pDays, DWORD dwSize, time_t tLastTime, BOOL bMonthDay = FALSE );
	void OnFinalMessage(HWND hWnd);

private:
	CMyTreeCfgUI  *             m_tree;
	CTabLayoutUI *              m_switch;

public:
	char                        m_szTagId[MAX_TAG_ID_LENGTH];              // Tag Id
	char                        m_szUIPName[MAX_TAG_PNAME_LENGTH];         // UI 传过来的name
};