#pragma once

#include "common.h"
#include "ModeButtonUI.h"

class CMyImageUI : public DuiLib::CControlUI
{
public:
	CMyImageUI();
	~CMyImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const;
	void MyInvalidate();
	// 为label作画
	void   PaintForLabelUI(HDC hDC, int width, int height, const RECT & rect);

private:
	void   DoPaint_7Days(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	void   DoPaint_SingleDay(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	// 计算本控件的占位地方的宽度
	int    GetMyWidth();
	int    GetMyScrollX();
	DWORD  GetGridIndex();
	DWORD  GetReaderIndex();
	const  std::vector<TempItem * > & GetTempData(DWORD j);
	CModeButton::Mode   GetMode();
	CControlUI * GetGrid();
	int    GetCelsiusHeight(int height, int nCelsiusCount);
	// 画水平刻度线
	void   DrawScaleLine( HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY,
		                  const RECT & rectScale, const RECT & rect );
	// 画边框
	void   DrawBorder(HDC hDC, const RECT & rectScale, int width);
	// 画刻度值
	void   DrawScale( HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY, int nMaxTemp,
		              const RECT & rectScale, int width );
	// 画报警线
	void   DrawWarning( HDC hDC, DWORD i, DWORD j, int nMaxTemp, int nHeightPerCelsius, int nMaxY,
		                const RECT & rectScale, int width );
	// 7日视图有几天数据
	int     GetDayCounts(DWORD i, DWORD j, CModeButton::Mode mode);
	time_t  GetFirstTime(DWORD i, DWORD j, CModeButton::Mode mode);
	// 画日子分割线
	void    DrawDaySplit( HDC hDC, int nDayCounts, const RECT & rectScale, int nDayWidth, int nMaxY,
		                  int nCelsiusCnt, int nHeightPerCelsius, time_t  tFirstDayZeroTime);
	// 画折线图 范围[tFirstTime, tLastTime]
	void    DrawPolyline( time_t tFirstTime, time_t tLastTime,      float fSecondsPerPixel,
		                  int    nMaxTemp,   int nHeightPerCelsius, POINT  tTopLeft,    Graphics & graphics,
		                  BOOL  bDrawPoints, DWORD i, DWORD j, CModeButton::Mode mode );
	void    DrawPolyline( time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel,
		                  int    nMaxTemp, int nHeightPerCelsius, POINT  tTopLeft, Graphics & graphics,
		                  BOOL  bDrawPoints, const  std::vector<TempItem * > & vTempData, 
		                  Pen * pen, SolidBrush * brush);
	void    DrawPoint(SolidBrush * brush, Graphics & g, int x, int y, HDC hDc, int radius);
	// 画时间文本
	void    DrawTimeText(HDC hDC, time_t  tFirstTime, time_t tLastTime, float fSecondsPerPixel, POINT  top_left);
	// 双击事件
	void    OnDbClick();
	// 温度数据个数
	DWORD   GetTempCount(DWORD i, DWORD j, CModeButton::Mode mode);
	// 计算双击了第几天
	int     GetClickDayIndex(DWORD i, DWORD j, CModeButton::Mode mode);
	// 获得single day的起始时间和结束时间
	BOOL    GetSingleDayTimeRange(time_t & start, time_t & end, DWORD i, DWORD j, CModeButton::Mode mode);
	BOOL    GetTimeRange(const std::vector<TempItem * > & v, time_t & start, time_t & end);
	// 鼠轮滑动
	void   OnMyMouseWheel(WPARAM wParam, LPARAM lParam);

private:
	enum   E_STATE {
		STATE_7_DAYS = 0,
		STATE_SINGLE_DAY
	};

	E_STATE                      m_state;                 // 7days or single day
	int                          m_nSingleDayIndex;       // 单日视图，第几天
	HPEN                         m_hCommonThreadPen;
	HPEN                         m_hBrighterThreadPen;
	HBRUSH                       m_hCommonBrush;
	HPEN                         m_hLowTempAlarmPen;
	HPEN                         m_hHighTempAlarmPen;
	HPEN                         m_hDaySplitThreadPen;
	Pen *                        m_temperature_pen[MAX_READERS_PER_GRID];
	SolidBrush *                 m_temperature_brush[MAX_READERS_PER_GRID];
	float                        m_fSecondsPerPixel;
	BOOL                         m_bSetSecondsPerPixel;
};

class CImageLabelUI : public DuiLib::CLabelUI
{
public:
	CImageLabelUI();
	~CImageLabelUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const;

	void  SetMyImage(CMyImageUI * img) {
		m_image = img;
	}

private:
	CMyImageUI  *   m_image;
};