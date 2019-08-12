
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// Test.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

#include "UIlib.h"
using namespace DuiLib;

#include "LmnCommon.h"
#include "LmnString.h"

const char * GetSexStr(int nSex) {
	if (nSex == 1)
		return "��";
	else if (nSex == 2)
		return "Ů";
	else
		return "";
}

char * PreviewNum(char * buf, DWORD dwSize, int nNum) {
	assert(dwSize > 0);
	assert(buf);

	if (0 >= nNum)
		buf[0] = '\0';
	else
		SNPRINTF(buf, dwSize, "%d", nNum);

	return buf;
}

void GetDateStr(char * year, DWORD d1, char * month, DWORD d2, char * day, DWORD d3, time_t t) {
	struct tm  tmp;
	localtime_s(&tmp, &t);

	SNPRINTF(year, d1, "%04d", tmp.tm_year + 1900);
	SNPRINTF(month, d2, "%02d", tmp.tm_mon + 1);
	SNPRINTF(day, d3, "%02d", tmp.tm_mday);
}

int GetPatientDataStartIndex(PatientData * pData, DWORD dwSize) {
	assert(dwSize >= 7);
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 6; j++) {
			if (pData[i].m_pulse[j] > 0) {
				return i;
			}
			if (pData[i].m_breath[j] > 0) {
				return i;
			}
			if (pData[i].m_temp[j] > 0) {
				return i;
			}
		}

		if (pData[i].m_defecate > 0)
			return i;

		if (pData[i].m_urine > 0)
			return i;

		if (pData[i].m_income > 0)
			return i;

		if (pData[i].m_output > 0)
			return i;

		if (pData[i].m_szBloodPressure[0] != '\0')
			return i;

		if (pData[i].m_szWeight[0] != '\0')
			return i;

		if (pData[i].m_szIrritability[0] != '\0')
			return i;
	}

	return 6;
}

char * Date2String_md(char * szDest, DWORD dwDestSize, const time_t * t) {
	struct tm  tmp;
	localtime_s(&tmp, t);

	_snprintf_s(szDest, dwDestSize, dwDestSize, "%02d-%02d", tmp.tm_mon + 1, tmp.tm_mday);
	return szDest;
}

time_t  GetAnyDayZeroTime(time_t t) {
	struct tm tTmTime;
	localtime_s(&tTmTime, &t);

	tTmTime.tm_hour = 0;
	tTmTime.tm_min = 0;
	tTmTime.tm_sec = 0;

	return mktime(&tTmTime);
}

