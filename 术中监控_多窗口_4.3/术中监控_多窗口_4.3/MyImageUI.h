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
	// Ϊlabel����
	void   PaintForLabelUI(HDC hDC, int width, int height, const RECT & rect);

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
	// ������ͼ ��Χ[tFirstTime, tLastTime]
	void    DrawPolyline( time_t tFirstTime, time_t tLastTime,      float fSecondsPerPixel,
		                  int    nMaxTemp,   int nHeightPerCelsius, POINT  tTopLeft,    Graphics & graphics,
		                  BOOL  bDrawPoints, DWORD i, DWORD j, CModeButton::Mode mode );
	void    DrawPolyline( time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel,
		                  int    nMaxTemp, int nHeightPerCelsius, POINT  tTopLeft, Graphics & graphics,
		                  BOOL  bDrawPoints, const  std::vector<TempItem * > & vTempData, 
		                  Pen * pen, SolidBrush * brush);
	void    DrawPoint(SolidBrush * brush, Graphics & g, int x, int y, HDC hDc, int radius);
	// ��ʱ���ı�
	void    DrawTimeText(HDC hDC, time_t  tFirstTime, time_t tLastTime, float fSecondsPerPixel, POINT  top_left);
	// ˫���¼�
	void    OnDbClick();
	// �¶����ݸ���
	DWORD   GetTempCount(DWORD i, DWORD j, CModeButton::Mode mode);
	// ����˫���˵ڼ���
	int     GetClickDayIndex(DWORD i, DWORD j, CModeButton::Mode mode);
	// ���single day����ʼʱ��ͽ���ʱ��
	BOOL    GetSingleDayTimeRange(time_t & start, time_t & end, DWORD i, DWORD j, CModeButton::Mode mode);
	BOOL    GetTimeRange(const std::vector<TempItem * > & v, time_t & start, time_t & end);
	// ���ֻ���
	void   OnMyMouseWheel(WPARAM wParam, LPARAM lParam);

private:
	enum   E_STATE {
		STATE_7_DAYS = 0,
		STATE_SINGLE_DAY
	};

	E_STATE                      m_state;                 // 7days or single day
	int                          m_nSingleDayIndex;       // ������ͼ���ڼ���
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