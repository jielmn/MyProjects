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

class CMyImageUI : public DuiLib::CControlUI {
public:
	CMyImageUI();
	~CMyImageUI();

	bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	void DoEvent(DuiLib::TEventUI& event);
	LPCTSTR GetClass() const;

private:
	std::vector<DataItem *>          m_vHeartBeat;
	std::vector<DataItem *>          m_vOxy;
	std::vector<DataItem *>          m_vTemp;

	float                            m_fSecondsPerPixel;

private:
	HPEN                             m_hCommonThreadPen;
	HPEN                             m_hDaySplitThreadPen;

private:
	int   GetMyWidth();
	int   GetMyScrollX();
	void  GetMaxMinScale(int & nMinTemp, int & nMaxTemp, int & nMinScale, int & nMaxScale);
	int   GetCelsiusHeight(int height, int nCelsiusCount, int nVMargin = MYIMAGE_V_MARGIN);
	void  DrawScale( HDC hDC, int nMaxTemp, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY,
		             const RECT & rectScale1, const RECT & rectScale2, int width );
	BOOL  HasData();
	void  GetTimeRange( time_t & tFirst, time_t & tLast );
	void  DrawDaySplit( HDC  hDC, time_t tFirst, time_t tLast, int nLeft, int nTop, int nBottom, int nVMargin);
};