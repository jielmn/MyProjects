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


private:
	enum   E_STATE {
		STATE_7_DAYS = 0,
		STATE_SINGLE_DAY
	};

	E_STATE                      m_state;                 // 7days or single day
	HPEN                         m_hCommonThreadPen;
	HPEN                         m_hBrighterThreadPen;
	HBRUSH                       m_hCommonBrush;
	HPEN                         m_hLowTempAlarmPen;
	HPEN                         m_hHighTempAlarmPen;
	HPEN                         m_hDaySplitThreadPen;
};

class CImageLabelUI : public DuiLib::CLabelUI
{
public:
	CImageLabelUI();
	~CImageLabelUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const;
};