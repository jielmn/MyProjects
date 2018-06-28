#include <time.h>
#include "MyImage.h"
#include "Business.h"
#include "exhDatabase.h"

#include<gdiplus.h>//gdi+ͷ�ļ�
using namespace Gdiplus;

CMyImageUI::CMyImageUI(DuiLib::CPaintManagerUI *pManager) : m_pen_3( Gdiplus::Color(0,255,0),1.0 ), m_brush_3(Gdiplus::Color(0, 255, 0) ) {
	m_pManager = pManager;

	char buf[8192];
	DWORD   dwValue;
	DWORD   dwValue_1;

	g_cfg->GetConfig( "color thread 1", buf, sizeof(buf), COLOR_THREAD_1);
	sscanf(buf, "0x%x", &dwValue);
	dwValue = RGB_REVERSE(dwValue);
	m_hPen  = ::CreatePen(0, 1, dwValue); 

	g_cfg->GetConfig("low temperature color", buf, sizeof(buf), COLOR_LOW_TEMPERATURE);
	sscanf(buf, "0x%x", &dwValue);	
	dwValue = RGB_REVERSE(dwValue);
	m_hPen1 = ::CreatePen(PS_DASH, 1, dwValue);

	g_cfg->GetConfig("high temperature color", buf, sizeof(buf), COLOR_HIGH_TEMPERATURE);
	sscanf(buf, "0x%x", &dwValue);
	dwValue = RGB_REVERSE(dwValue);
	m_hPen2 = ::CreatePen(PS_DASH, 1, dwValue);

	g_cfg->GetConfig("color thread 2", buf, sizeof(buf), COLOR_THREAD_2);
	sscanf(buf, "0x%x", &dwValue);
	dwValue_1 = RGB_REVERSE(dwValue);
	m_hPen3 = ::CreatePen( PS_SOLID, 2, dwValue_1);

	m_pen_3.SetWidth( 3.0 ); 
	m_pen_3.SetColor( Gdiplus::Color( (BYTE)((dwValue & 0xFF0000) >> 16) , (BYTE)((dwValue & 0xFF00) >> 8) , (BYTE)(dwValue & 0xFF) ) );
	m_brush_3.SetColor(Gdiplus::Color((BYTE)((dwValue & 0xFF0000) >> 16), (BYTE)((dwValue & 0xFF00) >> 8), (BYTE)(dwValue & 0xFF)));

	g_cfg->GetConfig("text color 1", buf, sizeof(buf), COLOR_TEXT_1);
	sscanf(buf, "0x%x", &dwValue);
	m_dwTextColor = RGB_REVERSE(dwValue);

	

	g_cfg->GetConfig("LEFT", dwValue, 50);
	m_nLeft = (int)dwValue;

	g_cfg->GetConfig("GRID SIZE", dwValue, 50);
	m_nGridSize = dwValue;

	g_cfg->GetConfig("text offset x", dwValue, -30); 
	m_nTextOffsetX = dwValue;

	g_cfg->GetConfig("text offset y", dwValue, -8);
	m_nTextOffsetY = dwValue;

	g_cfg->GetConfig("time offset x", dwValue, 1);
	m_nTimeOffsetX = dwValue;

	g_cfg->GetConfig("time offset y", dwValue, 1);
	m_nTimeOffsetY = dwValue;

	g_cfg->GetConfig("time unit length", dwValue, 12);
	m_nTimeUnitLen = dwValue;

	g_cfg->GetConfig("max poings count", dwValue, 10);
	if (0 == dwValue) {
		dwValue = 10;
	}
	m_nMaxPointsCnt = dwValue;

	g_cfg->GetConfig("RADIUS", dwValue, 4);  
	m_nRadius = dwValue;
	if ( m_nRadius < 2 || m_nRadius > 6 ) {
		m_nRadius = 4;
	}
}

CMyImageUI::~CMyImageUI() {
	ClearVector(m_vTempData);
}

