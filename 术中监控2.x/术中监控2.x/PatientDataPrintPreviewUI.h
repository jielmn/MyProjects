#pragma once
#include "common.h"
#include "UIlib.h"
using namespace DuiLib;

#include "Xml2Chart.h"

class CPatientDataPrintPreviewUI : public CControlUI {
public:
	CPatientDataPrintPreviewUI();
	~CPatientDataPrintPreviewUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const;
	void ZoomIn();
	void ZoomOut();

private:
	CXml2ChartFile   m_XmlChartFile;
	int              m_nOriginalHeight;
	float            m_fConstRatio;
	float            m_fZoom;

public:
	PatientInfo                  m_patient_info;
	std::vector<PatientEvent * > m_vEvents;
	PatientData                  m_patient_data[7];
	time_t                       m_tFirstDay;
};