#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;


class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile() { return _T("main.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);

	// �����Զ�����Ϣ
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CVerticalLayoutUI *           m_layMain;
	CHorizontalLayoutUI *         m_layLayer[LAY_LAYER_COUNT];
	CControlUI *                  m_chart[MYCHART_COUNT];
	int                           m_chart_state;

	// �˵���ť
	DuiLib::CButtonUI *             m_btnMenu;

private:
	void   OnDbClick();

	void   OnSetting();
	void   OnAbout();

};
