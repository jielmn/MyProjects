#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "resource.h"

#include "CustomControls.h"
#include "WaitingBarUI.h"

class MyFilter : public IMessageFilterUI
{
public:
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) {
		if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP) {
			if (wParam == VK_TAB) {
				bHandled = TRUE;
			}
		}
		return 0;
	}
};

class CDuiFrameWnd : public WindowImplBase
{
public:
	CDuiFrameWnd();
	~CDuiFrameWnd();
	virtual void OnFinalMessage(HWND hWnd);

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
	DuiLib::CHorizontalLayoutUI *             m_layMain;
	DuiLib::CVerticalLayoutUI *               m_layColumns[MAX_COLUMNS_CNT];
	int                                       m_nCurColumns;
	int                                       m_nCurRows;
	CTempItemUI *                             m_temp_items[MAX_TEMP_ITEMS_CNT];

	// 串口
	CComboUI *             m_cmbComPorts;
	CComboUI *             m_cmbMachineType;
	CComboUI *             m_cmbFiles;

	CButtonUI *            m_btnSave;
	CButtonUI *            m_btnSaveAs;

	BOOL                   m_bBusy;
	CButtonUI *            m_btnAdjustAll;
	CButtonUI *            m_btnDiff;
	CProgressUI *          m_waiting_bar;

	MyFilter               m_filter;
	int                    m_nHighlightIndex;          // 高亮的item，从0开始，-1是无效值

	CDialogBuilderCallbackEx                m_callback;
	BOOL                                    m_bItemsAdded;
	DuiLib::CTabLayoutUI *                  m_tabs;
	BOOL                                    m_bAdjustTagSelected;

	COptionUI *                             m_opSingle;
	COptionUI *                             m_opBroadcast;
	COptionUI *                             m_opAdjustMode;
	COptionUI *                             m_opTempMode;
	CEditUI *                               m_edReaderAddr;
	CButtonUI *                             m_btnOk1;

	CEditUI *                               m_edReaderAddr_1;
	CEditUI *                               m_edReaderChannel_1;
	CEditUI *                               m_edStationAddr_1;
	CEditUI *                               m_edStationChannel_1;
	CEditUI *                               m_edQueryStationChannel_1;
	CButtonUI *                             m_btnOk2;
	CButtonUI *                             m_btnOk3;
	CButtonUI *                             m_btnOk4;
	CEditUI *                               m_edReaderSn_1;
	CButtonUI *                             m_btnOk5;

private:
	void  OnWindowInit();
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	bool  OnMainSize(void * pParam);
	// 检查硬件(串口)
	void  CheckDevice();
	void  OnFileChanged();
	void  OnMachineChanged();
	void  OnAdjust(int nIndex);
	void  OnDiff();
	void  OnSaveAs();
	void  OnSave();	
	MachineType  GetMachineType();
	CDuiString GetTempDataFileName();
	int   GetComPort();
	void  SetBusy(BOOL bBusy = TRUE, BOOL bWaitingBar = FALSE);
	void  OnAdjustAll();

	void  OnSetWorkMode();

	void  OnSetReader();
	void  OnSetStation();
	void  OnQueryStation();
	void  OnSetReaderSn();
};
