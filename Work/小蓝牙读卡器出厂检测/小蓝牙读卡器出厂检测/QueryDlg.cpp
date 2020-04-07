#include "QueryDlg.h"
#include "business.h"

CQueryDlg::CQueryDlg() {
	m_StartTime = 0;
	m_EndTime = 0;
	m_edMac = 0;
	m_lstResult = 0;
	m_btnQuery = 0;
	m_btnExport = 0;
}                
 
void  CQueryDlg::Notify(DuiLib::TNotifyUI& msg) {
	CDuiString  strName = msg.pSender->GetName();

	if (msg.sType == "click") {
		if (strName == "btnQuery") {
			OnQuery();
		}
	}
	WindowImplBase::Notify(msg);
}

void  CQueryDlg::InitWindow() {
	m_StartTime = static_cast<DuiLib::CDateTimeUI*>(m_PaintManager.FindControl("DateTimeStart"));
	m_EndTime = static_cast<DuiLib::CDateTimeUI*>(m_PaintManager.FindControl("DateTimeEnd"));
	m_edMac = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edQMac"));
	m_lstResult = static_cast<DuiLib::CListUI*>(m_PaintManager.FindControl("lstResult"));
	m_btnQuery = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnQuery"));
	m_btnExport = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnExport"));

	WindowImplBase::InitWindow();
}

LRESULT  CQueryDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

CControlUI * CQueryDlg::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void  CQueryDlg::OnQuery() {
	SYSTEMTIME s1 =  m_StartTime->GetTime();
	SYSTEMTIME s2 =  m_EndTime->GetTime();

	time_t t1 = GetAdZeroTime(SystemTime2Time(s1));
	time_t t2 = GetAdZeroTime(SystemTime2Time(s2));

	CDuiString  strMac = m_edMac->GetText();

	CBusiness::GetInstance()->QueryDataAsyn(t1, t2 + 86400, strMac);
}