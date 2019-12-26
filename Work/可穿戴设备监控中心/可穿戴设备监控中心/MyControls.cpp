#include "MyControls.h"
#include "EditableButtonUI.h"

CEdtComboUI::CEdtComboUI() {
	m_cmb = 0;
	m_edt = 0;
	this->OnSize += MakeDelegate(this, &CEdtComboUI::OnMySize);
}


CEdtComboUI::~CEdtComboUI() {
	m_pManager->RemoveNotifier(this);
}

LPCTSTR CEdtComboUI::GetClass() const {
	return "EdtCombo";
}

void CEdtComboUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(_T("edtcombo.xml"), (UINT)0, 0, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow); 
	}
	else {
		this->RemoveAll();
		return;
	}

	m_cmb = (CComboUI *)m_pManager->FindControl("cmb");
	m_edt = (CEditUI *)m_pManager->FindControl("edt");
	m_pManager->AddNotifier(this);
}

bool CEdtComboUI::OnMySize(void * pParam) {
	m_edt->SetFixedWidth(m_rcItem.right - m_rcItem.left - 25);	
	return true;
}

void CEdtComboUI::Notify(TNotifyUI& msg) {
	if (msg.sType == "itemselect") {
		if (msg.pSender == m_cmb) {
			int nSel = m_cmb->GetCurSel();
			if (nSel >= 0) {
				CListLabelElementUI * pElement = (CListLabelElementUI *)m_cmb->GetItemAt(nSel);
				m_edt->SetText(pElement->GetText());
			}
		}
	}
}


CGridUI::CGridUI() : m_callback(m_pManager) {
	m_cstName = 0;
	m_lblHeartBeat = 0;
	m_lblOxy = 0;
	m_lblTemp = 0;
	m_lblDeviceId = 0;
	m_lblPose = 0;

	m_nIndex = 0;
	m_nHeartBeat = 0;
	m_nOxy = 0;
	m_nTemp = 0;
	m_nPose = 0;
	m_layMain = 0;
}

CGridUI::~CGridUI() {
	m_pManager->RemoveNotifier(this);
}

LPCTSTR CGridUI::GetClass() const {
	return "Grid";
}

void CGridUI::DoInit() {
	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create("grid.xml", (UINT)0, &m_callback, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
		m_pManager->AddNotifier(this);       
	}
	else {
		this->RemoveAll();
		return;
	}

	m_cstName = (CEdtComboUI *)m_pManager->FindControl("cstEdtBtnName");
	m_lblHeartBeat = (CLabelUI *)m_pManager->FindControl("lblBeat");
	m_lblOxy = (CLabelUI *)m_pManager->FindControl("lblOxy");
	m_lblTemp = (CLabelUI *)m_pManager->FindControl("lblTemp");
	m_lblDeviceId = (CLabelUI *)m_pManager->FindControl("lblDeviceId");
	m_lblIndex = (CLabelUI *)m_pManager->FindControl("lblIndex");
	m_lblPose = (CLabelUI *)m_pManager->FindControl("lblPose");
	m_layMain = (CVerticalLayoutUI *)m_pManager->FindControl("main");

	SetIndex();
	SetUserName();
	SetHeartBeat();
	SetOxy();
	SetTemp();
	SetDeviceId();
	SetPose();
}

void CGridUI::Notify(TNotifyUI& msg) {

}

void CGridUI::SetIndex(int nIndex) {
	m_nIndex = nIndex;
	SetIndex();
}

void CGridUI::SetIndex() {
	if (m_bInitiated) {
		CDuiString strText;
		if ( m_nIndex > 0 )
			strText.Format("%d", m_nIndex);
		m_lblIndex->SetText(strText);
	}
}

int  CGridUI::GetIndex() {
	return m_nIndex;
}

void CGridUI::SetUserName(CDuiString strName) {
	m_strName = strName;
	SetUserName();
}

