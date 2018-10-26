#pragma once

#include "common.h"
class CDuiFrameWnd;

class CMyImageUI : public DuiLib::CControlUI
{
public:
	CMyImageUI();
	~CMyImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	void  OnChangeSkin();

private:
	time_t  GetFirstTime();
	time_t  GetLastTime();
	void    DrawTempPoint(int nIndex, Graphics & g, int x, int y, HDC hDc, int RADIUS = DEFAULT_POINT_RADIUS);

private:
	HPEN                         m_hCommonThreadPen;
	HPEN                         m_hBrighterThreadPen;
	HBRUSH                       m_hCommonBrush;
	HPEN                         m_hLowTempAlarmPen;
	HPEN                         m_hHighTempAlarmPen;	
	Pen *                        m_temperature_pen[MAX_READERS_PER_GRID];
	SolidBrush *                 m_temperature_brush[MAX_READERS_PER_GRID];
	Pen                          m_remark_pen;
	SolidBrush                   m_remark_brush;

private:
	vector<TempData *>           m_vTempData[MAX_READERS_PER_GRID];
	DWORD                        m_dwSelectedReaderIndex;
};

