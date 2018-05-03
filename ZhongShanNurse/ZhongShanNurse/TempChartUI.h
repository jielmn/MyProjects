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

	DuiLib::CDuiString        m_sPatientName;                // 病人名字
	time_t                    m_tInDate;                     // 住院日期
	DuiLib::CDuiString        m_sOffice;                     // 科室
	DuiLib::CDuiString        m_sBedNo;                      // 床号
	DuiLib::CDuiString        m_sInNo;                       // 住院号

	time_t                    m_tFistDay;                     // 第一天日期
	int                       m_nWeekIndex;                   // 第几周

	TagData           m_TempData[7][6];               // 温度数据
};