void CGridUI::SetUserName() {
	if (m_bInitiated) {
		m_cstName->SetText(m_strName);
	}
}

CDuiString  CGridUI::GetUserName() {
	return m_strName;
}

void CGridUI::SetHeartBeat(int nHeartBeat) {
	m_nHeartBeat = nHeartBeat;
	SetHeartBeat();
}

void CGridUI::SetHeartBeat(CWearItem * pItem) {
	assert(pItem);
	SetHeartBeat(pItem->m_nHearbeat);
}

void CGridUI::SetHeartBeat() {
	if (m_bInitiated) {
		CDuiString strText;
		if (m_nHeartBeat > 0) {
			strText.Format("%d", m_nHeartBeat);
			m_lblHeartBeat->SetText(strText);
		}		
	}
}

int  CGridUI::GetHeartBeat() {
	return m_nHeartBeat;
}

void CGridUI::SetOxy(int nOxy) {
	m_nOxy = nOxy;
	SetOxy();
}

void CGridUI::SetOxy(CWearItem * pItem) {
	SetOxy(pItem->m_nOxy);
}

void CGridUI::SetOxy() {
	if (m_bInitiated) {
		CDuiString strText;
		if (m_nOxy > 0) {
			strText.Format("%d", m_nOxy);
			m_lblOxy->SetText(strText);
		}		
	}
}

int  CGridUI::GetOxy() {
	return m_nOxy;
}

void CGridUI::SetTemp(int nTemp) {
	m_nTemp = nTemp;
	SetTemp();
}

void CGridUI::SetTemp(CWearItem * pItem) {
	SetTemp(pItem->m_nTemp);
}

void CGridUI::SetTemp() {
	if (m_bInitiated) {
		CDuiString strText;
		if (m_nTemp > 0) {
			strText.Format("%.1f", m_nTemp / 100.0f);
			m_lblTemp->SetText(strText);
		}		
	}
}

int  CGridUI::GetTemp() {
	return m_nTemp;
}

void CGridUI::SetDeviceId(CDuiString strId) {
	m_strDeviceId = strId;
	SetDeviceId();
}

void CGridUI::SetDeviceId() {
	if (m_bInitiated) {
		m_lblDeviceId->SetText(m_strDeviceId);
	}
}

CDuiString  CGridUI::GetDeviceId() {
	return m_strDeviceId;
}

void CGridUI::SetPose(int nPose) {
	m_nPose = nPose;
	SetPose();
}

void CGridUI::SetPose() {
	if (m_bInitiated) {
		CDuiString strText;
		if (m_nPose > 0)
			strText.Format("%d", m_nPose);
		m_lblPose->SetText(strText);
	}
}

int  CGridUI::GetPose() {
	return m_nPose;
}

static DuiLib::CControlUI* CALLBACK CS_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData) {
	CDuiString strText;
	if (0 == strcmp(pSubControl->GetClass(), DUI_CTR_LABEL)) {
		((CLabelUI *)pSubControl)->SetTextColor((DWORD)lpData);
	}
	else if ( 0 == strcmp(pSubControl->GetClass(),"EditableButton") ) {
		CEditableButtonUI * btn = (CEditableButtonUI *)pSubControl;		
		strText.Format("#%X", (DWORD)lpData);
		btn->SetAttribute("btncolor", strText);
	}
	return 0;
}

