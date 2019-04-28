#pragma once
#include "common.h"

class CModeButton : public CButtonUI
{
public:
	enum Mode
	{
		Mode_Hand = 0,    // 手持
		Mode_Single,      // 单点连续
		Mode_Multiple     // 多点连续
	};

	CModeButton();
	~CModeButton();
	Mode GetMode() const;

private:
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const;
	void SwitchMode();

private:
	Mode                      m_mode;
};