bool CMyImageUI::DoPaint(HDC hDC, const RECT& rcPaint, DuiLib::CControlUI* pStopControl) {
	DuiLib::CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);	

	RECT rect   = this->GetPos();
	int  width  = rect.right - rect.left;
	int  height = rect.bottom - rect.top;

	int RIGHT = width - m_nLeft;

	int nMinTemp = g_dwMinTemp;
	int m_nGridCount = 42 - nMinTemp;

	m_nGridSize   = height / m_nGridCount;
	int nReminder = height % m_nGridCount;
	int nMulti    = (40 - nReminder - 1) / m_nGridCount + 1;
	m_nGridSize  -= nMulti;

	// �м��¶ȵ�λ��
	int middle = height / 2;

	::SelectObject(hDC, m_hPen);
	::SetTextColor(hDC, m_dwTextColor);

	// ������
	::MoveToEx(hDC, m_nLeft + rect.left, rect.top, 0);
	::LineTo(hDC, m_nLeft + rect.left, rect.bottom);

	// 42���ߵ�topλ��
	int nFirstTop = middle - m_nGridSize * ( m_nGridCount / 2 );
	int nFistTemperature = 42;
	DuiLib::CDuiString strText;

	for (int i = 0; i < m_nGridCount+1; i++) {
		int  nTop = nFirstTop + i * m_nGridSize;
		int  nTemperature = nFistTemperature - i;

		// ��38~42�ȵĺ���
		::MoveToEx(hDC, m_nLeft + rect.left, nTop + rect.top, 0);
		::LineTo(hDC, rect.right, nTop + rect.top);

		// ���̶�ֵ
		strText.Format("%d��", nTemperature);
		::TextOut(hDC, m_nLeft + rect.left + m_nTextOffsetX, nTop + rect.top + m_nTextOffsetY, strText, strText.GetLength());
	}

	time_t  tFistTime = 0;
	int     nLastTimeTextLeft = 0;
	if (m_vTempData.size() > 0) {
		TempData * pFist = m_vTempData[0];

		struct tm tTmTime;
		localtime_s(&tTmTime, &pFist->tTime);

		// ��һ�����ʱ��
		strText.Format("%02d:%02d:%02d", tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);
		::TextOut(hDC, m_nLeft + rect.left + m_nTimeOffsetX, middle + (m_nGridCount / 2) * m_nGridSize + rect.top + m_nTimeOffsetY, strText, strText.GetLength());

		tFistTime = pFist->tTime;
		nLastTimeTextLeft = 0;
	}


	::SelectObject(hDC, m_hPen3);

	assert(g_dwCollectInterval > 0);
	int nMiddleTemp = (42 + nMinTemp) / 2;

	vector<TempData *>::iterator it;
	for (it = m_vTempData.begin(); it != m_vTempData.end(); it++) {
		TempData * pItem = *it;

		int nDiff = (int)(pItem->tTime - tFistTime);
		int nX = (int)( ( (double)nDiff / g_dwCollectInterval ) * m_nTimeUnitLen ) ;
		int nY = (int)( (nMiddleTemp * 100.0 - (double)pItem->dwTemperature) / 100.0 * m_nGridSize );

		DrawTempPoint(graphics, nX + m_nLeft + rect.left, nY + middle + rect.top, hDC, m_nRadius);

		// �������㹻�󣬻���time
		if (nX - nLastTimeTextLeft >= 120) {
			struct tm tTmTime;
			localtime_s(&tTmTime, &pItem->tTime);
			strText.Format("%02d:%02d:%02d", tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);
			::TextOut(hDC, m_nLeft + rect.left + nX - 30, middle + (m_nGridCount / 2) * m_nGridSize + rect.top + m_nTimeOffsetY, strText, strText.GetLength());
			nLastTimeTextLeft = nX;
		}

		if (it == m_vTempData.begin()) {
			::MoveToEx(hDC, nX + m_nLeft + rect.left, nY + middle + rect.top, 0);
		}
		else {
#if 0
			::LineTo(hDC, nX + m_nLeft + rect.left, nY + middle + rect.top);
#else
			POINT pt;
			::GetCurrentPositionEx(hDC, &pt);
			graphics.DrawLine( &m_pen_3, pt.x, pt.y, nX + m_nLeft + rect.left, nY + middle + rect.top );
			::MoveToEx(hDC, nX + m_nLeft + rect.left, nY + middle + rect.top, 0);
#endif
		}
	}

	// ����������
	::SelectObject(hDC, m_hPen1);
	int nY = (int)((nMiddleTemp * 100.0 - (double)g_dwLowTempAlarm) / 100.0 * m_nGridSize);
	::MoveToEx(hDC, m_nLeft + rect.left, middle + nY + rect.top, 0);
	::LineTo(hDC, rect.right, middle + nY + rect.top);
	strText = "���±���";
	::TextOut(hDC, m_nLeft + rect.left + 5, middle + nY + rect.top + 5, strText, strText.GetLength());

	::SelectObject(hDC, m_hPen2);
	nY = (int)((nMiddleTemp * 100.0 - (double)g_dwHighTempAlarm) / 100.0 * m_nGridSize);
	::MoveToEx(hDC, m_nLeft + rect.left, middle + nY + rect.top, 0);
	::LineTo(hDC, rect.right, middle + nY + rect.top);
	strText = "���±���";
	::TextOut(hDC, m_nLeft + rect.left + 5, middle + nY + rect.top - 20, strText, strText.GetLength());

	return true;

}

