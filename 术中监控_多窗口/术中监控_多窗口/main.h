#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "MyImage.h"


class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CDialogBuilderCallbackEx(DuiLib::CPaintManagerUI *pManager) {
		m_pManager = pManager;
	}
	CControlUI* CreateControl(LPCTSTR pstrClass);
private:
	DuiLib::CPaintManagerUI *  m_pManager;
};

class CDuiFrameWnd : public WindowImplBase
{
public:
	CDuiFrameWnd();
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame_surgery"); }
	virtual CDuiString GetSkinFile() { return _T("main.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);

	// 处理自定义信息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CVerticalLayoutUI *           m_layMain;
	CHorizontalLayoutUI *         m_layLayer[LAY_LAYER_COUNT];
	CControlUI *                  m_chart[MYCHART_COUNT];
	int                           m_chart_state;

	// 菜单按钮
	DuiLib::CButtonUI *             m_btnMenu;

	CDialogBuilderCallbackEx        m_callback;

	CMyImageUI *                    m_ChartUi[MYCHART_COUNT];

private:
	void   OnDbClick();

	void   OnSetting();
	void   OnAbout();

	void   OnReaderStatus(WPARAM wParam, LPARAM lParam);
	void   OnReaderTemp(WPARAM wParam, LPARAM lParam);
	void   OnTemperatureResult(int nIndex, int nRet, DWORD dwTemp = 0);


private:
	DuiLib::CEditUI *               m_edtBedNo[MYCHART_COUNT];
	DuiLib::CEditUI *               m_edtName[MYCHART_COUNT];
	DuiLib::CEditUI *               m_edtSex[MYCHART_COUNT];
	DuiLib::CEditUI *               m_edtAge[MYCHART_COUNT];

	DuiLib::CButtonUI *             m_btnBedNo[MYCHART_COUNT];
	DuiLib::CButtonUI *             m_btnName[MYCHART_COUNT];
	DuiLib::CButtonUI *             m_btnSex[MYCHART_COUNT];
	DuiLib::CButtonUI *             m_btnAge[MYCHART_COUNT];

	DuiLib::CLabelUI *              m_lblReaderStatus[MYCHART_COUNT];
	DuiLib::CLabelUI *              m_lblReaderComPort[MYCHART_COUNT];
	DuiLib::CLabelUI *              m_lblTemperature[MYCHART_COUNT];

	DWORD                           m_dwLowTempColor;
	DWORD                           m_dwNormalColor;
	DWORD                           m_dwHighTempColor;

	int                             m_nLowestTemp[MYCHART_COUNT];
	int                             m_nHighestTemp[MYCHART_COUNT];
	int                             m_nTimeLeft[MYCHART_COUNT];

	DuiLib::CLabelUI *              m_lblLowTemperature[MYCHART_COUNT];
	DuiLib::CLabelUI *              m_lblHighTemperature[MYCHART_COUNT];
};
