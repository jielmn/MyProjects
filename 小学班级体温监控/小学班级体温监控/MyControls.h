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
};