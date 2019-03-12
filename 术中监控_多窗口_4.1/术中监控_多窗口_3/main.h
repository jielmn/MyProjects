#pragma once

#include "common.h"
#include "MyImage.h"
#include "AlarmImage.h"
#include "resource.h"

class CDuiFrameWnd : public WindowImplBase
{
public:
	enum ENUM_GRID_STATUS {
		GRID_STATUS_GRIDS = 0,
		GRID_STATUS_MAXIUM
	};

	CDuiFrameWnd();
	~CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_CLASS_WINDOW_NAME); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_FILE); }
	//virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

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
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void     OnFinalMessage(HWND hWnd);

private:
	CDialogBuilderCallbackEx           m_callback;
	DuiLib::CTileLayoutUI *            m_layMain;
	DuiLib::CHorizontalLayoutUI *      m_layStatus;
	CLabelUI *                         m_lblLaunchStatus;
	CLabelUI *                         m_lblBarTips;
	CLabelUI *                         m_lblProcTips;
	CControlUI *                       m_pGrids[MAX_GRID_COUNT];
	DuiLib::CVerticalLayoutUI *        m_layGrid[MAX_GRID_COUNT];
	DuiLib::CHorizontalLayoutUI *      m_layMaxium[MAX_GRID_COUNT];
	CLabelUI *                         m_LblIndex_grid[MAX_GRID_COUNT];
	CLabelUI *                         m_LblIndex_maxium[MAX_GRID_COUNT];
	CButtonUI *                        m_BtnEmpty[MAX_GRID_COUNT];
	CLabelUI *                         m_LblBodyPart[MAX_GRID_COUNT];
	CButtonUI *                        m_BtnBed_grid[MAX_GRID_COUNT];
	CEditUI *                          m_EdtBed_grid[MAX_GRID_COUNT];
	CButtonUI *                        m_BtnName_grid[MAX_GRID_COUNT];
	CEditUI *                          m_EdtName_grid[MAX_GRID_COUNT];
	CLabelUI *                         m_LblCurTemp_grid[MAX_GRID_COUNT];
	CMyLabelUI *                       m_LblCurTemp_grid1[MAX_GRID_COUNT];
	CLabelUI *                         m_LblCurTempTime[MAX_GRID_COUNT];
	CLabelUI *                         m_LblBedTitle_grid[MAX_GRID_COUNT];
	CLabelUI *                         m_LblNameTitle_grid[MAX_GRID_COUNT];
	CLabelUI *                         m_LblCurTempTitle_grid[MAX_GRID_COUNT];
	CMyImageUI *                       m_MyImage_grid[MAX_GRID_COUNT];
	COptionUI *                        m_OptGridSwitch_grid[MAX_GRID_COUNT];
	CAlarmImageUI *                    m_MyAlarm_grid[MAX_GRID_COUNT];
	CButtonUI *                        m_BtnBed_max[MAX_GRID_COUNT];
	CEditUI *                          m_EdtBed_max[MAX_GRID_COUNT];
	CButtonUI *                        m_BtnName_max[MAX_GRID_COUNT];
	CEditUI *                          m_EdtName_max[MAX_GRID_COUNT];
	CMyImageUI *                       m_MyImage_max[MAX_GRID_COUNT];
	COptionUI *                        m_OptGridSwitch_max[MAX_GRID_COUNT];
	DuiLib::CVerticalLayoutUI *        m_layReaders[MAX_GRID_COUNT];
	DuiLib::CLabelUI *                 m_lblTempTitle[MAX_GRID_COUNT];
	DuiLib::CVerticalLayoutUI *        m_lay21[MAX_GRID_COUNT];
	CControlUI *                       m_UiReaders[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	COptionUI *                        m_UiReaderSwitch[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	CAlarmImageUI *                    m_UiAlarms[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	CLabelUI *                         m_UiReaderTemp[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	CButtonUI *                        m_UiBtnReaderNames[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	CEditUI *                          m_UiEdtReaderNames[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	CVerticalLayoutUI *                m_UiLayReader[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	CControlUI *                       m_UiIndicator[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	DuiLib::CButtonUI *                m_btnMenu;    // 菜单按钮	
	ENUM_GRID_STATUS                   m_eGridStatus;
	DWORD                              m_dwInflateGridIndex;
	LastTemp                           m_tLastTemp[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	TagBinding                         m_tTagBinding[MAX_GRID_COUNT][MAX_READERS_PER_GRID];

	CLabelUI *                         m_LblDbStatus;
	CButtonUI *                        m_btnBinding[MAX_GRID_COUNT];
	CLabelUI *                         m_LblReaderId[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	CLabelUI *                         m_LblTagId[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	CLabelUI *                         m_LblTagBinding[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	CLabelUI *                         m_LblConflictTips;
	time_t                             m_LastSaveExcelTime;

	CHorizontalLayoutUI *              m_layMain_1;
	int                                m_nSelTabIndex;
	CMyImageUI_1 *                     m_MyImage_hand_reader;

private:
	void   OnSize(WPARAM wParam, LPARAM lParam);
	// 重新布局
	void   ReLayout(DWORD dwWidth, DWORD dwHeight);
	void   OnChangeSkin();
	void   OnDbClick();
	void   OnGridInflate(DWORD dwIndex);
	void   OnGridInflateSub(DWORD dwIndex);
	void   OnEdtRemarkKillFocus();
	void   OnBtnMenu(TNotifyUI& msg);
	void   OnSetting();
	void   OnAbout();
	void   OnTestTimer(DWORD  dwTimer);
	void   OnUpdateTimeDescTimer();

	void   OnBtnBed_grid(TNotifyUI& msg);
	void   OnEdtBedKillFocus_grid(TNotifyUI& msg);
	void   OnBtnName_grid(TNotifyUI& msg);
	void   OnEdtNameKillFocus_grid(TNotifyUI& msg);
	void   OnGridSwitch(TNotifyUI& msg);
	void   OnBtnBed_max(TNotifyUI& msg);
	void   OnBtnName_max(TNotifyUI& msg);
	void   OnEdtBedKillFocus_max(TNotifyUI& msg);
	void   OnEdtNameKillFocus_max(TNotifyUI& msg);
	void   OnReaderSwitch(TNotifyUI& msg);
	void   OnUpdateGridScroll(WPARAM wParam, LPARAM lParam);
	void   UpdateLayout();
	void   OnReaderName(TNotifyUI& msg);
	void   OnEdtReaderNameKillFocus(TNotifyUI& msg);
	void   OnMyLButtonDown(WPARAM wParam, LPARAM lParam);
	void   OnLayReaderSelected(DWORD dwIndex, DWORD dwSubIndex);
	void   OnTemp(DWORD dwIndex, DWORD dwSubIndex, DWORD dwTemp);
	void   OnLaunchStatus(WPARAM wParam, LPARAM  lParam);
	void   OnReaderTemp(WPARAM wParam, LPARAM  lParam);
	void   OnReaderDisconnected(WPARAM wParam, LPARAM  lParam);
	// 鼠轮
	void   OnMyMouseWheel(WPARAM wParam, LPARAM lParam);
	// 
	void   OnMyDeviceChanged();
	//
	void   OnMyImageClick(DWORD dwIndex, const POINT * pPoint);
	//
	void   OnMyImageMenu(TNotifyUI& msg);
	//
	void   OnExportExcel();
	//
	void   OnPrintExcel();
	// 清空数据
	void   OnEmpty(TNotifyUI& msg);
	//
	void   OnReaderProcessing(WPARAM wParam, LPARAM  lParam);
	//
	void   OnDbStatus(int nStatus);
	//
	void   OnQueryBindingRet(WPARAM wParam, LPARAM  lParam);
	// 
	void   OnCheckGridBinding(DWORD dwIndex);
	//
	void   OnCheckConflictTagTimer();
	//
	void   OnBinding(DWORD dwIndex);
	//
	void   OnCheckAutoSaveTimer();
	//
	void   OnTempSqliteRet(WPARAM wParam, LPARAM  lParam);
	//
	void   OnTabChanged(DWORD  dwIndex);
	//
	void   OnHandReaderTempSqliteRet(WPARAM wParam, LPARAM  lParam);
};
