#pragma once
#include "common.h"

class CTempItemUI : public CContainerUI
{
public:
	CTempItemUI();
	~CTempItemUI();
	void  SetTemp(int nTemp);
	int   GetTemp() const;
	void  SetDutyCycle(int nDutyCycle);
	int   GetDutyCycle() const;

private:
	LPCTSTR GetClass() const;
	void DoInit();
	void  SetTemp();
	void  SetDutyCycle();

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
};