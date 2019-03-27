#pragma once

#include "common.h"

// 自定义控件，在其上自定义Paint，试验拖动控件
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
	// 分割为2*2=4块
	int        m_nSplitX;                       // 垂直分割线
	int        m_nSplitY;                       // 水平分割线

private:
	BOOL       m_bPainted;                      // 是否DoPaint过。因为只有在paint的时候，GetPos才有效
	HBRUSH     m_brushs[4];
};