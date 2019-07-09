
#pragma once
#include "UIlib.h"
using namespace DuiLib;

class CDragDropUI : public CControlUI      //����̳���CContainerUI����Ҳ���Լ̳�����ؼ�����CButtonUI�ȣ�ʵ���Զ���ؼ�
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
