#pragma once
#include "common.h"
#include "UIlib.h"
using namespace DuiLib;

class CMyImageUI : public CControlUI
{
public:
	CMyImageUI(CPaintManagerUI *pManager);
	~CMyImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

	virtual void DoEvent(TEventUI& event);

	void  AddTemp(DWORD dwTemp);

private:
	HPEN                 m_hPen;
	HPEN                 m_hPen1;
	HPEN                 m_hPen2;

	DWORD                m_dwTimeIndex;
	vector<TempData *>   m_vTempData;
	DWORD                m_dwTempDataLimit;

	int                  m_nSubGridIndex;
	CPaintManagerUI *    m_pManager;

	void DrawTempPoint(int x, int y, HDC hDc, int RADIUS = 6);
};

#define  MYIMAGE_TIMER_ID             10