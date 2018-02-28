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

	vector<TempData *>   m_vTempData;
	
	CPaintManagerUI *    m_pManager;

	int                  m_nLeft;
	int                  m_nGridSize;
	int                  m_nTimeUnitLen;
	int                  m_nTextOffsetX;
	int                  m_nTextOffsetY;
	int                  m_nTimeOffsetX;
	int                  m_nTimeOffsetY;
	int                  m_nMaxTimeSpan;

	void DrawTempPoint(int x, int y, HDC hDc, int RADIUS = 6);
};

#define  MYIMAGE_TIMER_ID             10
