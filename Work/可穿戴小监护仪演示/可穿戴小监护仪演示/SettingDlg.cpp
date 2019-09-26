#include "SettingDlg.h"


CSettingDlg::CSettingDlg() {
	m_nComPort = g_data.m_nComPort;
}

void   CSettingDlg::Notify(DuiLib::TNotifyUI& msg) {
	if (msg.sType == "itemselect") {
		if (msg.pSender->GetName() == "cmComPort") {
			int nSel = m_cmbComPorts->GetCurSel();
			if (nSel >= 0) {
				int nCom = m_cmbComPorts->GetItemAt(nSel)->GetTag();
				CDuiString strText;
				strText.Format("com%d", nCom);
				m_edComPort->SetText(strText);
			}			
		}
	}
	else if (msg.sType == "click") {
		if (msg.pSender->GetName() == "btnOK") {
			CDuiString strText;
			strText = m_edComPort->GetText();
			int nCom = 0;
			int ret = sscanf(strText, "com%d", &nCom);
			if (1 == ret) {
				m_nComPort = nCom;
			}
			this->PostMessage(WM_CLOSE);
		}
	}
	WindowImplBase::Notify(msg);                
}

void   CSettingDlg::InitWindow() {
	m_cmbComPorts = (CComboUI *)m_PaintManager.FindControl("cmComPort");
	m_edComPort = (CEditUI *)m_PaintManager.FindControl("edtComPort");
	CheckDevice();
	CDuiString  strText;
	if ( g_data.m_nComPort > 0 )
		strText.Format("com%d", g_data.m_nComPort);
	m_edComPort->SetText(strText);
	WindowImplBase::InitWindow();
}

LRESULT  CSettingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CSettingDlg::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

// ¼ì²éÓ²¼þ(´®¿Ú)
void  CSettingDlg::CheckDevice() {
	m_cmbComPorts->RemoveAll();

	std::vector<std::string> vComPorts;
	EnumPortsWdm(vComPorts);

	BOOL  bFindReader = FALSE;
	int   nFindeIndex = -1;

	std::vector<std::string>::iterator it;
	int i = 0;
	for (it = vComPorts.begin(); it != vComPorts.end(); it++, i++) {
		std::string & s = *it;

		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(s.c_str());
		m_cmbComPorts->Add(pElement);

		int nComPort = 0;
		const char * pFind = strstr(s.c_str(), "COM");
		while (pFind) {
			if (1 == sscanf(pFind + 3, "%d", &nComPort)) {
				pElement->SetTag(nComPort);
				break;
			}
			pFind = strstr(pFind + 3, "COM");
		}

		if (!bFindReader) {
			char tmp[256];
			Str2Lower(s.c_str(), tmp, sizeof(tmp));
			if (0 != strstr(tmp, "usb-serial ch340")) {
				bFindReader = TRUE;
				nFindeIndex = i;
			}
		}
	}

	//if (m_cmbComPorts->GetCount() > 0) {
	//	if (nFindeIndex >= 0) {
	//		m_cmbComPorts->SelectItem(nFindeIndex, false, false);
	//	}
	//	else {
	//		m_cmbComPorts->SelectItem(0, false, false);
	//	}
	//}
}