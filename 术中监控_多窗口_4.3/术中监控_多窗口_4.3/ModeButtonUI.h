#pragma once
#include "common.h"

class CModeButton : public CButtonUI
{
public:
	enum Mode
	{
		Mode_Hand = 0,    // �ֳ�
		Mode_Single,      // ��������
		Mode_Multiple     // �������
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
