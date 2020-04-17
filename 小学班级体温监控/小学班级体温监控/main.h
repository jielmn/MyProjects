#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "MyControls.h"
#include "resource.h"

#define   FRESH_TEMP_COLOR      0xFF48D0CC
#define   FADE_TEMP_COLOR       0xFF447AA1

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

class CDeskDlg : public DuiLib::WindowImplBase
{
public:
	CDeskDlg();

	virtual LPCTSTR    GetWindowClassName() const { return "DUIDeskFrame"; }
	virtual DuiLib::CDuiString GetSkinFile() { return "DeskDlg.xml"; }
	virtual DuiLib::CDuiString GetSkinFolder() { return SKIN_FOLDER; }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

public:
	DeskItem         m_data;
	CEditUI *        m_edName;
	COptionUI *      m_opBoy;
	COptionUI *      m_opGirl;
};

typedef  struct tagDragdropDesk {
	CDeskUI *        m_source;
	CDeskUI *        m_highlight;
	DWORD            m_dwStartTick;
	BOOL             m_bStarted;
}DragdropDesk;

typedef  struct tagDragdropTag {
	CDeskUI *        m_highlight;
	DWORD            m_dwStartTick;
	BOOL             m_bStarted;
}DragdropTag;

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
	virtual LRESULT  OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);	
	virtual LRESULT  OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual LRESULT  OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

private:
	CListUI *                                   m_lstClasses;
	DuiLib::CTileLayoutUI *                     m_layDesks;
	DuiLib::CVerticalLayoutUI *                 m_layRows;
	DuiLib::CTileLayoutUI *                     m_layCols;
	DWORD                                       m_dwHighlightIndex;
	DuiLib::CContainerUI *                      m_layRoom;
	std::vector<DWORD>                          m_vClasses;
	std::vector<DeskItem *>                     m_vDeskes;
	DragdropDesk                                m_dragdrop_desk;
	CControlUI *                                m_DragdropUI;
	CLabelUI *                                  m_lblBarTips;
	TempItem                                    m_tNewTag;
	CLabelUI *                                  m_lblNewTagTemp;
	CLabelUI *                                  m_lblNewTagTime;
	CLabelUI *                                  m_lblNewTagId;
	CContainerUI *                              m_layNewTag;
	DragdropTag                                 m_dragdrop_tag;

private:
	void   OnDeskHighlight(DWORD dwIndex);
	void   OnDeskUnHighlight(DWORD dwIndex);
	CDeskUI *  GetDeskUI(DWORD  dwTag);
	void   OnAddClass();
	void   OnDelClass();
	void   UpdateClasses();
	void   UpdateRoom();
	void   UpdateRoom1(std::vector<DeskItem*> vRet, DWORD   dwNo);
	void   OnDeskDbClick(CDeskUI * pDeskUI);
	void   OnMoveDesk(const POINT & pt);
	void   OnStopMoveDesk();

	// 硬件设备变动，可能有串口变动
	void   CheckDevice();
	void   UpdateNewTag();
	void   OnMoveTag(const POINT & pt);
	void   OnStopMoveTag();
	WORD   GetCurClass();

};

class CFindDeskObj {
public:
	CFindDeskObj(DWORD  dwPos) {
		m_dwRow = HIBYTE(dwPos);
		m_dwCol = LOBYTE(dwPos);
	}

	bool operator() (DeskItem * pItem) {
		if ( pItem->nCol == m_dwCol && pItem->nRow == m_dwRow ) {
			return true;
		}
		return false;
	}

private:
	DWORD   m_dwRow;
	DWORD   m_dwCol;
};