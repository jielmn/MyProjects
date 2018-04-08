#pragma once

#include "zsCommon.h"
#include "Business.h"
#include "UIlib.h"
using namespace DuiLib;

class CPatientWnd : public WindowImplBase
{
public:
	CPatientWnd(BOOL  bAdd = TRUE);

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIPatientFrame"); }
	virtual CDuiString GetSkinFile() { return _T("Patient.xml"); }
	virtual CDuiString GetSkinFolder() { return _T("res"); }

	virtual void   Notify(TNotifyUI& msg);
	virtual void   InitWindow();

	PatientInfo    m_tPatientInfo;

	LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	BOOL    m_bAdd;                    // 是添加新病人信息，还是修改病人信息

	DuiLib::CEditUI *              m_edPatientId;
	DuiLib::CEditUI *              m_edPatientName;
	DuiLib::COptionUI *            m_opMale;
	DuiLib::COptionUI *            m_opFemale;
	DuiLib::COptionUI *            m_opInHos;
	DuiLib::COptionUI *            m_opOutHos;
	DuiLib::CEditUI *              m_edBedNo;
	DuiLib::CHorizontalLayoutUI *  m_lyTags;
	DuiLib::CListUI *              m_lstTags;
};