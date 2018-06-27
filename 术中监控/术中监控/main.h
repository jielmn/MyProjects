#pragma once
#include "UIlib.h"
#include "MyImage.h"

class CDuiFrameWnd : public DuiLib::WindowImplBase
{
public:
	CDuiFrameWnd();
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("main.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void      InitWindow();
	virtual void      Notify(DuiLib::TNotifyUI& msg);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

	// 处理自定义信息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	DuiLib::CButtonUI *             m_btnMenu;
	DuiLib::CLabelUI *              m_lblReaderStatus;
	CMyImageUI *                    m_pImageUI;
	DuiLib::CLabelUI *              m_lblTemperature;
	CAlarmImageUI *                 m_pAlarmUI;
	DuiLib::CTextUI *               m_txtTitle;

	DuiLib::CLabelUI *              m_lblLowTemperature;
	DuiLib::CLabelUI *              m_lblHighTemperature;
	DuiLib::CLabelUI *              m_lblTimeLeft;

	int                             m_nWidth;
	int                             m_nHeight;

	DWORD                           m_dwLowTempColor;
	DWORD                           m_dwNormalColor;
	DWORD                           m_dwHighTempColor;

	int                             m_nLowestTemp;
	int                             m_nHighestTemp;

	int                             m_nTimeLeft;

private:
	void  OnSetting();
	void  OnAbout();
	void  OnImageAll();

	void  OnTemperatureResult(int nRet,DWORD dwTemp = 0);
	void  ShowTimeLeft();
	void  ShowTimeLeftError();
};