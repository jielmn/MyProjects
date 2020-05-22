#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

class CTempRuleUI : public DuiLib::CControlUI
{
public:
	CTempRuleUI();
	~CTempRuleUI();
	LPCTSTR GetClass() const;

	static int  RULE_WIDTH;
	static int  RULE_HEIGHT;
	static int  MAX_TEMP;

private:
	bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	void DoEvent(DuiLib::TEventUI& event);

private:
	HPEN      m_hPen;
	HBRUSH    m_hBrush;
	int       m_nTemp;
	HFONT     m_hFont;
};

class CDeviceUI : public CContainerUI, INotifyUI
{
public:
	CDeviceUI();
	~CDeviceUI();

	static int FIXED_WIDTH;
	static int FIXED_HEIGHT;

private:
	LPCTSTR GetClass() const;
	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void DoEvent(DuiLib::TEventUI& event);
	void Notify(TNotifyUI& msg);

private:
	CDialogBuilderCallbackEx                    m_callback;

	CTempRuleUI *                               m_rule;
};


