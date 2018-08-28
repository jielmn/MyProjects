#include <time.h>
#include "MyImage.h"
#include "business.h"
#include "LmnTelSvr.h"
#include "LmnExcel.h"

CMyImageUI::CMyImageUI() : m_remark_pen(Gdiplus::Color(0x803D5E49), 3.0),
						   m_remark_brush(Gdiplus::Color(0x803D5E49))
{
	m_dwSelectedReaderIndex = 0;
	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		m_temperature_pen[i]   = new Pen(Gdiplus::Color(g_data.m_argb[i]), 3.0);
		m_temperature_brush[i] = new SolidBrush(Gdiplus::Color(g_data.m_argb[i]));
	}

	m_hCommonThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x66, 0x66, 0x66));
	m_hBrighterThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x99, 0x99, 0x99));
	m_hCommonBrush = ::CreateSolidBrush(RGB(0x19, 0x24, 0x31));
	m_hLowTempAlarmPen = ::CreatePen(PS_DASH, 1, RGB(0x02, 0xA5, 0xF1));
	m_hHighTempAlarmPen = ::CreatePen(PS_DASH, 1, RGB(0xFC, 0x23, 0x5C));

	m_dwNextTempIndex = 0;
	m_bSetParentScrollPos = FALSE;
	m_dwCurTempIndex = -1;



