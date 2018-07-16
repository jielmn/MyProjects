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

	// 处理自定义信息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CVerticalLayoutUI *           m_layMain;
	CHorizontalLayoutUI *         m_layLayer[LAY_LAYER_COUNT];
	CControlUI *                  m_chart[MYCHART_COUNT];
	int                           m_chart_state;

	// 菜单按钮
	DuiLib::CButtonUI *             m_btnMenu;

private:
	void   OnDbClick();

	void   OnSetting();
	void   OnAbout();


private:
	DuiLib::CEditUI *               m_edtBedNo[MYCHART_COUNT];
	DuiLib::CEditUI *               m_edtName[MYCHART_COUNT];
	DuiLib::CEditUI *               m_edtSex[MYCHART_COUNT];
	DuiLib::CEditUI *               m_edtAge[MYCHART_COUNT];

	DuiLib::CButtonUI *             m_btnBedNo[MYCHART_COUNT];
	DuiLib::CButtonUI *             m_btnName[MYCHART_COUNT];
	DuiLib::CButtonUI *             m_btnSex[MYCHART_COUNT];
	DuiLib::CButtonUI *             m_btnAge[MYCHART_COUNT];
};
