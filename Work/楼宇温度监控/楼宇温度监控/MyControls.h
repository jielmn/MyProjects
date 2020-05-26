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
	void  SetTemp(int nTemp);

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
	void  SetHighlight(BOOL bHighlight);
	void  SetAddress(const char * szAddr);
	void  SetTemp(int nTemp, time_t tTime);
	void  SetDeviceId(int nDeviceId);

	int   GetDeviceId();

	static int FIXED_WIDTH;
	static int FIXED_HEIGHT;

private:
	LPCTSTR GetClass() const;
	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void DoEvent(DuiLib::TEventUI& event);
	void Notify(TNotifyUI& msg);

	void  SetAddress();
	void  SetTemp();
	void  SetDeviceId();

private:
	CDialogBuilderCallbackEx                    m_callback;

	CTempRuleUI *                               m_rule;
	CButtonUI *                                 m_btnDel;

	CLabelUI *                                  m_lblAddress;
	CLabelUI *                                  m_lblDeviceId;
	CLabelUI *                                  m_lblTemp;
	CLabelUI *                                  m_lblTempTime;


private:
	BOOL                                        m_bHighlight;
	CDuiString                                  m_strAddress;
	int                                         m_nDeviceId;
	int                                         m_nTemperature;
	time_t                                      m_tTime;
};


