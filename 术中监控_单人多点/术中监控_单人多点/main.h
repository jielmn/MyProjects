#pragma once

#include "common.h"
#include "AlarmImage.h"
#include "MyImage.h"
#include "UIlib.h"
using namespace DuiLib;



class CDuiFrameWnd : public WindowImplBase
{
public:
	CDuiFrameWnd();
	~CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_CLASS_WINDOW_NAME); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_FILE); }
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void   OnMyLButtonDown(WPARAM wParam, LPARAM lParam);
	void   OnLayReaderSelected(DWORD dwIndex);

	void   OnBtnPatientName(TNotifyUI& msg);
	void   OnBtnPatientSex(TNotifyUI& msg);
	void   OnBtnPatientAge(TNotifyUI& msg);
	void   OnKillFocusPatientName(TNotifyUI& msg);
	void   OnKillFocusPatientSex(TNotifyUI& msg);
	void   OnKillFocusPatientAge(TNotifyUI& msg);
	void   OnBtnReaderName(TNotifyUI& msg);
	void   OnKillFocusReaderName(TNotifyUI& msg);

private:
	CDialogBuilderCallbackEx               m_callback;

	DuiLib::CVerticalLayoutUI *            m_layReaders;
	CControlUI *                           m_pUiReaders[MAX_READERS_COUNT];
	COptionUI *                            m_pUiReaderSwitch[MAX_READERS_COUNT];
	CAlarmImageUI *                        m_pUiAlarms[MAX_READERS_COUNT];
	CLabelUI *                             m_pUiReaderTemp[MAX_READERS_COUNT];
	CButtonUI *                            m_pUiBtnReaderNames[MAX_READERS_COUNT];
	CEditUI *                              m_pUiEdtReaderNames[MAX_READERS_COUNT];
	CHorizontalLayoutUI *                  m_pUiLayReader[MAX_READERS_COUNT];

	CButtonUI *                            m_pUiBtnPatientName;
	CEditUI *                              m_pUiEdtPatientName;
	CButtonUI *                            m_pUiBtnSex;
	CEditUI *                              m_pUiEdtSex;
	CButtonUI *                            m_pUiBtnAge;
	CEditUI *                              m_pUiEdtAge;

	CMyImageUI *                           m_pUiMyImage;
};
