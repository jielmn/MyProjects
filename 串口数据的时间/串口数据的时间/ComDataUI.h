#pragma once
#include "common.h"

class CComDataUI : public CContainerUI
{
public:
	CComDataUI();
	~CComDataUI();

	void SetDataText(LPCTSTR pstrText);
	void SetTimeText(LPCTSTR pstrText);

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	CRichEditUI *              m_richData;
	CLabelUI *                 m_lblTime;
	CDuiString                 m_strData;
	CDuiString                 m_strTime;
};
