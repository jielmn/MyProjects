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
	void  OnMouseWheel(BOOL bPositive);

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
	void    DrawPolyline( time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel, 
		int  nHighestTemp, int nPixelPerCelsius, POINT  tTopLeft, Graphics & graphics,
		BOOL  bDrawPoints = FALSE );

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
	std::string                  m_sTagId[MAX_READERS_PER_GRID];
	DWORD                        m_dwNextTempIndex;
	BOOL                         m_bSetParentScrollPos;
	E_TYPE                       m_type;
	DWORD                        m_dwCurTempIndex;
	E_STATE                      m_state;
	int                          m_nSingleDayIndex;
	time_t                       m_SingleDayZeroTime;
	float                        m_fSecondsPerPixel;    // 用于单天的记录每个像素代表的点
	BOOL                         m_bSetSecondsPerPixel;

private:
	void   SubPaint_0(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	void   SubPaint_1(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	void   CalcSingleDay(time_t & tMin, BOOL & bFindMin, time_t & tMax, BOOL & bFindMax);

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

	void  SetTagId(DWORD  dwSubIndex, const char * szTagId);

	//
	void   PaintForLabelUI(HDC hDC, int width, int height, const RECT & rect);
};



class CMyLabelUI : public DuiLib::CLabelUI
{
public:
	CMyLabelUI();
	~CMyLabelUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const {
		return "MyLabel";
	}

	void  SetMyImage(CMyImageUI * img) {
		m_image = img;
	}

private:
	CMyImageUI  *   m_image;
};


//////////////////////////////////////////////////////////////////////////
// new image

class CMyImageUI_1 : public DuiLib::CControlUI
{
public:
	CMyImageUI_1();
	~CMyImageUI_1();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const {
		return "MyImage_1";
	}

	void  OnDbClick();
	void  MyInvalidate();

private:
	enum   E_STATE {
		STATE_7_DAYS = 0,
		STATE_SINGLE_DAY
	};

	vector< vector<TempData *> * >               m_vData;
	vector< string * >                           m_vTagId;
	int                                          m_nCurIndex;
	E_STATE                                      m_state;
	int                                          m_nSingleDayIndex;
	time_t                                       m_SingleDayZeroTime;
	BOOL                                         m_bSetSecondsPerPixel;

	HPEN                                         m_hCommonThreadPen;
	HPEN                                         m_hBrighterThreadPen;
	HBRUSH                                       m_hCommonBrush;
	HPEN                                         m_hDaySplitThreadPen;
	Pen   *                                      m_temperature_pen;
	Pen                                          m_remark_pen;
	SolidBrush                                   m_remark_brush;

private:
	void   SubPaint_0(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	void   SubPaint_1(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	// 7日视图有几天数据
	int     GetDayCounts();
	time_t  GetFirstTime();
	void    DrawPolyline(time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel,
		int  nHighestTemp, int nPixelPerCelsius, POINT  tTopLeft, Graphics & graphics,
		BOOL  bDrawPoints = FALSE);
	
};
