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

public:
	static const int  WIDTH;
	static const int  HEIGHT;
};