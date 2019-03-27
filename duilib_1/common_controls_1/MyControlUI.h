#pragma once

#include "common.h"

// �Զ���ؼ����������Զ���Paint�������϶��ؼ�
class CMyControlUI : public DuiLib::CControlUI
{
public:
	CMyControlUI();
	~CMyControlUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const {
		return "MyControl";
	}

	bool OnMySize(void * pParam);

public:
	// �ָ�Ϊ2*2=4��
	int        m_nSplitX;                       // ��ֱ�ָ���
	int        m_nSplitY;                       // ˮƽ�ָ���

private:
	BOOL       m_bPainted;                      // �Ƿ�DoPaint������Ϊֻ����paint��ʱ��GetPos����Ч
	HBRUSH     m_brushs[4];
};