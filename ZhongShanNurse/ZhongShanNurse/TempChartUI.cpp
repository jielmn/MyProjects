#include "TempChartUI.h"

#define  CHART_UI_HEIGHT                   1000

CTempChartUI::CTempChartUI()
{
	m_XmlChartFile = new CXml2ChartFile;
	m_XmlChartFile->ReadXmlChartFile( DuiLib::CPaintManagerUI::GetInstancePath() + "res\\体温表设计.xml");

	m_tInDate = time(0);
	m_tFistDay = time(0);                        // 第一天日期
	m_nWeekIndex = 0;                            // 第几周
	memset(m_TempData, 0, sizeof(m_TempData));   // 温度数据
}

CTempChartUI::~CTempChartUI() {
	if (m_XmlChartFile) {
		delete m_XmlChartFile;
		m_XmlChartFile = 0;
	}
}

void CTempChartUI::DoEvent(DuiLib::TEventUI& event) {
	CControlUI::DoEvent(event);
}

void CTempChartUI::Print(HDC hDC, RECT & r, int nChartUIHeight) {
	char buf[8192];
	DuiLib::CDuiString tmp;
	//RECT r = this->GetPos();

	if (m_XmlChartFile->m_ChartUI) {
		int a = ((r.right - r.left - 1) - 120) / 42;
		int b = ((nChartUIHeight - 1) - 85 - 125 - 200) / 40;

		int right = a * 42 + 120 + r.left + 1;
		int h = b * 40 + 85 + 125 + 200 + 1;

		m_XmlChartFile->m_ChartUI->SetRect(r.left, r.top, right, r.top + h);
		m_XmlChartFile->m_ChartUI->RecacluteLayout();

		char szHospitalName[256];
		g_cfg->GetConfig("hospital name", buf, sizeof(buf), "省三医院");
		strncpy_s(szHospitalName, buf, sizeof(szHospitalName));

		CXml2ChartUI * pUI = m_XmlChartFile->FindChartUIByName("hospital_name");
		if (pUI) {
			pUI->SetText(szHospitalName);
		}

		pUI = m_XmlChartFile->FindChartUIByName("patient_name");
		if (pUI) {
			tmp = "姓名：";
			tmp += m_sPatientName;
			pUI->SetText((const char *)tmp);
		}

		CXml2ChartUI * pIndate = m_XmlChartFile->FindChartUIByName("indate");
		if (pIndate) {
			tmp = "入院日期：";
			//tmp += DateTime2Str(buf, sizeof(buf), &m_tInDate);
			pIndate->SetText((const char *)tmp);
		}

		pUI = m_XmlChartFile->FindChartUIByName("office");
		if (pUI) {
			tmp = "科室：";
			tmp += m_sOffice;
			pUI->SetText((const char *)tmp);
		}

		pUI = m_XmlChartFile->FindChartUIByName("bed_no");
		if (pUI) {
			tmp = "床号：";
			tmp += m_sBedNo;
			pUI->SetText((const char *)tmp);
		}

		pUI = m_XmlChartFile->FindChartUIByName("inno");
		if (pUI) {
			tmp = "住院号：";
			tmp += m_sInNo;
			pUI->SetText((const char *)tmp);
		}

		for (int i = 0; i < 7; i++) {
			DuiLib::CDuiString name;
			name.Format("no%d", i + 1);
			pUI = m_XmlChartFile->FindChartUIByName((const char *)name);
			if (pUI) {
				DuiLib::CDuiString tmpStr;
				tmpStr.Format("%d", m_nWeekIndex * 7 + i + 1);
				pUI->SetText((const char *)tmpStr);
			}
		}

		for (int i = 0; i < 7; i++) {
			DuiLib::CDuiString name;
			name.Format("day%d", i + 1);
			pUI = m_XmlChartFile->FindChartUIByName((const char *)name);
			if (pUI) {
				time_t t = m_tFistDay + 3600 * 24 * i;
				Date2Str(buf, sizeof(buf), &t);
				pUI->SetText(buf);
			}
		}


		DrawXml2ChartUI(hDC, m_XmlChartFile->m_ChartUI);

		// 画温度点 
		CXml2ChartUI * pMainBlock = m_XmlChartFile->FindChartUIByName("MainBlock");
		if (pMainBlock != 0) {
			RECT rectBlock = pMainBlock->GetAbsoluteRect();
			int w = rectBlock.right - rectBlock.left;
			int h = rectBlock.bottom - rectBlock.top;

			int grid_h = h / 40;
			int grid_w = w / 42;

			//给出合理的画点的园半径
			int m = grid_h < grid_w ? grid_h : grid_w;
			int RADIUS = 6;
			if (m >= 14) {
				RADIUS = 6;
			}
			else {
				RADIUS = 4;
			}

			BOOL bFistPoint = TRUE;
			for (int i = 0; i < 7; i++) {
				for (int j = 0; j < 6; j++) {
					TagData tTemp = m_TempData[i][j];

					// 不在35-42度范围内
					if (!(tTemp.dwTemperature >= 3400 && tTemp.dwTemperature <= 4200)) {
						continue;
					}

					// 高度比例
					double h_ratio = (tTemp.dwTemperature - 3400.0) / (4200.0 - 3400.0);
					int    nTempHeight = (int)(h_ratio * h);

					CPoint pt;
					pt.x = grid_w * (i * 6 + j) + grid_w / 2 + rectBlock.left;
					pt.y = rectBlock.bottom - nTempHeight;

					DrawTempPoint(pt, hDC, RADIUS);

					// 是否第一个点
					if (bFistPoint) {
						::MoveToEx(hDC, pt.x, pt.y, 0);
						bFistPoint = FALSE;
					}
					else {
						::LineTo(hDC, pt.x, pt.y);
					}
				}
			}
		}
	}
}

bool CTempChartUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	RECT r = this->GetPos();
	Print(hDC, r, CHART_UI_HEIGHT);
	//return CControlUI::DoPaint(hDC, rcPaint, pStopControl);
	return true;
}

void CTempChartUI::DrawTempPoint(CPoint point, HDC hDc, int RADIUS /*= 6*/)
{
	::Ellipse(hDc, point.x - RADIUS, point.y - RADIUS, point.x + RADIUS, point.y + RADIUS);

	int nTmp = (int)(0.707 * RADIUS);

	POINT points[2] = { { point.x - nTmp, point.y - nTmp },{ point.x + nTmp, point.y + nTmp } };
	::Polyline(hDc, points, 2);

	POINT points1[2] = { { point.x + nTmp, point.y - nTmp },{ point.x - nTmp, point.y + nTmp } };
	::Polyline(hDc, points1, 2);
}

void CTempChartUI::SetTempData(TagData arrTempData[][6]) {
	memcpy(m_TempData, arrTempData, sizeof(m_TempData));
}