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
	virtual LPCTSTR GetClass() const {
		return "MyImage";
	}

	void  OnChangeSkin();
	void  AddTemp(DWORD dwIndex, DWORD dwTemp);
	void  MyInvalidate();
	void  OnTempSqliteRet(std::vector<TempData*> & vRet, DWORD  dwIndex);
	void  OnDbClick();

private:
	enum   E_STATE {
		STATE_7_DAYS = 0,
		STATE_SINGLE_DAY
	};

	time_t  GetFirstTime();
	time_t  GetLastTime();
	void    DrawTempPoint(int nIndex, Graphics & g, int x, int y, HDC hDc, int RADIUS = DEFAULT_POINT_RADIUS);	
	int     CalcMinWidth();
	// 7日视图有几天数据
	int     GetDayCounts();
	void    DrawPolyline( time_t tFirstDayZeroTime, float fSecondsPerPixel, 
		int  nHighestTemp, int nPixelPerCelsius, POINT  tTopLeft, Graphics & graphics );

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
	HPEN                         m_hDaySplitThreadPen;

private:
	vector<TempData *>           m_vTempData[MAX_READERS_PER_GRID];	
	DWORD                        m_dwNextTempIndex;
	BOOL                         m_bSetParentScrollPos;
	E_TYPE                       m_type;
	DWORD                        m_dwCurTempIndex;
	E_STATE                      m_state;
	int                          m_nSingleDayIndex;
	float                        m_fSecondsPerPixel;    // 用于单天的记录每个像素代表的点

private:
	void   SubPaint_0(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	void   SubPaint_1(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	void   SubPaint_2(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

public:
	sigslot::signal1<DWORD>      m_sigUpdateScroll;
	sigslot::signal0<>           m_sigAlarm;
	DWORD                        m_dwSelectedReaderIndex;

public:
	void   OnReaderSelected(DWORD  dwSelectedIndex);
	void   OnMyClick(const POINT * pPoint);
	void   SetRemark(DuiLib::CDuiString & strRemark);

	void  ExportExcel(const char * szPatientName);
	void  PrintExcel(char szReaderName[MAX_READERS_PER_GRID][64], const char * szPatientName);

	void  EmptyData();
	void  EmptyData(DWORD  dwSubIndex);
};

