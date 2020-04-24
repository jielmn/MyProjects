#pragma once
#include "common.h"

class CMyChartUI : public DuiLib::CControlUI
{
public:
	CMyChartUI();
	~CMyChartUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const;

private:
	std::map<int, std::vector<int>* >     m_data;
};

class CMySliderUI : public DuiLib::CContainerUI
{
public:
	CMySliderUI();
	~CMySliderUI();

	virtual LPCTSTR GetClass() const;
	void DoInit();
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

private:
	CControlUI *                    m_ctl_1;
	CControlUI *                    m_ctl_2;
	CControlUI *                    m_butt_1;
	CControlUI *                    m_butt_2;
	CControlUI *                    m_block;
	BOOL                            m_bStartMove;
	int                             m_nMoveSource;
	int                             m_nMovePos;
	RECT                            m_rcMove;

	double                           m_fPos;                // 起点
	double                           m_fLength;             // 长度

private:
	bool  OnMySize(void * pParam);
	void  CalcData();
};

class CMyVSliderUI : public DuiLib::CContainerUI
{
public:
	CMyVSliderUI();
	~CMyVSliderUI();

	virtual LPCTSTR GetClass() const;
	void DoInit();
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

private:
	CControlUI *                    m_ctl_1;
	CControlUI *                    m_ctl_2;
	CControlUI *                    m_butt_1;
	CControlUI *                    m_butt_2;
	CControlUI *                    m_block;
	BOOL                            m_bStartMove;
	int                             m_nMoveSource;
	int                             m_nMovePos;	
	RECT                            m_rcMove;

	float                           m_fPos;                // 起点
	float                           m_fLength;             // 长度

private:
	bool  OnMySize(void * pParam);
	void  CalcData();
};