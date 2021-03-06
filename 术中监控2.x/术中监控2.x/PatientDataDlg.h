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
	~CPatientDataDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T(PATIENT_DATA_FRAME_NAME); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T(PATIENT_DATA_FILE); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);
	void  OnPatientInfo(PatientInfo * pInfo, const std::vector<PatientEvent * > & vEvents);
	void  OnPatientData(PatientData *, DWORD, const std::vector<TempItem *> &);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	void  OnMyInited();
	void  InitInfo();
	void  InitData();
	void  AddComboItem(CComboUI * pCombo, const char * szItem, UINT_PTR tag);
	void  OnPrintPreview();
	void  OnReturn();
	void  OnPrint();
	void  GetSevenDayStr( CDuiString * pDays, DWORD dwSize, time_t tFirstTime, BOOL bMonthDay = FALSE );
	void  OnFinalMessage(HWND hWnd);
	void  SetBusy(BOOL bBusy = TRUE);
	void  OnPatientInfoRet(WPARAM wParam, LPARAM  lParam);
	void  OnPatientDataRet(WPARAM wParam, LPARAM  lParam);
	// 从界面获取填入的info
	void  GetPatientInfo(PatientInfo * pInfo, std::vector<PatientEvent * > & vEvents);
	// 从界面获取填入的data
	void  GetPatientData(PatientData * pData, DWORD dwSize);
	// 病人info是否改变
	BOOL  IsPatientInfoChanged(PatientInfo * pInfo);
	// 病人data是否改变
	BOOL  IsPatientDataChanged(PatientData * pData, int nIndex);
	// 时间范围的起始时间改变
	void  OnDateStartChanged();
	// 时间控件失去焦点
	void  OnDateStartKillFocus();
	// 选中了事件UI
	void  OnMyEventSelected(CControlUI * pCtl);
	// 
	void  OnAddMyEvent(CMyEventUI * pEventUI);
	//
	void  OnDelMyEvent();
	// 大于0的显示字符串数字，否则显示空白
	CDuiString  FormatInt(int nValue);
	// 自动填充体温
	void  OnAutoTemp();
	DWORD  GetTemp(time_t tStart, time_t tEnd);
	// 上一页，下一页
	void  OnPrevPage();
	void  OnNextPage();

	// 上一页，下一页
	void  OnPrevPage_1();
	void  OnNextPage_1();

	void  OnInHospitalCheckbox();
	void  OnInHospitalKillfocus();

private:
	CMyTreeCfgUI  *             m_tree;
	CTabLayoutUI *              m_switch;
	CWaitingBarUI *             m_waiting_bar;
	CButtonUI *                 m_btnPreview;
	CButtonUI *                 m_btnPrint;
	CButtonUI *                 m_btnReturn;

	BOOL                        m_bBusy;
	PatientInfo                 m_patient_info;
	std::vector<PatientEvent *> m_vEvents;
	PatientData                 m_patient_data[7];

	CPatientDataPrintPreviewUI *  m_preview;

	CMyDateUI *               m_date_start;
	CMyDateUI *               m_date_end;
	CVerticalLayoutUI *       m_lay_events;
	int                       m_selected_event;

	CTempUI *                 m_cur_temp;
	CTempUI *                 m_instant_temp;
	std::vector<TempItem *>   m_VTemp;

	CSevenGridsUI *           m_OtherData_Week;
	CPatientImg *             m_img;
	CHorizontalLayoutUI *     m_layImg;

	CButtonUI *               m_btnPrev;
	CButtonUI *               m_btnNext;
	CButtonUI *               m_btnZoomout;
	CButtonUI *               m_btnZoomin;

	CLabelUI *                m_lblPageIndex;
	CCheckBoxUI *             m_chInHospital;

public:
	char                        m_szTagId[MAX_TAG_ID_LENGTH];              // Tag Id
	char                        m_szUIPName[MAX_TAG_PNAME_LENGTH];         // UI 传过来的name
	time_t                      m_out_hospital_time;                       // 出院日期
};