void PrepareXmlChart(CXml2ChartFile & xmlChart, PatientInfo * pInfo,
	PatientData * pData, DWORD dwSize, time_t tFirstDay) {
	char buf[256];
	char year[16];
	char month[16];
	char day[16];
	CXml2ChartUI * pItem = 0;
	CDuiString strText;

	// �����
	pItem = xmlChart.FindChartUIByName("outpatient_no");
	if (pItem) {
		pItem->SetText(pInfo->m_szOutpatientNo);
	}

	// סԺ��
	pItem = xmlChart.FindChartUIByName("hospical_admission_no");
	if (pItem) {
		pItem->SetText(pInfo->m_szHospitalAdmissionNo);
	}

	// ����
	pItem = xmlChart.FindChartUIByName("patient_name");
	if (pItem) {
		pItem->SetText(pInfo->m_szPName);
	}

	// �Ա�
	pItem = xmlChart.FindChartUIByName("sex");
	if (pItem) {
		pItem->SetText(GetSexStr(pInfo->m_sex));
	}

	// ����
	pItem = xmlChart.FindChartUIByName("age");
	if (pItem) {
		pItem->SetText(PreviewNum(buf, sizeof(buf), pInfo->m_age));
	}

	// ��Ժ����
	if (pInfo->m_in_hospital > 0) {
		GetDateStr(year, sizeof(year), month, sizeof(month), day, sizeof(day),
			pInfo->m_in_hospital);
		// ��
		pItem = xmlChart.FindChartUIByName("in_date_y");
		if (pItem) {
			pItem->SetText(year);
		}
		// ��
		pItem = xmlChart.FindChartUIByName("in_date_m");
		if (pItem) {
			pItem->SetText(month);
		}
		// ��
		pItem = xmlChart.FindChartUIByName("in_date_d");
		if (pItem) {
			pItem->SetText(day);
		}
	}
	else {
		// ��
		pItem = xmlChart.FindChartUIByName("in_date_y");
		if (pItem) {
			pItem->SetText("");
		}
		// ��
		pItem = xmlChart.FindChartUIByName("in_date_m");
		if (pItem) {
			pItem->SetText("");
		}
		// ��
		pItem = xmlChart.FindChartUIByName("in_date_d");
		if (pItem) {
			pItem->SetText("");
		}
	}

	// �Ʊ�
	pItem = xmlChart.FindChartUIByName("medical_department");
	if (pItem) {
		pItem->SetText(pInfo->m_szMedicalDepartment);
	}

	// ת�Ʊ�
	pItem = xmlChart.FindChartUIByName("a_1");
	if (pItem) {
		pItem->SetVisible(TRUE);

		pItem = xmlChart.FindChartUIByName("b_1");
		if (pItem)
			pItem->SetFontIndex(11);
	}

	// ����
	pItem = xmlChart.FindChartUIByName("ward");
	if (pItem) {
		pItem->SetText(pInfo->m_szWard);
	}

	// ����
	pItem = xmlChart.FindChartUIByName("bed_no");
	if (pItem) {
		pItem->SetText(pInfo->m_szBedNo);
	}

	// ת����
	pItem = xmlChart.FindChartUIByName("a_2");
	if (pItem) {
		pItem->SetVisible(TRUE);

		pItem = xmlChart.FindChartUIByName("b_2");  
		if (pItem)
			pItem->SetFontIndex(11);  
	}

	// �����ݲ�Ҫ��ӡ
	int nStartIndex = GetPatientDataStartIndex(pData, dwSize);
	tFirstDay = tFirstDay + 3600 * 24 * nStartIndex;

	// ���� 
	for (int i = 0; i < 7; i++) {
		strText.Format("date_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			time_t t = tFirstDay + 3600 * 24 * i;
			Date2String_md(buf, sizeof(buf), &t);
			pItem->SetText(buf);
		}
	}

	// סԺ����
	if (pInfo->m_in_hospital > 0) {
		time_t t1 = GetAnyDayZeroTime(pInfo->m_in_hospital);
		for (int i = 0; i < 7 - nStartIndex; i++) {
			time_t t2 = GetAnyDayZeroTime(tFirstDay + 3600 * 24 * i);
			if (t2 >= t1) {
				int nInDays = (int)(t2 - t1) / (3600 * 24);
				strText.Format("in_date_%d", i + 1);
				pItem = xmlChart.FindChartUIByName(strText);
				if (pItem) {
					strText.Format("%d", nInDays);
					pItem->SetText((const char *)strText);
				}
			}
		}
	}

	// ����������
	if (pInfo->m_surgery > 0) {
		time_t t1 = GetAnyDayZeroTime(pInfo->m_surgery);
		for (int i = 0; i < 7 - nStartIndex; i++) {
			time_t t2 = GetAnyDayZeroTime(tFirstDay + 3600 * 24 * i);
			if (t2 >= t1) {
				int nInDays = (int)(t2 - t1) / (3600 * 24);
				strText.Format("sur_date_%d", i + 1);
				pItem = xmlChart.FindChartUIByName(strText);
				if (pItem) {
					strText.Format("%d", nInDays);
					pItem->SetText((const char *)strText);
				}
			}
		}
	}

	// ����
	for (int i = 0; i < 7 - nStartIndex; i++) {
		for (int j = 0; j < 6; j++) {
			strText.Format("breath_%d_%d", i + 1, j + 1);
			pItem = xmlChart.FindChartUIByName(strText);
			if (pItem) {
				pItem->SetText(PreviewNum(buf, sizeof(buf), pData[i + nStartIndex].m_breath[j]));
				pItem->SetVAlignType(VALIGN_TOP);   
			}
		}
	}

	// ������
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("defecate_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(PreviewNum(buf, sizeof(buf), pData[i + nStartIndex].m_defecate));
		}
	}

	// ����
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("urine_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(PreviewNum(buf, sizeof(buf), pData[i + nStartIndex].m_urine));
		}
	}

	// ����
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("income_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(PreviewNum(buf, sizeof(buf), pData[i + nStartIndex].m_income));
		}
	}

	// ����
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("output_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(PreviewNum(buf, sizeof(buf), pData[i + nStartIndex].m_output));
		}
	}

	// Ѫѹ
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("blood_pressure_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i + nStartIndex].m_szBloodPressure);
		}
	}

	// ����
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("weight_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i + nStartIndex].m_szWeight);
		}
	}

	// ����
	for (int i = 0; i < 7 - nStartIndex; i++) {
		strText.Format("irritability_%d", i + 1);
		pItem = xmlChart.FindChartUIByName(strText);
		if (pItem) {
			pItem->SetText(pData[i + nStartIndex].m_szIrritability);
		}
	}
}

