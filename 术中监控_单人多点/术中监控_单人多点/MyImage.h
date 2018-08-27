#pragma once

#include "common.h"

class CMyImageUI : public DuiLib::CControlUI
{
public:
	CMyImageUI();
	~CMyImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	void  AddTemp(DWORD dwIndex, DWORD dwTemp);
	void  MyInvalidate();
	void  SelectedReader(int nIndex);
	void  OnMyClick(const POINT * pPoint);
	void  SetRemark(DuiLib::CDuiString & strRemark);

private:
	time_t    GetFirstTime();
	time_t    GetLastTime();
	void      DrawTempPoint(int nIndex, Graphics & g, int x, int y, HDC hDc, int RADIUS = DEFAULT_POINT_RADIUS);
	int       CalcMinWidth();

private:
	vector<TempData *>           m_vTempData[MAX_READERS_COUNT];
	DWORD                        m_dwSelectedReaderIndex;
	DWORD                        m_dwNextTempIndex;
	BOOL                         m_bSetParentScrollPos;
	DWORD                        m_dwCurTempIndex;

private:
	HPEN                         m_hCommonThreadPen;
	HPEN                         m_hBrighterThreadPen;
	HBRUSH                       m_hCommonBrush;
	HPEN                         m_hLowTempAlarmPen;
	HPEN                         m_hHighTempAlarmPen;
	Pen *                        m_temperature_pen[MAX_READERS_COUNT];
	SolidBrush *                 m_temperature_brush[MAX_READERS_COUNT];
	Pen                          m_remark_pen;
	SolidBrush                   m_remark_brush;
};