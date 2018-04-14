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
				::MessageBox(this->GetHWND(), "采集间隔请输入整数(5~60)", "设置", 0);
				return;
			}
			if ( dwTemp < 5 || dwTemp > 60) {
				::MessageBox(this->GetHWND(), "采集间隔请输入整数(5~60)", "设置", 0);
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
		else if (name == "btnBrowse") {
			OPENFILENAME ofn;       // common dialog box structure
			char szFile[260];       // buffer for file name
			//HWND hwnd;              // owner window

									// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = this->GetHWND();
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "mp3\0*.mp3\0wave\0*.wav\0";   //All\0*.*\0Text\0*.TXT\0
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// Display the Open dialog box. 

			if (GetOpenFileName(&ofn) == TRUE) {
				m_edAlarmPath->SetText(ofn.lpstrFile);
			}
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