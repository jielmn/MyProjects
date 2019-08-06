#pragma once
#include "common.h"
#include "UIlib.h"
using namespace DuiLib;

#include "Xml2Chart.h"

class CPatientDataPrintPreviewUI : public CControlUI {
public:
	CPatientDataPrintPreviewUI();

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(DuiLib::TEventUI& event);
	virtual LPCTSTR GetClass() const;

private:
	CXml2ChartFile   m_XmlChartFile;

public:
	PatientInfo                 m_patient_info;
	PatientData                 m_patient_data[7];
	time_t                      m_tFirstDay;
};