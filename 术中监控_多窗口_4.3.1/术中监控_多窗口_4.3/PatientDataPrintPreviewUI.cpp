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
	//SET_CLIP_REGION_ON_PAINT(rcPaint);
	PrepareXmlChart(m_XmlChartFile, &m_patient_info, m_patient_data, 7, m_tFirstDay);

	int w = 760;
	int h = 1044;
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hBitmap  = CreateCompatibleBitmap(hDC, w, h); 
	HBITMAP hOld = SelectBitmap(hMemDC, hBitmap);
	::Rectangle(hMemDC, 0, 0, w, h);

	//PrintXmlChart(hDC, m_XmlChartFile, m_rcItem.left, m_rcItem.top, m_patient_data, 7);  
	PrintXmlChart(hMemDC, m_XmlChartFile, 0, 0, m_patient_data, 7);
	StretchBlt( hDC, m_rcItem.left, m_rcItem.top, 524,   
		           720, hMemDC, 0, 0, 760, 1044, SRCCOPY);

	SelectBitmap(hMemDC, hOld);
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);

	return true;
}

void CPatientDataPrintPreviewUI::DoEvent(DuiLib::TEventUI& event) {
	CControlUI::DoEvent(event);   
}

LPCTSTR CPatientDataPrintPreviewUI::GetClass() const {
	return "PatientDataPrintPreview";
}