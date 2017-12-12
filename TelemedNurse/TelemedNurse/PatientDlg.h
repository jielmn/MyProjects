#pragma once

#include "UIlib.h"
using namespace DuiLib;

class CPatientWnd : public WindowImplBase
{
public:
	CPatientWnd( BOOL  bAdd = TRUE );

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIPatientFrame"); }
	virtual CDuiString GetSkinFile() { return _T("Patient.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }

	virtual void  Notify(TNotifyUI& msg);
	virtual void   InitWindow();

private:
	BOOL    m_bAdd;                    // 是添加新病人信息，还是修改病人信息
};
