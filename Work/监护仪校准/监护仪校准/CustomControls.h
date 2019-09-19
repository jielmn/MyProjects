#pragma once
#include "common.h"

class CTempItemUI : public CContainerUI
{
public:
	CTempItemUI();
	~CTempItemUI();

private:
	LPCTSTR GetClass() const;
	void DoInit();

private:
	
};