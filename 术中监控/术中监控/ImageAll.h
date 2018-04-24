#pragma once

#include "exhCommon.h"
#include "UIlib.h"

class CImageAll : public DuiLib::CControlUI
{
public:
	CImageAll(DuiLib::CPaintManagerUI *pManager);
	~CImageAll();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);

private:
	void   DrawTempPoint(int x, int y, HDC hDc, int RADIUS = 6);

private:
	std::vector<TempData * >    m_vTempData;

	HPEN                 m_hPen;
	HPEN                 m_hPen1;

private:
	int                         m_nHeightPerCelsius;   // 每度高度
	int                         m_nWidthPerSecond;     // 每秒宽度
	int                         m_nTextOffsetX;
	int                         m_nTextOffsetY;
	int                         m_nTextLeft;
	int                         m_nCelsiusCnt;
	int                         m_nTimeTextOffsetX;

};