void  CGridUI::CheckWarning() {
	BOOL  bHeartWarning = FALSE;
	BOOL  bOxyWarning = FALSE;
	BOOL  bTempWarning = FALSE;

	if ( m_nHeartBeat > 0 ) {
		if ( (m_nHeartBeat >= g_data.m_nMaxHeartBeat) || (m_nHeartBeat <= g_data.m_nMinHeartBeat) ) {
			bHeartWarning = TRUE;
		}
	}

	if ( m_nOxy > 0) {
		if ( m_nOxy <= g_data.m_nMinOxy ) {
			bOxyWarning = TRUE;
		}
	}

	if (m_nTemp > 0) {
		if ( m_nTemp <= g_data.m_nMinTemp || m_nTemp >= g_data.m_nMaxTemp ) {
			bTempWarning= TRUE;
		}
	}

	if ( bHeartWarning || bOxyWarning || bTempWarning ) {
		m_layMain->SetBkColor(ABNORMAL_COLOR);
		m_layMain->FindControl(CS_FINDCONTROLPROC, (LPVOID)0xFF333333, 0);
		if (bHeartWarning) {
			m_lblHeartBeat->SetTextColor(ABNORMAL_TEXT_COLOR);
		}
		if (bOxyWarning) {
			m_lblOxy->SetTextColor(ABNORMAL_TEXT_COLOR);
		}
		if (bTempWarning) {
			m_lblTemp->SetTextColor(ABNORMAL_TEXT_COLOR);
		}
	}
	else {
		m_layMain->SetBkColor(NORMAL_COLOR);
		m_layMain->FindControl(CS_FINDCONTROLPROC, (LPVOID)0xFFFFFFFF, 0);
	}
}

CMyImageUI::CMyImageUI() {
	m_hCommonThreadPen = ::CreatePen(PS_SOLID, 1, RGB(0x66, 0x66, 0x66));
}

CMyImageUI::~CMyImageUI() {
	DeleteObject(m_hCommonThreadPen);

	ClearVector(m_vHeartBeat);
	ClearVector(m_vOxy);
	ClearVector(m_vTemp);
}

bool CMyImageUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) {
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	// GDI+
	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	// self rectangle and width, height
	RECT rect   = GetPos();
	int  width  = GetMyWidth();
	int  height = rect.bottom - rect.top;
	// 水平滑动条位置
	int  nScrollX = GetMyScrollX();

	// 显示的最低温度和最高温度
	int  nMinTemp,  nMaxTemp;              // 左边温度刻度
	int  nMinScale, nMaxScale;             // 右边数值刻度
	// 34℃ -- 70刻度，  37℃ -- 100刻度，也就是1℃高度对应10刻度高度
	GetMaxMinScale( nMinTemp, nMaxTemp, nMinScale, nMaxScale );

	// 摄氏度个数
	int  nCelsiusCount = nMaxTemp - nMinTemp;
	// 每个摄氏度的高度
	int  nHeightPerCelsius = GetCelsiusHeight(height, nCelsiusCount);
	// 垂直留白
	int  nVMargin = (height - nHeightPerCelsius * nCelsiusCount) / 2;
	// 最高温度的Y坐标系值
	int  nMaxY = rect.top + nVMargin;

	// 全图分为左边温度刻度区域，中间折线图区域和右边刻度区域
	RECT rcScale1;
	rcScale1.left   = rect.left + nScrollX;
	rcScale1.top    = rect.top;
	rcScale1.right  = rcScale1.left + MYIMAGE_H_MARGIN;
	rcScale1.bottom = rect.bottom;

	RECT rcScale2;
	rcScale2.left   = rect.right - MYIMAGE_H_MARGIN;
	rcScale2.top    = rect.top;
	rcScale2.right  = rect.right;
	rcScale2.bottom = rect.bottom;

	// 画刻度线
	DrawScale(hDC, nMaxTemp, nCelsiusCount, nHeightPerCelsius, nMaxY, rcScale1, rcScale2, width);

	return true;
}

void CMyImageUI::DoEvent(DuiLib::TEventUI& event) {
	CControlUI::DoEvent(event);
}

LPCTSTR CMyImageUI::GetClass() const {
	return "MyImage";
}

int CMyImageUI::GetMyWidth() {
	CControlUI * pParent = GetParent();
	assert(pParent);
	return pParent->GetWidth();
}

int CMyImageUI::GetMyScrollX() {
	CContainerUI * pParent = (CContainerUI *)GetParent();
	assert(pParent);
	SIZE s = pParent->GetScrollPos();
	return s.cx;
}

