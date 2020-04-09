#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

class CDeskUI : public CContainerUI
{
public:
	CDeskUI();
	~CDeskUI();

private:
	LPCTSTR GetClass() const;
	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	CControlUI *                         m_Sex;
	CLabelUI *                           m_lblName;
	CLabelUI *                           m_lblTemp;
	CLabelUI *                           m_lblDate;
	CLabelUI *                           m_lblTime;
	CControlUI *                         m_Warning;
	CButtonUI *                          m_btnDel;
};