//#if TEST_FLAG
#if 0
	TempData * pData = 0;
	int nIndex = 0;

	/* 1 */
	nIndex = 0;
	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3357;
	pData->tTime = DateTime2String("2018-08-24 15:00:00");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3407;
	pData->tTime = DateTime2String("2018-08-24 15:01:00");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3435;
	pData->tTime = DateTime2String("2018-08-24 15:02:00");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3461;
	pData->tTime = DateTime2String("2018-08-24 15:03:00");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3478;
	pData->tTime = DateTime2String("2018-08-24 15:04:00");
	STRNCPY(pData->szRemark, "测试", sizeof(pData->szRemark));
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3497;
	pData->tTime = DateTime2String("2018-08-24 15:05:00");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3509;
	pData->tTime = DateTime2String("2018-08-24 15:06:00");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3521;
	pData->tTime = DateTime2String("2018-08-24 15:07:00");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3547;
	pData->tTime = DateTime2String("2018-08-24 15:08:00");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3559;
	pData->tTime = DateTime2String("2018-08-24 15:09:00");
	m_vTempData[nIndex].push_back(pData);


	/* 2 */
	nIndex = 1;
	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3516;
	pData->tTime = DateTime2String("2018-08-24 15:00:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3530;
	pData->tTime = DateTime2String("2018-08-24 15:01:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3540;
	pData->tTime = DateTime2String("2018-08-24 15:02:02");
	STRNCPY(pData->szRemark, "测试2.0", sizeof(pData->szRemark));
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3554;
	pData->tTime = DateTime2String("2018-08-24 15:03:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3561;
	pData->tTime = DateTime2String("2018-08-24 15:04:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3569;
	pData->tTime = DateTime2String("2018-08-24 15:05:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3578;
	pData->tTime = DateTime2String("2018-08-24 15:06:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3580;
	pData->tTime = DateTime2String("2018-08-24 15:07:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3590;
	pData->tTime = DateTime2String("2018-08-24 15:08:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3600;
	pData->tTime = DateTime2String("2018-08-24 15:09:02");
	m_vTempData[nIndex].push_back(pData);
	
	/* 3 */
	nIndex = 2;
	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3166;
	pData->tTime = DateTime2String("2018-08-24 15:00:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3195;
	pData->tTime = DateTime2String("2018-08-24 15:01:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3214;
	pData->tTime = DateTime2String("2018-08-24 15:02:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3228;
	pData->tTime = DateTime2String("2018-08-24 15:03:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3242;
	pData->tTime = DateTime2String("2018-08-24 15:04:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3259;
	pData->tTime = DateTime2String("2018-08-24 15:05:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3276;
	pData->tTime = DateTime2String("2018-08-24 15:06:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3290;
	pData->tTime = DateTime2String("2018-08-24 15:07:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3335;
	pData->tTime = DateTime2String("2018-08-24 15:08:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3366;
	pData->tTime = DateTime2String("2018-08-24 15:09:02");
	m_vTempData[nIndex].push_back(pData);

	/* 4 */
	nIndex = 3;
	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3347;
	pData->tTime = DateTime2String("2018-08-24 15:00:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3359;
	pData->tTime = DateTime2String("2018-08-24 15:01:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3421;
	pData->tTime = DateTime2String("2018-08-24 15:02:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3442;
	pData->tTime = DateTime2String("2018-08-24 15:03:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3461;
	pData->tTime = DateTime2String("2018-08-24 15:04:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3478;
	pData->tTime = DateTime2String("2018-08-24 15:05:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3492;
	pData->tTime = DateTime2String("2018-08-24 15:06:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3504;
	pData->tTime = DateTime2String("2018-08-24 15:07:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3533;
	pData->tTime = DateTime2String("2018-08-24 15:08:02");
	m_vTempData[nIndex].push_back(pData);

	pData = new TempData;
	memset(pData, 0, sizeof(TempData));
	pData->dwTemperature = 3545;
	pData->tTime = DateTime2String("2018-08-24 15:09:02");
	m_vTempData[nIndex].push_back(pData);

	this->SetMinWidth(CalcMinWidth());

#endif

}

CMyImageUI::~CMyImageUI() {
	DeleteObject(m_hCommonThreadPen);
	DeleteObject(m_hBrighterThreadPen);
	DeleteObject(m_hCommonBrush);
	DeleteObject(m_hLowTempAlarmPen);
	DeleteObject(m_hHighTempAlarmPen);
	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		delete m_temperature_pen[i];
		delete m_temperature_brush[i];
		ClearVector(m_vTempData[i]);
	}
}

bool CMyImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	DuiLib::CControlUI::DoPaint(hDC, rcPaint, pStopControl);
	DuiLib::CDuiString strText;

	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	// 父节点: vertical layout
	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	SIZE tParentScrollPos = pParent->GetScrollPos();
	SIZE tParentScrollRange = pParent->GetScrollRange();

	if (tParentScrollPos.cx != tParentScrollRange.cx) {
		if (m_bSetParentScrollPos)
		{
			CBusiness::GetInstance()->UpdateScrollAsyn();
			m_bSetParentScrollPos = FALSE;
		}
	}

	RECT rect   = this->GetPos();
	int  width  = pParent->GetWidth();
	int  height = rect.bottom - rect.top; 
	
	/* 开始作图 */
	int nMinTemp = g_data.m_dwMyImageMinTemp;
	int nGridCount = MAX_TEMPERATURE - nMinTemp;

	int nGridHeight = height / nGridCount;
	int nReminder = height % nGridCount;
	int nVMargin = MIN_MYIMAGE_VMARGIN;

	if (nVMargin * 2 > nReminder) {
		int nSpared = (nVMargin * 2 - nReminder - 1) / nGridCount + 1;
		nGridHeight -= nSpared;
	}

	// 中间温度的位置
	int middle = height / 2;

	::SetTextColor(hDC, RGB(255,255,255) );
	::SelectObject(hDC, m_hCommonThreadPen );

	RECT rectLeft;
	rectLeft.left   = rect.left + tParentScrollPos.cx;
	rectLeft.top    = rect.top;
	rectLeft.right  = rectLeft.left + g_data.m_dwMyImageLeftBlank;
	rectLeft.bottom = rect.bottom;

	int nFirstTop = middle - nGridHeight * (nGridCount / 2);
	int nFistTemperature = MAX_TEMPERATURE;

	// 画出刻度线(水平横线)
	int nVInterval = MIN_TEMP_V_INTERVAL;
	for (int i = 0; i < nGridCount + 1; i++) {
		if (nVInterval >= MIN_TEMP_V_INTERVAL) {
			::SelectObject(hDC, m_hBrighterThreadPen);
			nVInterval = nGridHeight;
		}
		else {
			::SelectObject(hDC, m_hCommonThreadPen);
			nVInterval += nGridHeight;
		}
		int  nTop = nFirstTop + i * nGridHeight;
		int  nTemperature = nFistTemperature - i;
		::MoveToEx(hDC, rectLeft.right, nTop + rect.top, 0);
		::LineTo(hDC, rect.right, nTop + rect.top);
	}

	::SelectObject(hDC, m_hCommonThreadPen );

	/* 画温度曲线 */
	time_t  tFirstTime = GetFirstTime();
	time_t  tLastTime = GetLastTime();
	int nMiddleTemp = (MAX_TEMPERATURE + nMinTemp) / 2;

	vector<TempData *>::iterator it;
	for ( DWORD i = 0; i < MAX_READERS_COUNT; i++ ) {
		vector<TempData *> & vTempData = m_vTempData[i];

		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;

			int nDiff = (int)(pItem->tTime - tFirstTime);
			int nX = (int)(((double)nDiff / g_data.m_dwCollectInterval) * g_data.m_dwCollectIntervalWidth);
			int nY = (int)((nMiddleTemp * 100.0 - (double)pItem->dwTemperature) / 100.0 * nGridHeight);

			DrawTempPoint(i, graphics, nX + g_data.m_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, hDC, 6);

			if (it == vTempData.begin()) {
				::MoveToEx(hDC, nX + g_data.m_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, 0);
			}
			else {
				POINT pt;
				::GetCurrentPositionEx(hDC, &pt);
				graphics.DrawLine(m_temperature_pen[i], pt.x, pt.y, nX + g_data.m_dwMyImageLeftBlank + rect.left, nY + middle + rect.top);
				::MoveToEx(hDC, nX + g_data.m_dwMyImageLeftBlank + rect.left, nY + middle + rect.top, 0);
			}
		}
	}

	// 从第一个10秒整数，画时间
	if ( tFirstTime >= 0 ) {
		int  remainder = tFirstTime % 10;
		time_t  tFirstTime_1 = tFirstTime;
		if ( remainder > 0 ) {
			tFirstTime_1 += 10 - remainder;
		}

		int nLastX = -1;
		time_t t = 0;
		for ( t = tFirstTime_1; t < tLastTime; t += g_data.m_dwCollectInterval ) {
			int nDiff = (int)(t - tFirstTime);
			int nX = (int)(((double)nDiff / g_data.m_dwCollectInterval) * g_data.m_dwCollectIntervalWidth);

			if (nLastX == -1 || nX - nLastX >= 300) {
				int nTextX = g_data.m_dwMyImageLeftBlank + rect.left + nX + 1;
				if (nTextX > rectLeft.left) {
					struct tm tTmTime;
					localtime_s(&tTmTime, &t);
					strText.Format("%02d:%02d:%02d", tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);

					::TextOut(hDC, nTextX,
						middle + (nGridCount / 2) * nGridHeight + rect.top + 5,
						strText, strText.GetLength());

					nLastX = nX;
				}
			}			
		}

		t = tLastTime;
		int nDiff = (int)(t - tFirstTime);
		int nX = (int)(((double)nDiff / g_data.m_dwCollectInterval) * g_data.m_dwCollectIntervalWidth);
		if (nLastX == -1 || nX - nLastX >= 80) {
			int nTextX = g_data.m_dwMyImageLeftBlank + rect.left + nX;
			if (nTextX > rectLeft.left) {
				struct tm tTmTime;
				localtime_s(&tTmTime, &t);
				strText.Format("%02d:%02d:%02d", tTmTime.tm_hour, tTmTime.tm_min, tTmTime.tm_sec);

				::TextOut(hDC, nTextX,
					middle + (nGridCount / 2) * nGridHeight + rect.top + 5,
					strText, strText.GetLength());

				nLastX = nX;
			}
		}
		
	}

	// 画边框
	::SelectObject(hDC, m_hCommonThreadPen);
	::FillRect(hDC, &rectLeft, m_hCommonBrush);
	::Rectangle(hDC, rectLeft.left, rectLeft.top, rectLeft.right, rectLeft.bottom);
	::MoveToEx(hDC, rectLeft.left + width - 1, rectLeft.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.bottom);
	::MoveToEx(hDC, rectLeft.left, rectLeft.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.top);
	::MoveToEx(hDC, rectLeft.left, rectLeft.bottom - 1, 0);
	::LineTo(hDC, rectLeft.left + width - 1, rectLeft.bottom - 1);
	
	// 画刻度值
	nVInterval = MIN_TEMP_V_INTERVAL;
	for (int i = 0; i < nGridCount + 1; i++) {
		if (nVInterval >= MIN_TEMP_V_INTERVAL) {
			int  nTop = nFirstTop + i * nGridHeight;
			int  nTemperature = nFistTemperature - i;
			strText.Format("%d℃", nTemperature);
			::TextOut(hDC, rectLeft.right + (-40),
				nTop + rect.top + (-8),
				strText, strText.GetLength());
			nVInterval = nGridHeight;
		}
		else {
			nVInterval += nGridHeight;
		}
	}

	// 画出报警线
	::SelectObject(hDC, m_hLowTempAlarmPen);
	int nY = (int)((nMiddleTemp * 100.0 - (double)g_data.m_dwLowTempAlarm[m_dwSelectedReaderIndex]) / 100.0 * nGridHeight);
	::MoveToEx(hDC, rectLeft.right, middle + nY + rect.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, middle + nY + rect.top);
	strText = "低温报警";
	::TextOut(hDC, rectLeft.right + 5,
		middle + nY + rect.top + 5,
		strText, strText.GetLength());


	::SelectObject(hDC, m_hHighTempAlarmPen);
	nY = (int)((nMiddleTemp * 100.0 - (double)g_data.m_dwHighTempAlarm[m_dwSelectedReaderIndex]) / 100.0 * nGridHeight);
	::MoveToEx(hDC, rectLeft.right, middle + nY + rect.top, 0);
	::LineTo(hDC, rectLeft.left + width - 1, middle + nY + rect.top);
	strText = "高温报警";
	::TextOut(hDC, rectLeft.right + 5,
		middle + nY + rect.top + (-20),
		strText, strText.GetLength());


	// 画出类似注释的框
	::SetBkMode(hDC, TRANSPARENT);

	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];

		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;

			if (pItem->szRemark[0] != '\0') {
				int nDiff = (int)(pItem->tTime - tFirstTime);
				int nX = (int)(((double)nDiff / g_data.m_dwCollectInterval ) * g_data.m_dwCollectIntervalWidth );
				int nY = (int)((nMiddleTemp * 100.0 - (double)pItem->dwTemperature) / 100.0 * nGridHeight);

				int nX1 = nX + g_data.m_dwMyImageLeftBlank + rect.left;
				int nY1 = nY + middle + rect.top;

				CGraphicsRoundRectPath  RoundRectPath;
				RoundRectPath.AddRoundRect(nX1 - EDT_REMARK_WIDTH / 2, nY1 + EDT_REMARK_Y_OFFSET,
					EDT_REMARK_WIDTH, EDT_REMARK_HEIGHT, 5, 5);
				graphics.DrawPath(&m_remark_pen, &RoundRectPath);
				graphics.FillPath(&m_remark_brush, &RoundRectPath);

				strText = pItem->szRemark;
				RECT rectRemark;
				rectRemark.left = nX1 - EDT_REMARK_WIDTH / 2;				
				rectRemark.top = nY1 + EDT_REMARK_Y_OFFSET;
				rectRemark.right = rectRemark.left + EDT_REMARK_WIDTH;
				rectRemark.bottom = rectRemark.top + EDT_REMARK_HEIGHT;				
				if (rectRemark.left < rectLeft.left) {
					rectRemark.left = rectLeft.left;
				}
				if (rectRemark.right < rectRemark.left) {
					rectRemark.right = rectRemark.left;
				}
				::DrawText(hDC, strText, strText.GetLength(), &rectRemark, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
		}
	}
	
	return true;
}

void CMyImageUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_BUTTONDOWN)
	{
		//告诉UIManager这个消息需要处理
		m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
		return;
	}
	DuiLib::CControlUI::DoEvent(event);	
}

void  CMyImageUI::AddTemp(DWORD dwIndex, DWORD dwTemp) {
	time_t now = time(0);

	TempData * pTemp = new TempData;
	pTemp->dwTemperature = dwTemp;
	pTemp->tTime = now;
	pTemp->dwIndex = m_dwNextTempIndex;
	m_dwNextTempIndex++;
	// skip -1 index
	if (m_dwNextTempIndex == -1) {
		m_dwNextTempIndex = 0;
	}
	pTemp->szRemark[0] = '\0';

	if ( m_vTempData[dwIndex].size() >= 1500 ) {
		vector<TempData *>::iterator it = m_vTempData[dwIndex].begin();
		TempData * pData = *it;
		m_vTempData[dwIndex].erase(it);
		delete pData;
	}
	m_vTempData[dwIndex].push_back(pTemp);

	// 重绘
	MyInvalidate();
}

time_t  CMyImageUI::GetFirstTime() {
	time_t  tTime = 0x7FFFFFFF;
	BOOL    bFind = FALSE;

	for ( DWORD i = 0; i < MAX_READERS_COUNT; i++ ) {
		if ( m_vTempData[i].size() > 0  ) {
			if (!bFind) {
				tTime = m_vTempData[i].at(0)->tTime;
				bFind = TRUE;
			}
			else if ( m_vTempData[i].at(0)->tTime < tTime ) {
				tTime = m_vTempData[i].at(0)->tTime;
			}
		}
	}

	if (bFind) {
		return tTime;
	}		
	else {
		return -1;
	}
}

time_t  CMyImageUI::GetLastTime() {
	time_t  tTime = -1;
	BOOL    bFind = FALSE;

	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		if (m_vTempData[i].size() > 0) {
			if (!bFind) {
				tTime = m_vTempData[i].at(m_vTempData[i].size()-1)->tTime;
				bFind = TRUE;
			}
			else if (m_vTempData[i].at(m_vTempData[i].size() - 1)->tTime > tTime) {
				tTime = m_vTempData[i].at(m_vTempData[i].size() - 1)->tTime;
			}
		}
	}

	if (bFind) {
		return tTime;
	}
	else {
		return -1;
	}
}

void   CMyImageUI::DrawTempPoint(int nIndex, Graphics & g, int x, int y, HDC hDc, int RADIUS /*= DEFAULT_POINT_RADIUS*/) {
	g.FillEllipse(m_temperature_brush[nIndex], x - RADIUS, y - RADIUS, 2 * RADIUS, 2 * RADIUS);
}

// 计算图像需要的宽度
int   CMyImageUI::CalcMinWidth() {
	time_t  tFirstTime = GetFirstTime();
	if ( tFirstTime < 0 ) {
		return 0;
	}

	time_t  tLastTime = GetLastTime();
	assert(tLastTime >= 0);

	DWORD  dwInterval = g_data.m_dwCollectInterval;
	DWORD  dwUnitWidth = g_data.m_dwCollectIntervalWidth;
	DWORD  dwLeftColumnWidth = g_data.m_dwMyImageLeftBlank;

	int nWidth = (int)((double)(tLastTime - tFirstTime) / dwInterval * dwUnitWidth);
	return nWidth + dwLeftColumnWidth + g_data.m_dwMyImageRightBlank;
}

void  CMyImageUI::MyInvalidate() {
	int nMinWidth = CalcMinWidth();
	this->SetMinWidth(nMinWidth);
	Invalidate();
	m_bSetParentScrollPos = TRUE;
}

void  CMyImageUI::SelectedReader(int nIndex) {
	m_dwSelectedReaderIndex = nIndex;
}

void  CMyImageUI::OnMyClick(const POINT * pPoint) {
	// 找到点击了哪个点
	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	RECT rect = this->GetPos();
	int  width = pParent->GetWidth();
	int  height = rect.bottom - rect.top;

	int nMinTemp    = g_data.m_dwMyImageMinTemp;
	int nGridCount  = MAX_TEMPERATURE - nMinTemp;
	int nMiddleTemp = (MAX_TEMPERATURE + nMinTemp) / 2;

	int nGridHeight = height / nGridCount;
	int nReminder = height % nGridCount;
	int nVMargin = MIN_MYIMAGE_VMARGIN;
	if (nVMargin * 2 > nReminder) {
		int nSpared = (nVMargin * 2 - nReminder - 1) / nGridCount + 1;
		nGridHeight -= nSpared;
	}

	time_t  tFirstTime = GetFirstTime();
	time_t  tLastTime = GetLastTime();

	int middle  = height / 2;
	int nRadius = 6;
	vector<TempData *>::iterator it;

	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];

		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;

			int nDiff = (int)(pItem->tTime - tFirstTime);
			int nX = (int)(((double)nDiff / g_data.m_dwCollectInterval) *  g_data.m_dwCollectIntervalWidth );
			int nY = (int)((nMiddleTemp * 100.0 - (double)pItem->dwTemperature) / 100.0 * nGridHeight);

			int nX1 = nX + g_data.m_dwMyImageLeftBlank + rect.left;
			int nY1 = nY + middle + rect.top;

			if (pPoint->x >= nX1 - 6 && pPoint->x <= nX1 + 6
				&& pPoint->y >= nY1 - 6 && pPoint->y <= nY1 + 6 ) {
				//JTelSvrPrint("you clicked the point!");

				// 如果之前在编辑状态
				if ( g_edRemark->IsVisible()) {
					OnEdtRemarkKillFocus(this);
				}

				m_dwCurTempIndex = pItem->dwIndex;
				RECT rectRemark;
				rectRemark.left = nX1 - EDT_REMARK_WIDTH / 2 + 1;
				rectRemark.top = nY1 + EDT_REMARK_Y_OFFSET + 3;
				rectRemark.right = rectRemark.left + EDT_REMARK_WIDTH - 2;
				rectRemark.bottom = rectRemark.top + EDT_REMARK_HEIGHT - 6;
				g_edRemark->SetPos(rectRemark);
				g_edRemark->SetText(pItem->szRemark);
				g_edRemark->SetVisible(true);
				g_edRemark->SetFocus();

				g_data.m_bAutoScroll = FALSE;
				break;
			}
		}
	}	
}

void  CMyImageUI::SetRemark(DuiLib::CDuiString & strRemark) {
	assert(m_dwCurTempIndex != -1);
	if (m_dwCurTempIndex == -1) {
		return;
	}

	vector<TempData *>::iterator it;
	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];
		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;
			if (pItem->dwIndex == m_dwCurTempIndex) {
				STRNCPY(pItem->szRemark, strRemark, sizeof(pItem->szRemark));
				return;
			}
		}
	}	
}

void  CMyImageUI::ExportExcel(const char * szPatientName) {
	CDuiString strText;

	if (!CExcel::IfExcelInstalled()) {
		::MessageBox(g_hWnd, "没有检测到系统安装了excel", "保存excel", 0);
		return;
	}

	DWORD  dwDataCnt = 0;
	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		dwDataCnt += m_vTempData[i].size();
	}
	if (dwDataCnt == 0) {
		strText.Format("没有温度数据，放弃保存excel" );
		::MessageBox(g_hWnd, strText, "保存excel", 0);
		return;
	}

	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名  

	char szTime[256];
	time_t now = time(0);
	Date2String_1(szTime, sizeof(szTime), &now);
	SNPRINTF(strFilename, sizeof(strFilename), "%s_%s", szPatientName, szTime);

	ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小  
	ofn.hwndOwner = g_hWnd;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄  
	ofn.lpstrFilter = TEXT("Excel Flie(*.xls)\0*.xls\0Excel Flie(*.xlsx)\0*.xlsx\0\0");//设置过滤  
	ofn.nFilterIndex = 1;//过滤器索引  
	ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL  
	ofn.nMaxFile = sizeof(strFilename);//缓冲区长度  
	ofn.lpstrInitialDir = CPaintManagerUI::GetInstancePath();
	ofn.lpstrTitle = TEXT("请选择一个文件");//使用系统默认标题留空即可  
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项  
	if (!GetSaveFileName(&ofn))
	{
		return;
	}

	CExcel  excel;
	excel.WriteGrid(0, 0, "姓名");
	excel.WriteGrid(0, 1, szPatientName);

	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		char tmp[64];
		SNPRINTF(tmp, sizeof(tmp), "时间%lu", i + 1);
		excel.WriteGrid(2, 0+i*4, tmp);

		SNPRINTF(tmp, sizeof(tmp), "体温%lu", i + 1);
		excel.WriteGrid(2, 1 + i * 4, tmp);

		SNPRINTF(tmp, sizeof(tmp), "注释%lu", i + 1);
		excel.WriteGrid(2, 2 + i * 4, tmp);
	}	

	std::vector<const char *> vValues;
	vector<TempData *>::iterator it;
	std::vector<const char *>::iterator  ix;

	for (DWORD i = 0; i < MAX_READERS_COUNT; i++) {
		vector<TempData *> & vTempData = m_vTempData[i];
		for (it = vTempData.begin(); it != vTempData.end(); it++) {
			TempData * pItem = *it;

			char * p = new char[32];
			Time2String(p, 32, &pItem->tTime);
			vValues.push_back(p);

			p = new char[8];
			SNPRINTF(p, 8, "%.2f", pItem->dwTemperature / 100.0);
			vValues.push_back(p);

			if (pItem->szRemark[0] != 0) {
				p = new char[MAX_REMARK_LENGTH];
				STRNCPY(p, pItem->szRemark, MAX_REMARK_LENGTH);
				vValues.push_back(p);
			}
			else {
				vValues.push_back(0);
			}
		}

		excel.WriteRange(3, 0+i*4, vTempData.size() + 3 - 1, 2+i*4, vValues);
		
		for (ix = vValues.begin(); ix != vValues.end(); ++ix) {
			if (*ix != 0) {
				delete[] * ix;
			}
		}
		vValues.clear();
	}
	
	int ret = excel.SaveAs(strFilename);
	excel.Quit();

	if (ret != 0) {
		strText.Format("保存excel数据失败。请确保excel文件没有打开");
		::MessageBox(g_hWnd, strText, "保存excel", 0);
	}
}