static int  GetPatientDataImagePoints(POINT * points, DWORD  dwSize, int w, int h,
	int nUnitsX, int nUnitsY, int nMaxY,
	int nLeft, int nTop, int nOffsetX, int nOffsetY,
	PatientData * pData, DWORD dwDataSize, int nType = 0) {
	assert(dwDataSize >= 7);
	assert(dwSize >= 6 * 7);

	float x = (float)w / (float)nUnitsX;
	float y = (float)h / (float)nUnitsY;
	int cnt = 0;
	int nStartIndex = GetPatientDataStartIndex(pData, dwDataSize);

	for (int i = 0; i < 7 - nStartIndex; i++) {
		// �¶�
		if (nType == 0) {
			for (int j = 0; j < 6; j++) {
				if (pData[i + nStartIndex].m_temp[j] >= 3400 && pData[i + nStartIndex].m_temp[j] <= 4200) {
					points[cnt].x = (int)((i * 6 + j + 0.5f) * x) + nLeft + nOffsetX;
					points[cnt].y = (int)((nMaxY - pData[i + nStartIndex].m_temp[j]) * y) + nTop + nOffsetY;
					cnt++;
				}
			}
		}
		// ����
		else {
			for (int j = 0; j < 6; j++) {
				if (pData[i + nStartIndex].m_pulse[j] >= 20 && pData[i + nStartIndex].m_pulse[j] <= 192) {
					points[cnt].x = (int)((i * 6 + j + 0.5f) * x) + nLeft + nOffsetX;
					points[cnt].y = (int)((nMaxY - pData[i + nStartIndex].m_pulse[j]) * y) + nTop + nOffsetY;
					cnt++;
				}
			}
		}
	}

	return cnt;
}


void PrintXmlChart(HDC hDC, CXml2ChartFile & xmlChart, int nOffsetX, int nOffsetY,
	PatientData * pData, DWORD dwDataSize) {
	SetBkMode(hDC, TRANSPARENT);
	DrawXml2ChartUI(hDC, xmlChart.m_ChartUI, nOffsetX, nOffsetY);

	CXml2ChartUI * pMain = xmlChart.FindChartUIByName("main");
	if (0 == pMain)
		return;

	HPEN   hOld_pen = 0;
	HBRUSH hOld_brush = 0;
	HPEN pen = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
	HPEN pen_1 = ::CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
	HPEN pen_2 = ::CreatePen(PS_SOLID, 2, RGB(0, 0, 0xFF));
	HBRUSH hBrush = ::CreateSolidBrush(RGB(0xFF, 0, 0));

	int radius = 3;

	RECT r = pMain->GetAbsoluteRect();
	int w = r.right - r.left;
	int h = r.bottom - r.top;

	POINT points[6 * 7];
	int cnt = 0;

	// ������������
	cnt = GetPatientDataImagePoints(points, 6 * 7, w, h, 42, 860, 4260, r.left, r.top,
		nOffsetX, nOffsetY, pData, dwDataSize);
	hOld_pen = (HPEN)::SelectObject(hDC, pen);
	::Polyline(hDC, points, cnt);

	// �����µ�
	::SelectObject(hDC, pen_2);
	for (int i = 0; i < cnt; i++) {
		POINT temp_points[2];

		temp_points[0].x = points[i].x - radius;
		temp_points[0].y = points[i].y - radius;
		temp_points[1].x = points[i].x + radius;
		temp_points[1].y = points[i].y + radius;
		::Polyline(hDC, temp_points, 2);

		temp_points[0].x = points[i].x - radius;
		temp_points[0].y = points[i].y + radius;
		temp_points[1].x = points[i].x + radius;
		temp_points[1].y = points[i].y - radius;
		::Polyline(hDC, temp_points, 2);
	}


	// ������������
	cnt = GetPatientDataImagePoints(points, 6 * 7, w, h, 42, 172, 192, r.left, r.top,
		nOffsetX, nOffsetY, pData, dwDataSize, 1);
	::SelectObject(hDC, pen_1);
	::Polyline(hDC, points, cnt);

	// ��������
	hOld_brush = (HBRUSH)::SelectObject(hDC, hBrush);
	for (int i = 0; i < cnt; i++) {
		::Ellipse(hDC, points[i].x - radius, points[i].y - radius,
			points[i].x + radius, points[i].y + radius);
	}

	::SelectObject(hDC, hOld_pen);
	::SelectObject(hDC, hOld_brush);

	DeleteObject(pen);
	DeleteObject(pen_1);
	DeleteObject(pen_2);
	DeleteObject(hBrush);
}