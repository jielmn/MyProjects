#pragma once

#include "Common.h"
#include "UIlib.h"

class CImageAll : public DuiLib::CControlUI
{
public:
	CImageAll(DuiLib::CPaintManagerUI *pManager);
	~CImageAll();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);

	void  AddTempData( DWORD dwTemp );

private:
	void   DrawTempPoint(int x, int y, HDC hDc, int RADIUS = 6);
	int    CalcMinWidth();

private:
	std::vector<TempData * >    m_vTempData;

	HPEN                 m_hPenGrid;
	HPEN                 m_hPenThread;
	HBRUSH               m_brush;

	BOOL                 m_bSetParentScrollPos;

};
