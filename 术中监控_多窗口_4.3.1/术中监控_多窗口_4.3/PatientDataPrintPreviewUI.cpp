#include "PatientDataPrintPreviewUI.h"
#include "resource.h"

CPatientDataPrintPreviewUI::CPatientDataPrintPreviewUI() {
	memset( &m_patient_info, 0, sizeof(PatientInfo) );
	memset( m_patient_data,  0, sizeof(PatientData) * 7 );
	m_tFirstDay = 0;

	HRSRC hResource = ::FindResource(0, MAKEINTRESOURCE(IDR_XML1), "XML");
	if (hResource != NULL) {
		// 加载资源
		HGLOBAL hg = LoadResource(0, hResource);
		if (hg != NULL) {
			// 锁定资源
			LPVOID pData = LockResource(hg);
			if (pData != NULL) {
				// 获取资源大小
				DWORD dwSize = SizeofResource(0, hResource);
				m_XmlChartFile.ReadXmlChartStr((const char *)pData, dwSize);
			}
		}
	}
}

bool CPatientDataPrintPreviewUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);	

	PrepareXmlChart(m_XmlChartFile, &m_patient_info, m_patient_data, 7, m_tFirstDay);

	SetBkMode(hDC, TRANSPARENT);
	DrawXml2ChartUI(hDC, m_XmlChartFile.m_ChartUI, m_rcItem.left, m_rcItem.top);
	return true;
}

void CPatientDataPrintPreviewUI::DoEvent(DuiLib::TEventUI& event) {
	CControlUI::DoEvent(event);   
}

LPCTSTR CPatientDataPrintPreviewUI::GetClass() const {
	return "PatientDataPrintPreview";
}