
#pragma once
#include "UIlib.h"
using namespace DuiLib;

class CNetDevice : public CContainerUI      //����̳���CContainerUI����Ҳ���Լ̳�����ؼ�����CButtonUI�ȣ�ʵ���Զ���ؼ�
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
