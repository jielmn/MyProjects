#pragma once

#include "common.h"
#include "UIlib.h"
using namespace DuiLib;

class CAreaWnd : public WindowImplBase
{
public:
	CAreaWnd(BOOL  bAdd = TRUE);

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIAreaFrame"); }
	virtual CDuiString GetSkinFile() { return _T("Area.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }

	virtual void  Notify(TNotifyUI& msg);
	virtual void   InitWindow();

	TArea    m_tArea;

private:
	void  OnMyOk();

private:
	BOOL    m_bAdd;  
	DuiLib::CTextUI *      m_lblTitle;
	DuiLib::CEditUI *      m_edtAreaNo;
	DuiLib::CEditUI *      m_edtAreaName;
};

