#include "TagUI.h"
#include "business.h"

CTagUI::CTagUI() : m_callback(m_pManager) {
	m_bInited = FALSE;
	memset(&m_item, 0, sizeof(m_item));
	m_nBindingGridIndex = 0;
}

CTagUI::~CTagUI() {
	m_pManager->RemoveNotifier(this);
}


LPCTSTR CTagUI::GetClass() const {
	return "TagUI";
}

void CTagUI::DoInit() {
	if (m_bInited) {
		return;
	}

	CDialogBuilder builder;
	CContainerUI* pChildWindow = static_cast<CHorizontalLayoutUI*>(builder.Create(TAG_FILE_NAME, (UINT)0, &m_callback, m_pManager));
	if (pChildWindow) {
		this->Add(pChildWindow);
		m_pManager->AddNotifier(this);
	}
	else {
		this->RemoveAll();
		m_bInited = TRUE;
		return;
	}

	m_lblReaderId = static_cast<DuiLib::CLabelUI *>(m_pManager->FindControl(LBL_HAND_READER_ID));
	m_lblTagId    = static_cast<DuiLib::CLabelUI *>(m_pManager->FindControl(LBL_HAND_TAG_ID));
	m_lblTempTime = static_cast<DuiLib::CLabelUI *>(m_pManager->FindControl(LBL_HAND_TEMP_TIME));
	m_cstPatientName = static_cast<CEditableButtonUI *>(m_pManager->FindControl(CST_HAND_P_NAME));
	m_lblTemp = static_cast<DuiLib::CLabelUI *>(m_pManager->FindControl(LBL_HAND_TEMP));
	m_lblBinding = static_cast<DuiLib::CLabelUI *>(m_pManager->FindControl(LBL_HAND_BINDING));

	m_bInited = TRUE;
}
  
void CTagUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) {
	CContainerUI::SetAttribute(pstrName, pstrValue);
}
   
void CTagUI::Notify(TNotifyUI& msg) {
	if (msg.sType == "textchanged") {
		if (msg.pSender == m_cstPatientName) {
			OnTagNameChanged();
		}
	}
} 
  
void  CTagUI::OnHandTemp(const TempItem * pItem, const char * szName) {
	assert(m_bInited);
	char  szTime[256];

	memcpy(&m_item, pItem, sizeof(TempItem));

	m_lblReaderId->SetText(pItem->m_szReaderId);
	m_lblTagId->SetText(pItem->m_szTagId);
	DateTime2String(szTime, sizeof(szTime), &pItem->m_time);
	m_lblTempTime->SetText(szTime);

	CDuiString  strName = m_cstPatientName->GetText();
	if ( 0 != strcmp(szName, strName) ) {
		m_cstPatientName->SetText(szName);
	}

	CDuiString  strText;
	strText.Format("%.2f", pItem->m_dwTemp / 100.0);
	m_lblTemp->SetText(strText);
}

void  CTagUI::DoEvent(DuiLib::TEventUI& event) {
	if (event.Type == UIEVENT_BUTTONDOWN) {		
		m_pManager->SendNotify(this, "tag_selected");
	}
	CContainerUI::DoEvent(event);
}

CDuiString CTagUI::GetTagId() {
	assert(m_bInited);
	return m_lblTagId->GetText();
}

void  CTagUI::SetBindingGridIndex(int nIndex) {
	assert(m_bInited);
	m_nBindingGridIndex = nIndex;

	CDuiString strText;
	if ( m_nBindingGridIndex > 0 ) {
		strText.Format("°ó¶¨´²ºÅ%d", m_nBindingGridIndex);
		m_lblBinding->SetText(strText);
		m_lblBinding->SetTextColor(0xFFCAF100);
	}
	else {
		m_lblBinding->SetText("Î´°ó¶¨´²Î»");
		m_lblBinding->SetTextColor(0xFFCCCCCC);
	}
}

int  CTagUI::GetBindingGridIndex() {
	assert(m_bInited);
	return m_nBindingGridIndex;
}

void CTagUI::OnTagNameChanged() {
	assert(m_item.m_szTagId[0] != '\0');
	CDuiString strName = m_cstPatientName->GetText();
	CBusiness::GetInstance()->SaveTagPNameAsyn(m_item.m_szTagId, strName);
	if ( m_nBindingGridIndex > 0 )
		::SendMessage( g_data.m_hWnd, UM_TAG_NAME_CHANGED, (WPARAM)this, 0 );
}

CDuiString CTagUI::GetPTagName() {
	assert(m_bInited);
	return m_cstPatientName->GetText();
}