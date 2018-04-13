#include "SettingDlg.h"


CSettingDlg::CSettingDlg() {

}

void   CSettingDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;
	DWORD   dwTemp;
	double  dbTemp;

	if (msg.sType == "click") {
		if (name == "btnOK") {
			DWORD   dwInterval = 0;
			DWORD   dwLowAlarm = 0;
			DWORD   dwHighAlarm = 0;

			strText = m_edInteval->GetText();
			if (1 != sscanf_s(strText, "%lu", &dwTemp)) {
				::MessageBox(this->GetHWND(), "采集间隔请输入整数(1~60)", "设置", 0);
				return;
			}
			if (dwTemp > 60) {
				::MessageBox(this->GetHWND(), "采集间隔请输入整数(1~60)", "设置", 0);
				return;
			}
			dwInterval = dwTemp;

			strText = m_edLowAlarm->GetText();
			if (1 != sscanf_s(strText, "%lf", &dbTemp)) {
				::MessageBox(this->GetHWND(), "低温报警请输入数字", "设置", 0);
				return;
			}
			if ( dbTemp > 42 || dbTemp < 35 ) {
				::MessageBox(this->GetHWND(), "低温报警请输入范围(35~42)", "设置", 0);
				return;
			}
			dwLowAlarm = (DWORD)(dbTemp * 100);

			strText = m_edHighAlarm->GetText();
			if (1 != sscanf_s(strText, "%lf", &dbTemp)) {
				::MessageBox(this->GetHWND(), "高温报警请输入数字", "设置", 0);
				return;
			}
			if (dbTemp > 42 || dbTemp < 35) {
				::MessageBox(this->GetHWND(), "高温报警请输入范围(35~42)", "设置", 0);
				return;
			}
			if (dbTemp < dwLowAlarm / 100.0) {
				::MessageBox(this->GetHWND(), "高温报警请大于低温报警", "设置", 0);
				return;
			}
			dwHighAlarm = (DWORD)(dbTemp * 100);

			g_dwCollectInterval = dwInterval;
			g_dwLowTempAlarm = dwLowAlarm;
			g_dwHighTempAlarm = dwHighAlarm;

			strText = m_edAlarmPath->GetText();
			strncpy_s(g_szAlarmFilePath, strText, sizeof(g_szAlarmFilePath));

			PostMessage(WM_CLOSE);

		}
	}
	DuiLib::WindowImplBase::Notify(msg);
}

void   CSettingDlg::InitWindow() {
	m_edInteval   = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edInteval"));
	m_edLowAlarm  = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edLowTempAlarm"));
	m_edHighAlarm = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edHiTempAlarm"));
	m_edAlarmPath = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edAlarm"));

	DuiLib::CDuiString  strText;
	strText.Format( "%lu", g_dwCollectInterval );
	m_edInteval->SetText( strText );

	strText.Format("%.2f", g_dwLowTempAlarm / 100.0 );
	m_edLowAlarm->SetText(strText);

	strText.Format("%.2f", g_dwHighTempAlarm / 100.0 );
	m_edHighAlarm->SetText(strText);

	m_edAlarmPath->SetText(g_szAlarmFilePath);

	DuiLib::WindowImplBase::InitWindow();
}

LRESULT  CSettingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}