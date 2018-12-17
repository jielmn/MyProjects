#include "BindingDlg.h"

CBindingDlg::CBindingDlg() {
	memset(m_tTagItem, 0, size(m_tTagItem));
	m_dwPatientId = 0;
	m_dwIndex = 0;
	m_dwTagItemCnt = 0;
	m_layTags = 0;
}

void   CBindingDlg::Notify(DuiLib::TNotifyUI& msg) {
	WindowImplBase::Notify(msg);
}

void   CBindingDlg::InitWindow() {
	DuiLib::CDuiString   strText;

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
	WindowImplBase::InitWindow();            
} 

LRESULT  CBindingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_SIZE) {
		OnSize(wParam, lParam);
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