#pragma once

#include "common.h"
#include "business.h"
#include "UIlib.h"
#include "MyTreeCfgUI.h"
#include "WaitingBarUI.h"
#include "sigslot.h"
#include "PatientDataPrintPreviewUI.h"
#include "SixGridsUI.h"

class CPatientDataDlg : public DuiLib::WindowImplBase, public sigslot::has_slots<>
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
	void  OnPatientInfo(PatientInfo * pInfo);
	void  OnPatientData(PatientData *, DWORD);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	void  OnMyInited();
	void  InitInfo();
	void  InitData();
	void  AddComboItem(CComboUI * pCombo, const char * szItem, UINT_PTR tag);
	void  OnPrintPreview();
	void  OnReturn();
	void  OnPrint();
	void  GetSevenDayStr( CDuiString * pDays, DWORD dwSize, time_t tLastTime, BOOL bMonthDay = FALSE );
	void  OnFinalMessage(HWND hWnd);
	void  SetBusy(BOOL bBusy = TRUE);
	void  OnPatientInfoRet(WPARAM wParam, LPARAM  lParam);
	void  OnPatientDataRet(WPARAM wParam, LPARAM  lParam);
	// 从界面获取填入的info
	void  GetPatientInfo(PatientInfo * pInfo);
	// 从界面获取填入的data
	void  GetPatientData(PatientData * pData, DWORD dwSize);
	// 病人info是否改变
	BOOL  IsPatientInfoChanged(PatientInfo * pInfo);
	// 病人data是否改变
	BOOL  IsPatientInfoChanged(PatientData * pData, int nIndex);
	// 时间范围的起始时间改变
	void  OnDateStartChanged();
	// 时间控件失去焦点
	void  OnDateStartKillFocus();
	// 选中了事件UI
	void  OnMyEventSelected(CControlUI * pCtl);
	// 
	void  OnAddMyEvent();
	//
	void  OnDelMyEvent();

private:
	CMyTreeCfgUI  *             m_tree;
	CTabLayoutUI *              m_switch;
	CWaitingBarUI *             m_waiting_bar;
	CButtonUI *                 m_btnPreview;
	CButtonUI *                 m_btnPrint;
	CButtonUI *                 m_btnReturn;

	BOOL                        m_bBusy;
	PatientInfo                 m_patient_info;
	PatientData                 m_patient_data[7];
	time_t                      m_tDate;

	CPatientDataPrintPreviewUI *  m_preview;

	CMyDateUI *               m_date_start;
	CMyDateUI *               m_date_end;
	CVerticalLayoutUI *       m_lay_events;
	int                       m_selected_event;

public:
	char                        m_szTagId[MAX_TAG_ID_LENGTH];              // Tag Id
	char                        m_szUIPName[MAX_TAG_PNAME_LENGTH];         // UI 传过来的name
};