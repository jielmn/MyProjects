#pragma once
#include "common.h"

class CEdtComboUI : public  CContainerUI, INotifyUI {
public:
	CEdtComboUI();
	~CEdtComboUI();

private:
	LPCTSTR GetClass() const;
	void DoInit();
	bool OnMySize(void * pParam);
	void Notify(TNotifyUI& msg);

public:
	CComboUI *             m_cmb;
	CEditUI *              m_edt;
};

class CGridUI : public CContainerUI, INotifyUI {
public:
	CGridUI();
	~CGridUI();

	void SetIndex(int nIndex);
	int  GetIndex();

	void SetUserName(CDuiString strName);
	CDuiString  GetUserName();

	void SetHeartBeat(int nHeartBeat);
	void SetHeartBeat(CWearItem * pItem);
	int  GetHeartBeat();

	void SetOxy(int nOxy);
	void SetOxy(CWearItem * pItem);
	int  GetOxy();

	void SetTemp(int nTemp);
	void SetTemp(CWearItem * pItem);
	int  GetTemp();

	void SetPose(int nPose);
	int  GetPose();

	void SetDeviceId(CDuiString strId);
	CDuiString  GetDeviceId();

	void CheckWarning();

private:
	LPCTSTR GetClass() const;
	void DoInit();
	void Notify(TNotifyUI& msg);

	void SetIndex();
	void SetUserName();
	void SetHeartBeat();
	void SetOxy();
	void SetTemp();
	void SetDeviceId();
	void SetPose();            

private:
	CDialogBuilderCallbackEx                    m_callback;
	CLabelUI *                                  m_lblIndex;
	CEdtComboUI *                               m_cstName;
	CLabelUI *                                  m_lblHeartBeat;
	CLabelUI *                                  m_lblOxy;
	CLabelUI *                                  m_lblTemp;
	CLabelUI *                                  m_lblDeviceId;
	CLabelUI *                                  m_lblPose;
	CVerticalLayoutUI *                         m_layMain;

private:
	int           m_nIndex;
	CDuiString    m_strName;
	int           m_nHeartBeat;
	int           m_nOxy;
	int           m_nTemp;
	int           m_nPose;
	CDuiString    m_strDeviceId;
};

#define  MYIMAGE_V_MARGIN      40
#define  MYIMAGE_H_MARGIN      50

#define  MAX_SECONDS_PER_PIXEL               200.0f
#define  MIN_SECONDS_PER_PIXEL               1.0f

class CMyImageUI : public DuiLib::CControlUI {
public:
	CMyImageUI();
	~CMyImageUI();

	bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	void DoEvent(DuiLib::TEventUI& event);
	LPCTSTR GetClass() const;
	void Clear();
	void OnGetDataRet(CGetDataRet * pRet);
	void MyInvalidate();
	void OnNewWearItem(CWearItem * pItem);

private:
	std::vector<DataItem *>          m_vHeartBeat;
	std::vector<DataItem *>          m_vOxy;
	std::vector<DataItem *>          m_vTemp;

	float                            m_fSecondsPerPixel;

public:
	BOOL                             m_bSetSecondsPerPixel;

private:
	HPEN                             m_hCommonThreadPen;	
	HBRUSH                           m_hCommonBrush;
	HPEN                             m_hDaySplitThreadPen;
	Pen *                            m_temperature_pen;
	SolidBrush *                     m_temperature_brush;
	Pen *                            m_heartbeat_pen;
	SolidBrush *                     m_heartbeat_brush;
	Pen *                            m_oxy_pen;
	SolidBrush *                     m_oxy_brush;

public:
	CDuiString                       m_strDeviceId;

private:
	int   GetMyWidth();
	int   GetMyScrollX();
	void  GetMaxMinScale(int & nMinTemp, int & nMaxTemp, int & nMinScale, int & nMaxScale);
	int   GetCelsiusHeight(int height, int nCelsiusCount, int nVMargin = MYIMAGE_V_MARGIN);
	void  DrawScale( HDC hDC, int nMaxTemp, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY,
		             const RECT & rectScale1, const RECT & rectScale2, int width );
	// �ұ߿̶�
	void  DrawScale2 (HDC hDC, int nMaxTemp, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY,
		const RECT & rectScale1, const RECT & rectScale2, int width);

	BOOL  HasData();
	void  GetTimeRange( time_t & tFirst, time_t & tLast );
	void  DrawDaySplit( HDC  hDC, time_t tFirst, time_t tLast, int nLeft, int nTop, int nBottom, int nVMargin, int nMaxX);
	void  DrawPolyline(time_t tFirstTime, time_t tLastTime, float fSecondsPerPixel,
		int nMaxValue, float fHeightPerUnit, POINT  tTopLeft, Graphics & graphics,
		const std::vector<DataItem * > & vData, Pen * pen, SolidBrush * brush);
	void  DrawPoint(SolidBrush * brush, Graphics & g, int x, int y, HDC hDc, int radius);

	// ���ֻ���
	void   OnMyMouseWheel(WPARAM wParam, LPARAM lParam);
	void   DrawTimeText(HDC hDC, time_t  tFirstTime, time_t tLastTime,
		float fSecondsPerPixel, POINT  top_left, const RECT & rValid);
};