#pragma once


#include "zsCommon.h"
#include "UIlib.h"
#include "Xml2Chart.h"

class CTempChartUI : public DuiLib::CControlUI
{
public:
	CTempChartUI();
	~CTempChartUI();

	virtual void DoEvent(DuiLib::TEventUI& event);

	void Print(HDC hDC, RECT & r, int nChartUIHeight);

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

	void DrawTempPoint(CPoint point, HDC hDc, int RADIUS = 6);

	void SetTempData(TagData arrTempData[][6]);

	CXml2ChartFile *  m_XmlChartFile;

	DuiLib::CDuiString        m_sPatientName;                // ��������
	time_t                    m_tInDate;                     // סԺ����
	DuiLib::CDuiString        m_sOffice;                     // ����
	DuiLib::CDuiString        m_sBedNo;                      // ����
	DuiLib::CDuiString        m_sInNo;                       // סԺ��

	time_t                    m_tFistDay;                     // ��һ������
	int                       m_nWeekIndex;                   // �ڼ���

	TagData           m_TempData[7][6];               // �¶�����
};