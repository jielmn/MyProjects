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
	// 得到最大和最小显示温度
	void   GetMaxMinShowTemp(int & nMinTemp, int & nMaxTemp, DWORD i, DWORD j, CModeButton::Mode mode);
	void   GetMaxMinShowTemp(int & nMinTemp, int & nMaxTemp, BOOL & bFirst, const std::vector<TempItem * > & v );
	// 画十字线
	void   DrawCrossLine( HDC hDC, const RECT & rValid, const POINT & cursor_point,
		                  time_t tFirstTime, float fSecondsPerPixel, int nHeightPerCelsius,
		                  int nMaxY, int nMaxTemp);
	// 拖放操作开始
	void  BeginDragDrop();
	// 检查鼠标是否需要改变指针
	void  CheckCursor(const POINT & pt);
	// 正在拖放操作
	void  DragDropIng(const POINT & pt);
	// 结束拖放操作
	void  EndDragDrop(const POINT & pt);

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

	/***** 拖放操作 ****/
	enum DragDropObj {
		DragDrop_None = 0,
		DragDrop_LowAlarm,
		DragDrop_HighAlarm
	};

	BOOL                  m_bDragDrop;                 // 是否开始拖放操作
	DragDropObj           m_DragDropObj;               // 拖放操作开始后，拖放的对象
	DragDropObj           m_CursorObj;                 // 拖放操作开始前，鼠标滑动时经过的对象
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