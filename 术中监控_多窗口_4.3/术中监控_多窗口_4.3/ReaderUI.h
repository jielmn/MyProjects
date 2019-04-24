#pragma once

#include "common.h"

class CReaderUI : public CContainerUI
{
public:
	CReaderUI();
	~CReaderUI();

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	BOOL                                        m_bInited;
	CDialogBuilderCallbackEx                    m_callback;
};