void CMyImageUI::DoEvent(DuiLib::TEventUI& event) {


	DuiLib::CControlUI::DoEvent(event);
}

void  CMyImageUI::AddTemp(DWORD dwTemp) {
	time_t now = time(0);

	RECT r = this->GetPos();
	int nWidth = (r.right - r.left) - m_nLeft;

	// �������
	if ( m_vTempData.size() > 0 ) {
		TempData * pFirst = m_vTempData[0];
		if ( (int)(((double)(now - pFirst->tTime) / g_dwCollectInterval)  * m_nTimeUnitLen) > nWidth ) {
			ClearVector(m_vTempData);
		}		
	}

	TempData * pTemp = new TempData;
	pTemp->dwTemperature = dwTemp;
	pTemp->tTime = now;
	g_log->Output(ILog::LOG_SEVERITY_INFO, "temp = %ld, time=%lu\n", dwTemp, now);
	m_vTempData.push_back(pTemp);
	CMyDatabase::GetInstance()->AddTemp(dwTemp, now);

	Invalidate();

	if (dwTemp < g_dwLowTempAlarm || dwTemp > g_dwHighTempAlarm) {
		CBusiness::GetInstance()->AlarmAsyn(g_szAlarmFilePath);
	}
}

void   CMyImageUI::DrawTempPoint(Graphics & g, int x, int y, HDC hDc, int RADIUS /*= 6*/) {
	// ::Ellipse(hDc, x - RADIUS, y - RADIUS, x + RADIUS, y + RADIUS);
	//g.DrawEllipse(&m_pen_3, x - RADIUS, y - RADIUS, 2 * RADIUS, 2 * RADIUS);
	g.FillEllipse(&m_brush_3, x - RADIUS, y - RADIUS, 2 * RADIUS, 2 * RADIUS);

	//int nTmp = (int)(0.707 * RADIUS);

	//POINT points[2] = { { x - nTmp, y - nTmp },{ x + nTmp, y + nTmp } };
	//::Polyline(hDc, points, 2);

	//POINT points1[2] = { { x + nTmp, y - nTmp },{ x - nTmp, y + nTmp } };
	//::Polyline(hDc, points1, 2);
}

void  CMyImageUI::SetWndRect(int x, int y) {
	//int h = y - 340;
	//if (h > 0) {
	//	m_nGridSize = h / (42 - g_dwMinTemp);
	//	Invalidate();
	//}
}






CAlarmImageUI::CAlarmImageUI(DuiLib::CPaintManagerUI *pManager) {
	m_pManager = pManager;
	m_bSetBkImage = FALSE;
}

CAlarmImageUI::~CAlarmImageUI() {

}

bool CAlarmImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	return CControlUI::DoPaint(hDC, rcPaint, pStopControl);
}

void CAlarmImageUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == DuiLib::UIEVENT_TIMER && event.wParam == MYIMAGE_TIMER_ID) {
		if (m_bSetBkImage) {
			this->SetBkImage(m_strBkImage);
		}
		else {
			this->SetBkImage("");
		}
		m_bSetBkImage = !m_bSetBkImage;
	}

	CControlUI::DoEvent(event);
}

void   CAlarmImageUI::HighTempAlarm() {
	m_strBkImage = "alarm_high_temp.png";
	m_bSetBkImage = TRUE;
	m_pManager->SetTimer(this, MYIMAGE_TIMER_ID, 500);
}

void   CAlarmImageUI::LowTempAlarm() {
	m_strBkImage = "alarm_low_temp_1.png";   
	m_bSetBkImage = TRUE;
	m_pManager->SetTimer(this, MYIMAGE_TIMER_ID, 500);
}

void  CAlarmImageUI::FailureAlarm() {
	m_strBkImage = "alarm_fail.png";
	m_bSetBkImage = TRUE;
	m_pManager->SetTimer(this, MYIMAGE_TIMER_ID, 500);
}

void   CAlarmImageUI::StopAlarm() {
	this->SetBkImage("");
	m_pManager->KillTimer(this, MYIMAGE_TIMER_ID);
}