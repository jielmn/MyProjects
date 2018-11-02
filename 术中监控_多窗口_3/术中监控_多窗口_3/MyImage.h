#pragma once

#include "common.h"
#include "sigslot.h"
class CDuiFrameWnd;

class CMyImageUI : public DuiLib::CControlUI
{
public:
	enum E_TYPE {
		TYPE_GRID = 0,
		TYPE_MAX,
	};
	CMyImageUI(E_TYPE e);
	~CMyImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	void  OnChangeSkin();
	void  AddTemp(DWORD dwIndex, DWORD dwTemp);
	void  MyInvalidate();

private:
	time_t  GetFirstTime();
	time_t  GetLastTime();
	void    DrawTempPoint(int nIndex, Graphics & g, int x, int y, HDC hDc, int RADIUS = DEFAULT_POINT_RADIUS);	
	int     CalcMinWidth();

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
	DWORD                        m_dwNextTempIndex;
	BOOL                         m_bSetParentScrollPos;
	E_TYPE                       m_type;

public:
	sigslot::signal1<DWORD>      m_sigUpdateScroll;
	sigslot::signal0<>           m_sigAlarm;
	DWORD                        m_dwSelectedReaderIndex;

public:
	void                         OnReaderSelected(DWORD  dwSelectedIndex);
};

