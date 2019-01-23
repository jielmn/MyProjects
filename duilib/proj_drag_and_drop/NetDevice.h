
#pragma once
#include "UIlib.h"
using namespace DuiLib;

class CNetDevice : public CContainerUI      //这里继承了CContainerUI，你也可以继承任意控件类如CButtonUI等，实现自定义控件
{
public:
	CNetDevice();
	~CNetDevice();

	LPVOID GetInterface(LPCTSTR pstrName);
	void DoEvent(TEventUI& event);
	void DoPostPaint(HDC hDC, const RECT& rcPaint);

	//CControlUI* CreateControl(LPCTSTR pstrClass);

private:
	UINT m_uButtonState;
	POINT m_ptLastMouse;
	RECT m_rcNewPos;

	//CPaintManagerUI m_PaintManager;
};
