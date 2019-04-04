#pragma once
#include "common.h"

class CComDataUI : public CContainerUI
{
public:
	CComDataUI();
	~CComDataUI();

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
};
