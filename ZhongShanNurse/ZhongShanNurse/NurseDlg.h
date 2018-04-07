#pragma once

#include "zsCommon.h"
#include "Business.h"
#include "UIlib.h"
using namespace DuiLib;

class CNurseWnd : public WindowImplBase
{
public:
	CNurseWnd(BOOL  bAdd = TRUE);

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIPatientFrame"); }
	virtual CDuiString GetSkinFile() { return _T("Nurse.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }

	virtual void   Notify(TNotifyUI& msg);
	virtual void   InitWindow();

	NurseInfo    m_tNurseInfo;

	LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	BOOL    m_bAdd;                    // 是添加新护士信息，还是修改护士信息

	DuiLib::CEditUI *              m_edNurseId;
	DuiLib::CEditUI *              m_edNurseName;
};
