#include "BindingDlg.h"

CBindingDlg::CBindingDlg() {
	memset(m_tTagItem, 0, size(m_tTagItem));
	m_dwPatientId = 0;
	m_dwIndex = 0;
	m_dwTagItemCnt = 0;
	m_layTags = 0;

	m_dwBindingPatientId = 0;
	memset( m_szBindingPatientName, 0, sizeof(m_szBindingPatientName) );
	memset( m_szTagName, 0, sizeof(m_szTagName) );
}

void   CBindingDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	if (msg.sType == "click" && name == "btnOK") {
		OnBtnOk();
	}
	WindowImplBase::Notify(msg);
}

void   CBindingDlg::InitWindow() {
	DuiLib::CDuiString   strText;

	m_lstPatients = static_cast<CListUI *>(m_PaintManager.FindControl("lstPatients"));
	m_lstTags = static_cast<CListUI *>(m_PaintManager.FindControl("lstTags"));
	m_layTags = static_cast<CTileLayoutUI *>(m_PaintManager.FindControl("layTags"));
	m_layTags->SetFixedColumns(1);
	int nBodyPartsCnt = g_vBodyParts.size();

	assert(m_dwTagItemCnt <= MAX_READERS_PER_GRID);
	for ( DWORD i = 0; i < m_dwTagItemCnt; i++ ) {
		CDialogBuilder builder;
		m_pTags[i] = builder.Create("tag.xml", (UINT)0, 0, &m_PaintManager);
		m_pTags[i]->SetName("tag");
		m_pTags[i]->SetTag(i);

		m_lblTagPos[i] = static_cast<CLabelUI*>(m_pTags[i]->FindControl(MY_FINDCONTROLPROC, "lblTagPos", 0));
		m_lblTagPos[i]->SetTag(i);
		strText.Format("%02lu%c", m_dwIndex + 1, m_tTagItem[i].m_dwSubIndex + 'A');
		m_lblTagPos[i]->SetText(strText);

		m_lblTagId[i] = static_cast<CLabelUI*>(m_pTags[i]->FindControl(MY_FINDCONTROLPROC, "lblTagId", 0));
		m_lblTagId[i]->SetTag(i);
		m_lblTagId[i]->SetText(m_tTagItem[i].m_szTagId);

		m_cmbTagName[i] = static_cast<CComboUI*>(m_pTags[i]->FindControl(MY_FINDCONTROLPROC, "cmbTagName", 0));
		m_cmbTagName[i]->SetTag(i);

		std::vector<BodyPart *>::iterator it;
		for (it = g_vBodyParts.begin(); it != g_vBodyParts.end(); ++it) {
			BodyPart * pBody = *it;

			CListLabelElementUI * pElement = new CListLabelElementUI;
			pElement->SetText(pBody->m_szName);
			m_cmbTagName[i]->Add(pElement);
		}

		if ( (int)i < nBodyPartsCnt ) {
			m_cmbTagName[i]->SelectItem(i);
		}
		else {
			if (m_cmbTagName[i]->GetCount() > 0) {
				m_cmbTagName[i]->SelectItem(0);
			}
		}
		
		m_layTags->Add(m_pTags[i]);             
	}

	CBusiness::GetInstance()->GetAllPatientsAsyn(this->GetHWND(), m_dwPatientId);
	WindowImplBase::InitWindow();            
} 

LRESULT  CBindingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_SIZE) {
		OnSize(wParam, lParam);
	} 
	else if (uMsg == UM_GET_ALL_PATIENTS_RET) {
		OnAllPatientsRet(wParam, lParam);
	}
	else if (uMsg == UM_BIND_TAGS_RET) {
		OnBindTagsRet(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CBindingDlg::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void   CBindingDlg::OnSize(WPARAM wParam, LPARAM lParam) {
	if ( m_layTags == 0 ) {
		return;
	}

	DWORD dwWidth = LOWORD(lParam);
	DWORD dwHeight = HIWORD(lParam);

	SIZE s;
	s.cx = dwWidth;
	s.cy = 30;
	m_layTags->SetItemSize(s);         
}

void  CBindingDlg::OnAllPatientsRet(WPARAM wParam, LPARAM lParam) {
	std::vector<Patient *> * pvRet = (std::vector<Patient *> *)wParam;
	DuiLib::CDuiString  strText;

	std::vector<Patient *>::iterator it;
	int i = 0;
	for ( it = pvRet->begin(); it != pvRet->end(); ++it, ++i ) {
		Patient * pItem = *it;

		CListTextElementUI* pListElement = new CListTextElementUI;
		m_lstPatients->Add(pListElement);

		strText.Format("%d", i + 1);
		pListElement->SetText(0, strText);
		pListElement->SetText(1, pItem->m_szId);
		pListElement->SetText(2, pItem->m_szName);
		pListElement->SetText(3, pItem->m_bMale ? "男":"女");
		strText.Format("%lu", pItem->m_dwAge);
		pListElement->SetText(4, strText);
		pListElement->SetTag(pItem->m_dwPatientId);      
	}	

	ClearVector(*pvRet); 
	delete pvRet;
}

void   CBindingDlg::OnBtnOk() {
	int  nSelectedIndex = m_lstPatients->GetCurSel();
	if ( nSelectedIndex < 0 ) {
		::MessageBox(GetHWND(), "没有选中要绑定的病人", "绑定", 0);
		return;
	}

	CListTextElementUI * pSelected = (CListTextElementUI *)m_lstPatients->GetItemAt(nSelectedIndex);
	m_dwBindingPatientId = pSelected->GetTag();
	STRNCPY(m_szBindingPatientName, pSelected->GetText(2), sizeof(m_szBindingPatientName));

	TagBinding_1  items[MAX_READERS_PER_GRID];
	for ( DWORD i = 0; i < m_dwTagItemCnt; i++) {
		STRNCPY(items[i].m_szTagId, m_tTagItem[i].m_szTagId, sizeof(items[i].m_szTagId));
		if ( m_cmbTagName[i]->GetCurSel() < 0 ) {
			items[i].m_szTagName[0] = '\0';
			m_szTagName[i][0] = '\0';
		}
		else {
			STRNCPY(items[i].m_szTagName, m_cmbTagName[i]->GetText(), sizeof(items[i].m_szTagName));
			STRNCPY(m_szTagName[i], m_cmbTagName[i]->GetText(), sizeof(m_szTagName[i]));
		}
	}

	CBusiness::GetInstance()->SetBindingAsyn(GetHWND(), m_dwBindingPatientId, items, m_dwTagItemCnt);
}

void   CBindingDlg::OnBindTagsRet(WPARAM wParam, LPARAM lParam) {
	int nRet = wParam;
	if (0 == nRet) {
		PostMessage(WM_CLOSE);
	}
	else {
		::MessageBox(GetHWND(), "绑定失败", "绑定", 0);
	}
}