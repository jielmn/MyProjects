#pragma once

#include "common.h"
#include "ModeButtonUI.h"

class CMyImageUI : public DuiLib::CControlUI
{
public:
	CMyImageUI();
	virtual ~CMyImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const;
	virtual void MyInvalidate();
	// 为label作画
	void   PaintForLabelUI(HDC hDC, int width, int height, const RECT & rect);
	virtual void   SetRemark(DuiLib::CDuiString & strRemark);	

protected:
	virtual void   DoPaint_7Days(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void   DoPaint_SingleDay(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	// 计算本控件的占位地方的宽度
	int    GetMyWidth();
	int    GetMyScrollX();
	virtual DWORD  GetGridIndex();
	virtual DWORD  GetReaderIndex();
	const  std::vector<TempItem * > & GetTempData(DWORD j);
	virtual CModeButton::Mode   GetMode();
	CControlUI * GetGrid();
	int    GetCelsiusHeight(int height, int nCelsiusCount,int nVMargin = MIN_MYIMAGE_VMARGIN);
	// 画水平刻度线
	void   DrawScaleLine( HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY,
		                  const RECT & rectScale, const RECT & rect );
	// 画边框
	void   DrawBorder(HDC hDC, const RECT & rectScale, int width);
	// 画刻度值
	void   DrawScale( HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY, int nMaxTemp,
		              const RECT & rectScale, int width, BOOL bDrawRectangle = TRUE, DWORD dwTextColor = RGB(255, 255, 255) );
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
	void    DrawTimeText(HDC hDC, time_t  tFirstTime, time_t tLastTime, float fSecondsPerPixel, POINT  top_left, const RECT & rValid);
	// 双击事件
	virtual void    OnDbClick();
	// 温度数据个数
	virtual DWORD   GetTempCount(DWORD i, DWORD j, CModeButton::Mode mode);
	// 计算双击了第几天
	virtual int     GetClickDayIndex(DWORD i, DWORD j, CModeButton::Mode mode);
	// 获得single day的起始时间和结束时间
	virtual BOOL    GetSingleDayTimeRange(time_t & start, time_t & end, DWORD i, DWORD j, CModeButton::Mode mode);
	BOOL    GetTimeRange(const std::vector<TempItem * > & v, time_t & start, time_t & end);
	// 鼠轮滑动
	void   OnMyMouseWheel(WPARAM wParam, LPARAM lParam);
	// 得到最大和最小显示温度
	virtual void   GetMaxMinShowTemp(int & nMinTemp, int & nMaxTemp, DWORD i, DWORD j, CModeButton::Mode mode);
	virtual void   GetMaxMinShowTemp(int & nMinTemp, int & nMaxTemp, BOOL & bFirst, const std::vector<TempItem * > & v );
	// 画十字线
	void   DrawCrossLine( HDC hDC, const RECT & rValid, const POINT & cursor_point,
		                  time_t tFirstTime, float fSecondsPerPixel, int nMaxTemp, int nHeightPerCelsius,
		                  POINT  top_left );
	// 拖放操作开始
	void  BeginDragDrop();
	// 检查鼠标是否需要改变指针
	virtual void  CheckCursor(const POINT & pt);
	// 正在拖放操作
	void  DragDropIng(const POINT & pt);
	// 结束拖放操作
	void  EndDragDrop(const POINT & pt);
	// 检查是否点击了注释
	virtual void  CheckRemark(const POINT & pt);
	virtual BOOL  CheckRemark( const POINT & pt, const std::vector<TempItem * > & v, 
		               time_t tFirstTime, float fSecondsPerPixel,
		               int    nMaxTemp,   int nHeightPerCelsius, POINT  top_left );
	// 设置注释
	BOOL  SetRemark( const std::vector<TempItem * > & v, DWORD  dwDbId, DuiLib::CDuiString & strRemark );
	// 画注释
	void  DrawRemark( HDC hDC, Graphics & g, time_t tFirstTime, float fSecondsPerPixel,
		              int nMaxTemp, int nHeightPerCelsius, POINT  top_left,
		              DWORD i, DWORD j, CModeButton::Mode mode, const RECT & rValid );
	void  DrawRemark( HDC hDC, Graphics & g, time_t tFirstTime, float fSecondsPerPixel,
		              int nMaxTemp, int nHeightPerCelsius, POINT  top_left,
		              const std::vector<TempItem * > & v, const RECT & rValid);

protected:
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
	DWORD                 m_dwRemarkingIndex;          // 正在编辑的点

	Pen *                 m_remark_pen;
	SolidBrush *          m_remark_brush;
};

class CImageLabelUI : public DuiLib::CLabelUI
{
public:
	CImageLabelUI();
	~CImageLabelUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const;

	// 设置透明色
	void SetOpacityTextColor(DWORD  dwColor) {
		SetTextColor( (dwColor & 0xFFFFFF) | (GRID_LABLE_TRANSPARENT_PARAM << 24) );
	}

	void  SetMyImage(CMyImageUI * img) {
		m_image = img;
	}

private:
	CMyImageUI  *   m_image;
};

class CMyHandImage : public CMyImageUI {
public:
	CMyHandImage();
	~CMyHandImage();
	void  OnHandTemp( TempItem * pTemp, BOOL & bNewTag );
	void  OnHandTempVec(vector<TempItem *> * pVec, const char * szTagId );
	// 删除过时的数据
	void  PruneData();
	void  SetCurTag(const char * szTagId);
	CDuiString   GetCurTagId();
	void   SetRemark(DuiLib::CDuiString & strRemark);

	sigslot::signal1<const char *>              m_sigTagErased;

	// 导出excel
	void  ExportExcel(const char * szPatientName);

	// 打印excel表格
	void  PrintExcel(const char * szPatientName);

	// 删除Tag
	void  DelTag(const char * szTagId);

private:
	virtual void   DoPaint_7Days(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void   DoPaint_SingleDay(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

	// 检查鼠标是否需要改变指针
	virtual void  CheckCursor(const POINT & pt);
	// 双击事件
	virtual void    OnDbClick();

	/***** 空起来 *****/
	virtual DWORD  GetGridIndex();
	virtual DWORD  GetReaderIndex();
	virtual CModeButton::Mode   GetMode();	
	/* end 空起来  */

	void   GetMaxMinShowTemp(int & nMinTemp, int & nMaxTemp);
	int    GetDayCounts();
	int    GetClickDayIndex();
	void   MyInvalidate();
	DWORD  GetTempCount();
	BOOL   GetSingleDayTimeRange(time_t & start, time_t & end);
	void   CheckRemark(const POINT & pt);
	BOOL   CheckRemark(const POINT & pt, const std::vector<TempItem * > & v,
		time_t tFirstTime, float fSecondsPerPixel,
		int    nMaxTemp, int nHeightPerCelsius, POINT  top_left);

	void Clear();
	void PruneData(std::vector<TempItem*> & v, time_t t);

private:
	// 所有的手持读卡器的数据，按照tag id进行分类
	//           tag id,   对应的数据集合
	std::map<std::string, vector<TempItem *> *>        m_data;
	// 当前显示的是哪个数据
	std::string                                        m_cur_tag;
};