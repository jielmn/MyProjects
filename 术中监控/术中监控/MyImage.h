#pragma once

#include "exhCommon.h"
#include "UIlib.h"

class CMyImageUI : public DuiLib::CControlUI
{
public:
	CMyImageUI(DuiLib::CPaintManagerUI *pManager);
	~CMyImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);

	void  AddTemp(DWORD dwTemp);

	void  SetWndRect(int x, int y);

private:
	vector<TempData *>   m_vTempData;

	DuiLib::CPaintManagerUI *    m_pManager;

	void DrawTempPoint(Graphics & g, int x, int y, HDC hDc, int RADIUS = 6);

private:
	int                  m_nLeft;
	int                  m_nGridSize;
	int                  m_nTimeUnitLen;
	int                  m_nTextOffsetX;
	int                  m_nTextOffsetY;
	int                  m_nTimeOffsetX;
	int                  m_nTimeOffsetY;
	int                  m_nMaxPointsCnt;
	int                  m_nRadius;

	//int                  m_nMinTemp;
	//int                  m_nGridCount;

	HPEN                 m_hPen;
	HPEN                 m_hPen1;    // 低温线
	HPEN                 m_hPen2;    // 高温线

	HPEN                 m_hPen3;     // 数据线

	DWORD                m_dwTextColor;

	Pen                  m_pen_3;
	SolidBrush           m_brush_3;
};

#define  MYIMAGE_TIMER_ID             10


class CAlarmImageUI : public DuiLib::CControlUI
{
public:
	CAlarmImageUI(DuiLib::CPaintManagerUI *pManager);
	~CAlarmImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);

	void  HighTempAlarm();
	void  LowTempAlarm();
	void  FailureAlarm();
	void  StopAlarm();

private:
	DuiLib::CPaintManagerUI *    m_pManager;
	BOOL                         m_bSetBkImage;
	DuiLib::CDuiString           m_strBkImage;
};

