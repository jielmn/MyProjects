#pragma once

#include "common.h"

class CTagUI : public CContainerUI, INotifyUI
{
public:
	CTagUI();
	~CTagUI();

private:
	LPCTSTR GetClass() const;
	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	BOOL                                        m_bInited;
	CDialogBuilderCallbackEx                    m_callback;
};
