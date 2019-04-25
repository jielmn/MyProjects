#pragma once

#include "common.h"

class CReaderUI : public CContainerUI
{
public:
	CReaderUI();
	~CReaderUI();

	void  SetIndicator(DWORD dwIndex);

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	BOOL                                        m_bInited;
	CDialogBuilderCallbackEx                    m_callback;

	CLabelUI *                                  m_lblIndicator;           // A~F指示器
	DWORD                                       m_dwIndicator;
	CControlUI *                                m_indicator;              // 颜色指示器
};
