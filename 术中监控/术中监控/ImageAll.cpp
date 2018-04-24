#include "ImageAll.h"
#include "exhDatabase.h"

CImageAll::CImageAll(DuiLib::CPaintManagerUI *pManager) {

	CMyDatabase::GetInstance()->GetAllTemp( m_vTempData );

	DWORD  dwTemp = 0;
	g_cfg->GetConfig("height per celsius", dwTemp, 20);
	m_nHeightPerCelsius = dwTemp;

	g_cfg->GetConfig("width per second", dwTemp, 1 );
	m_nWidthPerSecond = dwTemp;

	g_cfg->GetConfig("all image text offset x", dwTemp, 1);
	m_nTextOffsetX = dwTemp;

	g_cfg->GetConfig("all image text offset y", dwTemp, 1);
	m_nTextOffsetY = dwTemp;

	g_cfg->GetConfig("all image left", dwTemp, 15);
	m_nTextLeft = dwTemp;

	g_cfg->GetConfig("all image celsius count", dwTemp, 10);
	m_nCelsiusCnt = dwTemp;	

	g_cfg->GetConfig("all image time offset x", dwTemp, -25);
	m_nTimeTextOffsetX = dwTemp;

	

	DWORD dwCnt = m_vTempData.size(); 
	if ( dwCnt > 1 ) {
		int nWidth = (int)( ( m_vTempData[dwCnt - 1]->tTime - m_vTempData[0]->tTime ) * m_nWidthPerSecond );
		this->SetMinWidth(nWidth);
	}

	m_hPen = ::CreatePen(0, 1, RGB(0, 128, 64));
	m_hPen1 = ::CreatePen(0, 1, RGB(0, 0, 255));
}

CImageAll::~CImageAll() {
	ClearVector(m_vTempData);
	DeleteObject(m_hPen);
	DeleteObject(m_hPen1);
}

