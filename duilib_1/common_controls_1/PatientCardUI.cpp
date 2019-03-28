#include "PatientCardUI.h"

CPatinfoCardUI::CPatinfoCardUI()
{
	m_strPatientImg = "man.png";
}

CPatinfoCardUI::~CPatinfoCardUI()
{

}

LPCTSTR CPatinfoCardUI::GetClass()const {
	return _T("PatientCard");
}


void CPatinfoCardUI::DoInit()
{
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(_T("PatientCard.xml"), (UINT)0, NULL, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
	}
	else {
		this->RemoveAll();
	}

	m_pPatName = static_cast<CLabelUI*>(m_pManager->FindControl(_T("PatName")));
	m_pPatName->SetText(m_strPatientName);  

	m_pPatSex = static_cast<CLabelUI*>(m_pManager->FindControl(_T("PatSex")));
	m_pPatSex->SetText(m_strPatientSex);

	m_pPatAge = static_cast<CLabelUI*>(m_pManager->FindControl(_T("PatAge")));
	m_pPatAge->SetText(m_strPatientAge);

	m_pPatBirth = static_cast<CLabelUI*>(m_pManager->FindControl(_T("PatBirth")));
	m_pPatBirth->SetText(m_strPatientBirth);

	m_pPatTel = static_cast<CLabelUI*>(m_pManager->FindControl(_T("PatTel")));
	m_pPatTel->SetText(m_strPatientTel);

	m_pPatId = static_cast<CLabelUI*>(m_pManager->FindControl(_T("PatID")));
	m_pPatId->SetText(m_strPatientId);

	m_pPatImage = m_pManager->FindControl(_T("PatImage"));
	m_pPatImage->SetBkImage(m_strPatientImg);
}

void CPatinfoCardUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (0 == StrICmp(pstrName, "patient_name")) {
		m_strPatientName = pstrValue;
	}
	else if (0 == StrICmp(pstrName, "sex")) {
		m_strPatientSex = pstrValue;
	}
	else if (0 == StrICmp(pstrName, "age")) {
		m_strPatientAge = pstrValue; 
	}
	else if (0 == StrICmp(pstrName, "birth")) {
		m_strPatientBirth = pstrValue;
	}
	else if (0 == StrICmp(pstrName, "tel")) {
		m_strPatientTel = pstrValue;
	}
	else if (0 == StrICmp(pstrName, "id")) {
		m_strPatientId = pstrValue;  
	}
	else if (0 == StrICmp(pstrName, "patient_image")) {
		m_strPatientImg = pstrValue;   
	}
	else {
		CContainerUI::SetAttribute(pstrName, pstrValue);   
	}	
}

