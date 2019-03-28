#pragma once
#include "common.h"

class CPatinfoCardUI : public CContainerUI
{
public:
	CPatinfoCardUI();
	~CPatinfoCardUI();

private:
	LPCTSTR GetClass() const;

	void DoInit();
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	CDuiString      m_strPatientName;
	CLabelUI*       m_pPatName;

	CDuiString      m_strPatientSex;
	CLabelUI*       m_pPatSex;

	CDuiString      m_strPatientAge;
	CLabelUI*       m_pPatAge;

	CDuiString      m_strPatientBirth;
	CLabelUI*       m_pPatBirth;

	CDuiString      m_strPatientTel;
	CLabelUI*       m_pPatTel;

	CDuiString      m_strPatientId;
	CLabelUI*       m_pPatId;

	CDuiString      m_strPatientImg;
	CControlUI *    m_pPatImage;
};
