#include "MyControls.h"

// 348,212

int CDeviceUI::FIXED_WIDTH = 338;
int CDeviceUI::FIXED_HEIGHT = 212;

CDeviceUI::CDeviceUI() : m_callback(m_pManager) {
	m_rule = 0;
	m_btnDel = 0;
	m_bHighlight = FALSE;

	m_nDeviceId    = 0;
	m_nTemperature = 0;
	m_tTime        = 0;

	m_lblAddress = 0;
	m_lblDeviceId = 0;
	m_lblTemp = 0;
	m_lblTempTime = 0;
}

CDeviceUI::~CDeviceUI() {
	m_pManager->RemoveNotifier(this);                          
}

LPCTSTR CDeviceUI::GetClass() const {
	return "Device";
}

void CDeviceUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(_T("DeviceUI.xml"), (UINT)0, &m_callback, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
		m_pManager->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		return;
	}

	m_rule = (CTempRuleUI *)m_pManager->FindControl("rule");
	//m_rule->SetBkColor(0xFFFF0000 );    

	m_btnDel = (CButtonUI *)m_pManager->FindControl("btnDel");
	m_btnDel->SetVisible(false);

	m_lblAddress = (CLabelUI *)m_pManager->FindControl("lblAddress");
	m_lblDeviceId = (CLabelUI *)m_pManager->FindControl("lblDeviceId");
	m_lblTemp = (CLabelUI *)m_pManager->FindControl("lblTemp");
	m_lblTempTime = (CLabelUI *)m_pManager->FindControl("lblTime");

	SetTemp();
	SetAddress();
	SetDeviceId();

	m_rule->SetTemp(m_nTemperature);

	this->SetFixedWidth(FIXED_WIDTH);
	this->SetFixedHeight(FIXED_HEIGHT);
}

void CDeviceUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CDeviceUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_MOUSEENTER) {
		if (!m_bHighlight) {
			m_pManager->SendNotify(this, "device_selected");
		}
	}
	else if (event.Type == UIEVENT_MOUSELEAVE) {
		if (m_bHighlight) {
			m_pManager->SendNotify(this, "device_unselected");
		}
	}
	else if (event.Type == UIEVENT_DBLCLICK) {
		m_pManager->SendNotify(this, "device_dbclick");
	}
	CContainerUI::DoEvent(event);
}

void CDeviceUI::Notify(TNotifyUI& msg) {

}

void  CDeviceUI::SetHighlight(BOOL bHighlight) {
	if (bHighlight) {
		if (!m_bHighlight) {
			this->SetBkColor(0x30000000);
			m_btnDel->SetVisible(true);
			m_bHighlight = TRUE;
		}
	}
	else {
		if (m_bHighlight) {
			this->SetBkColor(0x00000000);
			m_btnDel->SetVisible(false);
			m_bHighlight = FALSE;
		}
	}
}

void  CDeviceUI::SetAddress(const char * szAddr) {
	m_strAddress = szAddr;
	SetAddress();
}

void  CDeviceUI::SetTemp(int nTemp, time_t tTime) {
	m_nTemperature = nTemp;
	m_tTime = tTime;
	SetTemp();
}

int  CDeviceUI::GetDeviceId() {
	return m_nDeviceId;
}

void  CDeviceUI::SetDeviceId(int nDeviceId) {
	m_nDeviceId = nDeviceId;
	SetDeviceId();
}

void  CDeviceUI::SetAddress( ) {
	if ( m_bInitiated ) {
		m_lblAddress->SetText(m_strAddress);
	}
}

void  CDeviceUI::SetTemp( ) {
	if (m_bInitiated) {
		CDuiString  strText;
		char  szTime[256];

		if (m_nTemperature > 0 && m_tTime > 0) {
			strText.Format("%.1f", m_nTemperature / 100.0f);
			m_lblTemp->SetText(strText);

			LmnFormatTime(szTime, sizeof(szTime), m_tTime, "%Y-%m-%d %H:%M:%S");
			m_lblTempTime->SetText(szTime);

			m_rule->SetTemp(m_nTemperature);

			if ( m_nTemperature > g_data.m_nHighTemp ) {
				m_lblTemp->SetTextColor(0xFFFF0000);
			}
			else {
				m_lblTemp->SetTextColor(0xFF14B914);
			}
		}
		else {
			m_lblTemp->SetText("");
			m_lblTempTime->SetText(""); 
		}
	}
}

