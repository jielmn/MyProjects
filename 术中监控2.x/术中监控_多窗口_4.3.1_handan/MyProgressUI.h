#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

class CMyProgressUI : public DuiLib::CProgressUI
{
public:
	virtual LPCTSTR GetClass() const {
		return "MyProgress";
	}

private:
	virtual void DoEvent(DuiLib::TEventUI& event);
};