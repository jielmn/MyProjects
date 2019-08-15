#pragma once
#include "common.h"

class CSixGridsUI : public CContainerUI
{
public:
	CSixGridsUI();
	~CSixGridsUI();
	void  SetMode(int nMode);
	void  SetValues(int nIndex, const char * szValue);
	CDuiString  GetValues(int nIndex);
	void  SetNumberOnly(BOOL bOnly);

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	CEditUI *              m_edits[6];
	CHorizontalLayoutUI *  m_top;
	CHorizontalLayoutUI *  m_bottom;
	int                    m_nMode;
	BOOL                   m_bNumberOnly;
};

class CSevenGridsUI : public CContainerUI
{
public:
	CSevenGridsUI();
	~CSevenGridsUI();
	void  SetMode(int nMode);
	void  SetNumberOnly(BOOL bOnly);
	void  SetWeekStr(CDuiString * pWeek, DWORD dwSize);
	void  SetFont(int nFont);
	void  SetValues(int nIndex,const char * szValue);	
	CDuiString  GetValues(int nIndex);

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	CEditUI *              m_edits[7];
	CLabelUI *             m_labels[7];
	CHorizontalLayoutUI *  m_top;
	CHorizontalLayoutUI *  m_bottom;
	int                    m_nMode;
	BOOL                   m_bNumberOnly;
	int                    m_nFont;
	CDuiString             m_week_days[7];
};


class CShiftUI : public CContainerUI
{
public:
	CShiftUI();
	~CShiftUI();
	void  SetValues(int nIndex, const char * szValue);
	CDuiString  GetValues(int nIndex);

private:
	LPCTSTR GetClass() const;
	void DoInit();

private:
	CEditUI *              m_edits[2];
};

class CMyDateUI : public  CDateTimeUI, INotifyUI {
public:
	CMyDateUI();
	~CMyDateUI();
	void SetMyTime(SYSTEMTIME* pst);

private:
	LPCTSTR GetClass() const;
	void Notify(TNotifyUI& msg);
	void DoInit();
};

class CMyDateTimeUI : public  CContainerUI {
public:
	CMyDateTimeUI();
	~CMyDateTimeUI();
	void SetTime(time_t t);
	time_t GetTime();

private:
	LPCTSTR GetClass() const;
	void DoInit();

	CDialogBuilderCallbackEx                    m_callback;
	CMyDateUI *                                 m_date;
	CEditUI   *                                 m_hour;
	CEditUI *                                   m_minute;
	time_t                                      m_time;
};


class CMyEventUI : public CContainerUI, INotifyUI
{
public:
	CMyEventUI();
	~CMyEventUI();
	void SetSelected(BOOL bSel);
	CHorizontalLayoutUI *                       m_lay_1;
	void GetValue(int & nDbId, int & nType, time_t & t1, time_t & t2);
	void SetValue(int nDbId, int nType, time_t t1, time_t t2 = 0);

private:
	LPCTSTR GetClass() const;
	void DoInit();
	void Notify(TNotifyUI& msg);
	void DoEvent(DuiLib::TEventUI& event);

private:
	CDialogBuilderCallbackEx                    m_callback;

	CComboUI *                                  m_cmbType;
	CDateTimeUI *                               m_date_1;
	CEditUI *                                   m_edt_1;
	CEditUI *                                   m_edt_2;

	CDateTimeUI *                               m_date_2;
	CEditUI *                                   m_edt_3;
	CEditUI *                                   m_edt_4;
	CHorizontalLayoutUI *                       m_lay_2;

	CControlUI *                                m_sel;	
	BOOL                                        m_bSelected;

	int                                         m_nType;
	time_t                                      m_time1;
	time_t                                      m_time2;
};

class CTempUI : public  CContainerUI, INotifyUI {
public:
	CTempUI();
	~CTempUI();

	void GetValue(int & t1, int &t2);
	void SetValue(int t1, int t2 = 0);

private:
	LPCTSTR GetClass() const; 
	void DoInit();
	void Notify(TNotifyUI& msg);

	CEditUI   *                                 m_temp1;
	CEditUI   *                                 m_temp2;
	CButtonUI *                                 m_btn;

	int                                         m_t1;
	int                                         m_t2;
};


class CSixTempUI : public  CContainerUI {
public:
	CSixTempUI();
	~CSixTempUI();

	void  SetValues(int nIndex, int t1, int t2 = 0);
	void  GetValues(int nIndex, int & t1, int & t2);

private:
	LPCTSTR GetClass() const;
	void DoInit();

private:
	CDialogBuilderCallbackEx                    m_callback;

	CTempUI   *                                 m_temp[6];
	int                                         m_values[6][2];
};


class CPatientImg : public DuiLib::CControlUI
{
public:
	CPatientImg();
	~CPatientImg();

	bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	void DoEvent(DuiLib::TEventUI& event);
	LPCTSTR GetClass() const;

public:
	std::vector<TempItem *> *               m_pVec;
	time_t                                  m_tStart;
	time_t                                  m_tEnd;
	float                                   m_fSecondsPerPixel;

private:
	void  GetMaxMinShowTemp(int & nMinTemp, int & nMaxTemp);
	int   GetCelsiusHeight(int height, int nCelsiusCount, int nVMargin = MIN_MYIMAGE_VMARGIN);
	void  DrawScaleLine(HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY,
		const RECT & rectScale, const RECT & rect);
	void  DrawBorder(HDC hDC, const RECT & rectScale, int width);
	void  DrawScale(HDC hDC, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY, int nMaxTemp,
		const RECT & rectScale, int width, BOOL bDrawRectangle = TRUE,
		DWORD dwTextColor = RGB(255, 255, 255));
	// 温度数据个数
	DWORD  GetTempCount();
	void   DrawPolyline(time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel,
		int    nMaxTemp, int nHeightPerCelsius, POINT  tTopLeft, Graphics & graphics,
		BOOL  bDrawPoints,Pen * pen, SolidBrush * brush);
	void  DrawPoint(SolidBrush * brush, Graphics & g, int x, int y, HDC hDc, int radius);

private:
	HPEN                         m_hCommonThreadPen;
	HPEN                         m_hBrighterThreadPen;
	HBRUSH                       m_hCommonBrush;
	Pen *                        m_temperature_pen;
	SolidBrush *                 m_temperature_brush;
};