void  CMyImageUI::GetMaxMinScale( int & nMinTemp, int & nMaxTemp, int & nMinScale, int & nMaxScale ) {
	nMinTemp = 34;
	nMaxTemp = 37;

	int nMinTemperature = nMinTemp * 100;
	int nMaxTemperature = nMaxTemp * 100;

	std::vector<DataItem *>::iterator it;
	for ( it = m_vTemp.begin(); it != m_vTemp.end(); ++it ) {
		DataItem * pItem = *it;
		if ( pItem->nData < nMinTemperature ) {
			nMinTemperature = pItem->nData / 100 * 100;
		}
		else if (pItem->nData > nMaxTemperature) {
			nMaxTemperature = ( ( pItem->nData - 1 ) / 100 + 1 ) * 100;
		}
	}

	// 转化为scale刻度
	nMinScale = (nMinTemperature - 3700) / 100 * 10 + 100;
	nMaxScale = (nMaxTemperature - 3700) / 100 * 10 + 100;

	for ( it = m_vHeartBeat.begin(); it != m_vHeartBeat.end(); ++it ) {
		DataItem * pItem = *it;
		if (pItem->nData < nMinScale) {
			nMinScale = pItem->nData / 10 * 10;
		}
		else if (pItem->nData > nMaxScale) {
			nMaxScale = ((pItem->nData - 1) / 10 + 1) * 10;
		}
	}

	for (it = m_vOxy.begin(); it != m_vOxy.end(); ++it) {
		DataItem * pItem = *it;
		if (pItem->nData < nMinScale) {
			nMinScale = pItem->nData / 10 * 10;
		}
		else if (pItem->nData > nMaxScale) {
			nMaxScale = ((pItem->nData - 1) / 10 + 1) * 10;
		}
	}

	// 转化为温度刻度
	nMinTemp = (nMinScale - 100) / 10 + 37;
	nMaxTemp = (nMaxScale - 100) / 10 + 37;
}

int  CMyImageUI::GetCelsiusHeight(int height, int nCelsiusCount, int nVMargin /*= MYIMAGE_V_MARGIN*/) {
	int h = height / nCelsiusCount;
	int r = height % nCelsiusCount;
	// int nVMargin = MIN_MYIMAGE_VMARGIN;

	if (nVMargin * 2 > r) {
		int nSpared = (nVMargin * 2 - r - 1) / nCelsiusCount + 1;
		h -= nSpared;
	}
	return h;
}

void  CMyImageUI::DrawScale( HDC hDC, int nMaxTemp, int nCelsiusCnt, int nHeightPerCelsius, int nMaxY,
	                         const RECT & rcScale1, const RECT & rcScale2, int width ) {
	HPEN hOldPen = (HPEN)SelectObject(hDC, m_hCommonThreadPen);

	::MoveToEx(hDC, rcScale1.right, rcScale1.top, 0);
	::LineTo(hDC, rcScale1.right, rcScale1.bottom);

	::MoveToEx(hDC, rcScale2.left, rcScale2.top, 0);
	::LineTo(hDC, rcScale2.left, rcScale2.bottom);

	CDuiString strText;
	for ( int i = 0; i <= nCelsiusCnt; i++ ) {
		int nY = nMaxY + nHeightPerCelsius * i;
		::MoveToEx(hDC, rcScale1.right, nY, 0);
		::LineTo(hDC, rcScale2.left, nY);

		int nTemp = nMaxTemp - i;
		strText.Format("%d℃", nTemp);
		::TextOut( hDC, rcScale1.right + (-40), nY + (-8),
			strText, strText.GetLength());

		int nScale = (nTemp - 37) * 10 + 100;
		strText.Format("%d", nScale);
		::TextOut(hDC, rcScale2.left + (10), nY + (-8),
			strText, strText.GetLength());
	}



	SelectObject(hDC, hOldPen);
}