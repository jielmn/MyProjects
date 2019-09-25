#pragma once

#include "common.h"

class CGridUI : public CContainerUI, INotifyUI
{
public:
	CGridUI();
	~CGridUI();

private:
	LPCTSTR GetClass() const;
	void DoInit();
	void Notify(TNotifyUI& msg);	
};
