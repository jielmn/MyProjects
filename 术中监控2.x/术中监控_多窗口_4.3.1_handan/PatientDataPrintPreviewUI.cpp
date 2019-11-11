#include "PatientDataPrintPreviewUI.h"
#include "resource.h"

CPatientDataPrintPreviewUI::CPatientDataPrintPreviewUI() {
	memset( &m_patient_info, 0, sizeof(PatientInfo) );
	memset( m_patient_data,  0, sizeof(PatientData) * 7 );
	m_tFirstDay = 0;

	m_nOriginalHeight = 720;
	m_fConstRatio = (float)XML_CHART_WIDTH / (float)XML_CHART_HEIGHT;
	m_fZoom = 1.0f;

	LoadXmlChart(m_XmlChartFile);
}

CPatientDataPrintPreviewUI::~CPatientDataPrintPreviewUI() {
	ClearVector(m_vEvents);
}

bool CPatientDataPrintPreviewUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);		
	//SET_CLIP_REGION_ON_PAINT(rcPaint);
	PrepareXmlChart(m_XmlChartFile, &m_patient_info, m_patient_data, 7, m_tFirstDay, m_vEvents );

	int w = XML_CHART_WIDTH;
	int h = XML_CHART_HEIGHT;
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hBitmap  = CreateCompatibleBitmap(hDC, w, h); 
	HBITMAP hOld = SelectBitmap(hMemDC, hBitmap);
	::Rectangle(hMemDC, 0, 0, w, h);

	//PrintXmlChart(hDC, m_XmlChartFile, m_rcItem.left, m_rcItem.top, m_patient_data, 7); 
	int h1 = (int)(m_nOriginalHeight * m_fZoom);
	int w1 = (int)(h1 * m_fConstRatio);

	PrintXmlChart(hMemDC, m_XmlChartFile, 0, 0, m_patient_data, 7,m_tFirstDay, m_vEvents, m_patient_info);

	// µÚ¶þ¸ömem dc
	HDC hMemDC_1 = CreateCompatibleDC(hDC);
	HBITMAP hBitmap_1 = CreateCompatibleBitmap(hDC, w1, h1);
	HBITMAP hOld_1 =  (HBITMAP)SelectObject(hMemDC_1, hBitmap_1);
	StretchBlt(hMemDC_1, 0, 0, w1, h1, hMemDC, 0, 0, w, h, SRCCOPY); 

	BitBlt(hDC, m_rcItem.left, m_rcItem.top, w1,
		h1, hMemDC_1, 0, 0, SRCCOPY);

	SelectBitmap(hMemDC, hOld);
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);

	SelectBitmap(hMemDC_1, hOld_1);
	DeleteObject(hBitmap_1);
	DeleteDC(hMemDC_1);

	return true;
}

void CPatientDataPrintPreviewUI::DoEvent(DuiLib::TEventUI& event) {
	CControlUI::DoEvent(event);   
}

LPCTSTR CPatientDataPrintPreviewUI::GetClass() const {
	return "PatientDataPrintPreview";
}

void CPatientDataPrintPreviewUI::ZoomIn() {
	if ( m_fZoom >= 1.4f ) {
		return;
	}

	m_fZoom += 0.1f;
	if (m_fZoom > 1.4f) {
		m_fZoom = 1.4f; 
	}

	int h = (int)(m_nOriginalHeight * m_fZoom);
	int w = (int)(h * m_fConstRatio);
	this->SetFixedWidth(w);
	this->SetFixedHeight(h);

	this->Invalidate();
}

void CPatientDataPrintPreviewUI::ZoomOut() {
	if (m_fZoom <= 1.0f) {
		return;
	}

	m_fZoom -= 0.1f;
	if (m_fZoom < 1.0f) {
		m_fZoom = 1.0f;
	}

	int h = (int)(m_nOriginalHeight * m_fZoom);
	int w = (int)(h * m_fConstRatio);
	this->SetFixedWidth(w);
	this->SetFixedHeight(h);

	this->Invalidate();
}