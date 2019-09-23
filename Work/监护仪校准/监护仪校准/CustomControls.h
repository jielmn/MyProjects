#pragma once
#include "common.h"

class CTempItemUI : public CContainerUI, INotifyUI
{
public:
	CTempItemUI();
	~CTempItemUI();
	void  SetTemp(int nTemp);
	int   GetTemp() const;
	void  SetDutyCycle(int nDutyCycle);
	int   GetDutyCycle() const;
	BOOL  IsChecked() const;
	void  SetChecked(BOOL bChecked);

private:
	LPCTSTR GetClass() const;
	void DoInit();
	void  SetTemp();
	void  SetDutyCycle();
	void  SetChecked();
	virtual void DoEvent(DuiLib::TEventUI& event);
	void Notify(TNotifyUI& msg);

public:
	static const int  WIDTH;
	static const int  HEIGHT;

private:
	COptionUI *                    m_opBasePoint;
	CLabelUI *                     m_lblTemp;
	CLabelUI *                     m_lblDutyCycle;
	CButtonUI *                    m_btnUp;
	CButtonUI *                    m_btnDown;
	CButtonUI *                    m_btnAdjust;

private:
	int                            m_nTemp;
	int                            m_nDutyCycle;
	BOOL                           m_bHighlight;
	BOOL                           m_bChecked;
};