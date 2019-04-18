#pragma once

#include "common.h"

class CGridUI : public CContainerUI
{
public:
	CGridUI();
	~CGridUI();

	void  SetBedNo(DWORD dwIndex);

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	CDialogBuilderCallbackEx                    m_callback;

	CButtonUI *                                 m_btnBedNo;
	DWORD                                       m_dwBedNo;

	CLabelUI *                                  m_lblReaderNo;
	DWORD                                       m_dwReaderNo;
};
