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