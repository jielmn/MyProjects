#pragma once

#include "common.h"

class CMyImageUI : public DuiLib::CControlUI
{
public:
	CMyImageUI(DuiLib::CPaintManagerUI *pManager);
	~CMyImageUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);

	void  AddTemp(DWORD dwTemp);
	void  MyInvalidate();
	void  SetState(int nNewState);
	void  SetIndex(int nIndex);
	void  OnChangeSkin();
	void  OnMyClick(const POINT * pPoint);

private:
	DuiLib::CPaintManagerUI *    m_pManager;
	vector<TempData *>           m_vTempData;
	int                          m_nState;     // 0 小格子状态，1 全屏状态
	BOOL                         m_bSetParentScrollPos;
	int                          m_nIndex;     // 序列号

	Pen                          m_temperature_pen;
	SolidBrush                   m_temperature_brush;

private:
	int    CalcMinWidth();   // 计算图像需要的宽度
	void   DrawTempPoint(Graphics & g, int x, int y, HDC hDc, int RADIUS = DEFAULT_POINT_RADIUS);
};