bool CImageAll::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {	
	DuiLib::CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	
	RECT rect   = this->GetPos();
	int  width  = rect.right - rect.left;
	int  height = rect.bottom - rect.top;
	DuiLib::CDuiString strText;
	// 温度范围 20 ~ 56

	// 38度位置
	int middle = height / 2;

	::SelectObject(hDC, m_hPen);
	::SetTextColor(hDC, RGB(68, 122, 161));

	::MoveToEx(hDC,  rect.left, middle + rect.top, 0);
	::LineTo(hDC, rect.right, middle + rect.top);
	strText = "38°";
	::TextOut(hDC, m_nTextLeft + m_nTextOffsetX, middle + rect.top + m_nTextOffsetY, strText, strText.GetLength());

	for (int i = 0; i < m_nCelsiusCnt; i++) {
		int h = middle + rect.top - (i + 1) * m_nHeightPerCelsius;
		::MoveToEx(hDC, rect.left,h, 0);
		::LineTo(hDC, rect.right, h);

		strText.Format("%d°", 38 + (i + 1));
		::TextOut(hDC, m_nTextLeft + m_nTextOffsetX, h + m_nTextOffsetY, strText, strText.GetLength());
	}

	for (int i = 0; i < m_nCelsiusCnt; i++) {
		int h = middle + rect.top + (i + 1) * m_nHeightPerCelsius;
		::MoveToEx(hDC, rect.left, h, 0);
		::LineTo(hDC, rect.right, h);

		strText.Format("%d°", 38 - (i + 1));
		::TextOut(hDC, m_nTextLeft + m_nTextOffsetX, h + m_nTextOffsetY, strText, strText.GetLength());
	}


	DuiLib::CVerticalLayoutUI * pParent = (DuiLib::CVerticalLayoutUI *)this->GetParent();
	DuiLib::CScrollBarUI* pScrollBar = pParent->GetHorizontalScrollBar();
	int nStartPos = pScrollBar->GetScrollPos();
	int nEndPos = nStartPos + (rcPaint.right - rcPaint.left);

	DWORD  dwCnt = m_vTempData.size();
	DWORD  dwStartIndex = 0;
	DWORD  dwEndIndex = 0;
	DWORD  dwMiddleIndex = 0;

	if (dwCnt == 0) {
		return true;
	}

	// 第一个点的时间，也是最早时间
	time_t  tFistTime = m_vTempData[0]->tTime;

	dwEndIndex    = dwCnt - 1;
	dwMiddleIndex = ( dwStartIndex + dwEndIndex ) / 2;
	int  nFindIndex = -1;

	do 
	{
		TempData * pMiddleItem = m_vTempData[dwMiddleIndex];
		int nDiff = (int)((pMiddleItem->tTime - tFistTime) * m_nWidthPerSecond);
		// 如果时间恰好是范围内的第一个点
		if (  nDiff == nStartPos ) {
			nFindIndex = dwMiddleIndex;
			break;
		}
		else if ( nDiff < nStartPos ) {
			dwStartIndex = dwMiddleIndex + 1;
			// 已经遍历完
			if ( dwStartIndex > dwEndIndex) {
				nFindIndex = dwStartIndex;
				break;
			}
			dwMiddleIndex = (dwStartIndex + dwEndIndex) / 2;
		}
		else {
			if ( nDiff >= nEndPos ) {
				if ( 0 == dwMiddleIndex ) {
					nFindIndex = -1;
					break;
				}
				dwEndIndex = dwMiddleIndex - 1;
				// 已经遍历完
				if (dwStartIndex > dwEndIndex) {
					nFindIndex = dwEndIndex;
					break;
				}
				dwMiddleIndex = (dwStartIndex + dwEndIndex) / 2;
			}
			else {
				if (0 == dwMiddleIndex) {
					nFindIndex = 0;
					break;
				}
				dwEndIndex = dwMiddleIndex - 1;
				// 已经遍历完
				if (dwStartIndex > dwEndIndex) {
					nFindIndex = dwMiddleIndex;
					break;
				}
				dwMiddleIndex = (dwStartIndex + dwEndIndex) / 2;
			}
		}

	} while ( TRUE );

	if ( nFindIndex < 0 ) {
		return true;
	}

	::SelectObject(hDC, m_hPen1);

	if (nFindIndex > 0) {
		nFindIndex--;
	}

	int nLastTextIndex = -1;
	BOOL bBreak = FALSE;
	char buf[8192];

	for (DWORD i = nFindIndex; i < dwCnt; i++) {
		TempData * pItem = m_vTempData[i];
		int nDiff = (int)((pItem->tTime - tFistTime) * m_nWidthPerSecond);
		if (nDiff >= nEndPos) {
			bBreak = TRUE;
		}

		int x = nDiff + rect.left;
		int y = middle - (int)((((int)pItem->dwTemperature - 3800) / 100.0) * m_nHeightPerCelsius) + rect.top;
		DrawTempPoint(x, y, hDC, 6);

		if (nFindIndex == i) {
			::MoveToEx(hDC, x, y, 0);
		}
		else {
			::LineTo(hDC, x, y);
		}
		
		if (nLastTextIndex == -1  ) {
			// 画时间
			if (nDiff - nStartPos >= 100) {
				Time2String( buf, sizeof(buf), &pItem->tTime);
				strText = buf;
				::TextOut(hDC, x + m_nTimeTextOffsetX, middle + m_nCelsiusCnt * m_nHeightPerCelsius + rect.top, strText, strText.GetLength());
				nLastTextIndex = i;
			}
		}
		else {
			if ((pItem->tTime - m_vTempData[nLastTextIndex]->tTime) * m_nWidthPerSecond >= 200) {
				Time2String(buf, sizeof(buf), &pItem->tTime);
				strText = buf;
				::TextOut(hDC, x + m_nTimeTextOffsetX, middle + m_nCelsiusCnt * m_nHeightPerCelsius + rect.top, strText, strText.GetLength());
				nLastTextIndex = i;
			}
		}

		if (bBreak) {
			break;
		}

	}

//	strText.Format("%d", nStartPos);
//	::TextOut(hDC, 100, 100, strText, strText.GetLength());

	return true;
}

void CImageAll::DoEvent(DuiLib::TEventUI& event) {
	DuiLib::CControlUI::DoEvent(event);
}

void   CImageAll::DrawTempPoint(int x, int y, HDC hDc, int RADIUS /*= 6*/) {
	::Ellipse(hDc, x - RADIUS, y - RADIUS, x + RADIUS, y + RADIUS);

	int nTmp = (int)(0.707 * RADIUS);

	POINT points[2] = { { x - nTmp, y - nTmp },{ x + nTmp, y + nTmp } };
	::Polyline(hDc, points, 2);

	POINT points1[2] = { { x + nTmp, y - nTmp },{ x - nTmp, y + nTmp } };
	::Polyline(hDc, points1, 2);
}