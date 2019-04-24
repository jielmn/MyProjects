#pragma once

#include "common.h"

class CGridUI : public CContainerUI
{
public:
	CGridUI();
	~CGridUI();

	void  SetBedNo(DWORD dwIndex);
	// Ë«»÷ºóµÄÊÓÍ¼ÇÐ»»
	void  SwitchView();

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	BOOL                                        m_bInited;
	CDialogBuilderCallbackEx                    m_callback;
	DuiLib::CTabLayoutUI *                      m_tabs;

	CButtonUI *                                 m_btnBedNo;
	CButtonUI *                                 m_btnBedNoM;
	DWORD                                       m_dwBedNo;

	CLabelUI *                                  m_lblReaderNo;
	DWORD                                       m_dwReaderNo;
};
