#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "MyControls.h"
#include "resource.h"

class CAddClassDlg : public DuiLib::WindowImplBase
{
public:
	CAddClassDlg();

	virtual LPCTSTR    GetWindowClassName() const { return "DUIAddClassFrame"; }
	virtual DuiLib::CDuiString GetSkinFile() { return "AddClassDlg.xml"; }
	virtual DuiLib::CDuiString GetSkinFolder() { return SKIN_FOLDER; }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

private:
	CComboUI *                 m_cmbGrade;
	CComboUI *                 m_cmbClass;

public:
	DWORD                      m_dwClassNo;
};

class CDuiFrameWnd : public WindowImplBase
{
public:
	CDuiFrameWnd();
	~CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_CLASS_WINDOW_NAME); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_FILE); }

#ifndef _DEBUG
	virtual CDuiString GetSkinFolder() { return _T(""); }
#else
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }
#endif

#ifndef _DEBUG
	virtual UILIB_RESOURCETYPE GetResourceType() const {
		return UILIB_ZIPRESOURCE;
	}

	virtual LPCTSTR GetResourceID() const {
		return MAKEINTRESOURCE(IDR_ZIPRES1);
	}
#endif

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CListUI *                                   m_lstClasses;
	DuiLib::CTileLayoutUI *                     m_layDesks;
	DuiLib::CVerticalLayoutUI *                 m_layRows;
	DuiLib::CTileLayoutUI *                     m_layCols;
	DWORD                                       m_dwHighlightIndex;
	DuiLib::CContainerUI *                      m_layRoom;
	std::vector<DWORD>                          m_vClasses;
	std::vector<DeskItem *>                     m_vDeskes;

private:
	void   OnDeskHighlight(DWORD dwIndex);
	void   OnDeskUnHighlight(DWORD dwIndex);
	CDeskUI *  GetDeskUI(DWORD  dwTag);
	void   OnAddClass();
	void   OnDelClass();
	void   UpdateClasses();

};
