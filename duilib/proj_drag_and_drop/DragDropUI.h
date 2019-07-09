
#pragma once
#include "UIlib.h"
using namespace DuiLib;

class CDragDropUI : public CControlUI      //这里继承了CContainerUI，你也可以继承任意控件类如CButtonUI等，实现自定义控件
{
public:
	CDragDropUI();
	~CDragDropUI();

	LPVOID GetInterface(LPCTSTR pstrName);
	void DoEvent(TEventUI& event);
	void DoPostPaint(HDC hDC, const RECT& rcPaint);

private:
	UINT m_uButtonState;
	POINT m_ptLastMouse;
	RECT m_rcNewPos;
};
