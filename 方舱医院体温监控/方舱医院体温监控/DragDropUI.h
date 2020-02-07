#pragma once

#include "UIlib.h"
using namespace DuiLib;

class CDragDropUI : public CControlUI {
public:
	enum DragDropType {
		DragDropType_ANY = 0,
		DragDropType_WE,
		DragDropType_NS,		
	};

	CDragDropUI();
	~CDragDropUI();

	virtual LPCTSTR GetClass() const { return "DragDropUI"; }
	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

	void  SetCursor(HCURSOR h);
	void  SetDragDropType(DragDropType e);
	void  SetBoundingRect(const RECT & r);

private:
	HCURSOR       m_cursor;
	RECT          m_rcNewPos;
	DragDropType  m_eDragDropType;
	RECT          m_rcBounding;
	BOOL          m_bBounding;
};
