#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

class CDeskUI : public CContainerUI, INotifyUI
{
public:
	CDeskUI();
	~CDeskUI();
	void  SetHighlight(BOOL bHighlight);
	BOOL  IsHighlight();
	void  SetValid(BOOL bValid);

	// ¸ßÁÁ±ß¿ò
	void  SetHighlightBorder(BOOL bHighlight);

private:
	LPCTSTR GetClass() const;
	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void DoEvent(DuiLib::TEventUI& event);
	void Notify(TNotifyUI& msg);

private:
	CControlUI *                         m_Sex;
	CLabelUI *                           m_lblName;
	CLabelUI *                           m_lblTemp;
	CLabelUI *                           m_lblDate;
	CLabelUI *                           m_lblTime;
	CControlUI *                         m_Warning;
	CButtonUI *                          m_btnDel;
	CControlUI *                         m_total;

public:
	DeskItem                             m_data;
	BOOL                                 m_bHighlight;

public:
	void                                 UpdateUI();
};