void  CDeviceUI::SetDeviceId( ) {
	if (m_bInitiated) {
		CDuiString  strText;
		strText.Format("%08X", m_nDeviceId);
		m_lblDeviceId->SetText(strText);
	}
}




int  CTempRuleUI::RULE_WIDTH = 48;
int  CTempRuleUI::RULE_HEIGHT = 170;
int  CTempRuleUI::MAX_TEMP = 12000;

CTempRuleUI::CTempRuleUI() {
	m_hPen   = ::CreatePen(PS_SOLID, 1, RGB(0xCC, 0xCC, 0xCC));
	m_hBrush = ::CreateSolidBrush(RGB(0x14,0xB9, 0x14));
	m_hFont  = CreateFont(-10, 0, 0, 0, FW_THIN, true, false, false,
		CHINESEBIG5_CHARSET, OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_MODERN, "ËÎÌå");
	m_nTemp = 3223;

}

CTempRuleUI::~CTempRuleUI() {
	::DeleteObject(m_hPen);
	::DeleteObject(m_hBrush);
	::DeleteObject(m_hFont);
}

LPCTSTR CTempRuleUI::GetClass() const {
	return "TempRule";
}

void  CTempRuleUI::SetTemp(int nTemp) {
	m_nTemp = nTemp;
	Invalidate();
}

// scale range: 0 - 120 ÉãÊÏ¶È
bool CTempRuleUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	HPEN  hOldPen = (HPEN)::SelectObject(hDC, m_hPen);

	// »­±ß¿ò
	RECT  rcPos = this->GetPos();
	rcPos.bottom -= 6;

	::Rectangle(hDC, rcPos.left, rcPos.bottom - RULE_HEIGHT, rcPos.left + RULE_WIDTH, rcPos.bottom);

	if (m_nTemp < 0) {
		m_nTemp = 0;
	}
	else if (m_nTemp > MAX_TEMP) {
		m_nTemp = MAX_TEMP;
	}

	int nTempHeight = (int)round( (float)m_nTemp / MAX_TEMP * RULE_HEIGHT );

	RECT  rcTemp;
	rcTemp.left   = rcPos.left + 1;
	rcTemp.right  = rcPos.left + RULE_WIDTH - 1;
	rcTemp.bottom = rcPos.bottom - 1;
	rcTemp.top    = rcPos.bottom - nTempHeight;

	// »­ÎÂ¶È
	::FillRect(hDC, &rcTemp, m_hBrush);

	// »­¿Ì¶È
	int nSegmentCnt   = 10;
	int nTempInterval = MAX_TEMP / 100 / nSegmentCnt;
	int nY0 = rcPos.bottom;
	int nYInterval = RULE_HEIGHT / nSegmentCnt;
	CDuiString strText;

	HFONT  hOldFont = (HFONT)::SelectObject(hDC, m_hFont);
	::SetTextColor(hDC, RGB(0x44, 0x7A, 0xA1)); 
	::SetBkMode(hDC, TRANSPARENT);

	for (int i = 0; i <= nSegmentCnt; i++) {
		int nX = rcPos.left + RULE_WIDTH - 1;
		int nY = nY0 - nYInterval * i;

		if (i > 0 && i < nSegmentCnt) {
			::MoveToEx(hDC, nX, nY, 0);
			::LineTo(hDC, nX - 10, nY);
		}		

		strText.Format("%d", nTempInterval * i);
		::TextOut(hDC, nX + 6, nY - 6, strText, strText.GetLength());
	}


	::SelectObject(hDC, hOldPen);  
	::SelectObject(hDC, hOldFont);
	return true;
}

void CTempRuleUI::DoEvent(DuiLib::TEventUI& event) {
	CControlUI::DoEvent(event);
}
   