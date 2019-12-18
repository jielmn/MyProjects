#pragma once
#include "common.h"

class CEdtComboUI : public  CContainerUI, INotifyUI {
public:
	CEdtComboUI();
	~CEdtComboUI();

private:
	LPCTSTR GetClass() const;
	void DoInit();
	bool OnMySize(void * pParam);
	void Notify(TNotifyUI& msg);

public:
	CComboUI *             m_cmb;
	CEditUI *              m_edt;
};

class CGridUI : public CContainerUI, INotifyUI {
public:
	CGridUI();
	~CGridUI();

private:
	LPCTSTR GetClass() const;
	void DoInit();
	void Notify(TNotifyUI& msg);
};