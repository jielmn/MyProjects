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

private:
	void   DoPaint_7Days(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	void   DoPaint_SingleDay(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	// ���㱾�ؼ���ռλ�ط��Ŀ��
	int    GetMyWidth();
	int    GetMyScrollX();
	DWORD  GetGridIndex();
	DWORD  GetReaderIndex();
	const  std::vector<TempItem * > & GetTempData(DWORD j);
	CModeButton::Mode   GetMode();
	CControlUI * GetGrid();
	int    GetCelsiusHeight(int height, int nCelsiusCount);
	// ��ˮƽ�̶���
	void   DrawScaleLine( HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY,
		                  const RECT & rectScale, const RECT & rect );
	// ���߿�
	void   DrawBorder(HDC hDC, const RECT & rectScale, int width);
	// ���̶�ֵ
	void   DrawScale( HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY, int nMaxTemp,
		              const RECT & rectScale, int width );
	// ��������
	void   DrawWarning( HDC hDC, DWORD i, DWORD j, int nMaxTemp, int nHeightPerCelsius, int nMaxY,
		                const RECT & rectScale, int width );
	// 7����ͼ�м�������
	int     GetDayCounts(DWORD i, DWORD j, CModeButton::Mode mode);
	time_t  GetFirstTime(DWORD i, DWORD j, CModeButton::Mode mode);
	// �����ӷָ���
	void    DrawDaySplit( HDC hDC, int nDayCounts, const RECT & rectScale, int nDayWidth, int nMaxY,
		                  int nCelsiusCnt, int nHeightPerCelsius, time_t  tFirstDayZeroTime);
	// ������ͼ ��Χ[tFirstTime, tLastTime)
	void    DrawPolyline( time_t tFirstTime, time_t tLastTime,      float fSecondsPerPixel,
		                  int    nMaxTemp,   int nHeightPerCelsius, POINT  tTopLeft,    Graphics & graphics,
		                  BOOL  bDrawPoints, DWORD i, DWORD j, CModeButton::Mode mode );
	void    DrawPolyline( time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel,
		                  int    nMaxTemp, int nHeightPerCelsius, POINT  tTopLeft, Graphics & graphics,
		                  BOOL  bDrawPoints, const  std::vector<TempItem * > & vTempData, 
		                  Pen * pen, SolidBrush * brush);
	void    DrawPoint(SolidBrush * brush, Graphics & g, int x, int y, HDC hDc, int radius);
	// ˫���¼�
	void    OnDbClick();
	// �¶����ݸ���
	DWORD   GetTempCount(DWORD i, DWORD j, CModeButton::Mode mode);

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
	Pen *                        m_temperature_pen[MAX_READERS_PER_GRID];
	SolidBrush *                 m_temperature_brush[MAX_READERS_PER_GRID];
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