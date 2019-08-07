#include "PatientDataPrintPreviewUI.h"
#include "resource.h"

CPatientDataPrintPreviewUI::CPatientDataPrintPreviewUI() {
	memset( &m_patient_info, 0, sizeof(PatientInfo) );
	memset( m_patient_data,  0, sizeof(PatientData) * 7 );
	m_tFirstDay = 0;

	LoadXmlChart(m_XmlChartFile);
}

bool CPatientDataPrintPreviewUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);	

	PrepareXmlChart(m_XmlChartFile, &m_patient_info, m_patient_data, 7, m_tFirstDay);

	PrintXmlChart(hDC, m_XmlChartFile, m_rcItem.left, m_rcItem.top, m_patient_data, 7);  

	return true;
}

void CPatientDataPrintPreviewUI::DoEvent(DuiLib::TEventUI& event) {
	CControlUI::DoEvent(event);   
}

LPCTSTR CPatientDataPrintPreviewUI::GetClass() const {
	return "PatientDataPrintPreview";
}