#include "SettingDlg.h"
#include "LmnSerialPort.h"


CSettingDlg::CSettingDlg() {
	m_tree = 0;
	m_comport = 0;
	m_nComPort = 0;
}

void   CSettingDlg::Notify(DuiLib::TNotifyUI& msg) {
	if (msg.sType == "windowinit") {
		OnMyInited();
	}
	else if (msg.sType == "click") {
		if ( msg.pSender->GetName() == "btnOK") {
			OnMyOk();
		}
	}
	WindowImplBase::Notify(msg);                 
}

void   CSettingDlg::InitWindow() {
	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl("CfgTree");

	m_comport = new CEdtComboUI;
	m_tree->AddNode("串口", 0, 0, m_comport, 3, 0xFF666666, 0, 0, 36);	

	WindowImplBase::InitWindow();  
}

void  CSettingDlg::OnMyInited() {
	CComboUI * pCombo = m_comport->m_cmb;

	pCombo->RemoveAll();
	
	std::map<int, std::string> ComPorts;
	GetAllSerialPorts(ComPorts);

	std::map<int, std::string>::iterator it;
	int nSel = -1;
	int i = 0;
	for (it = ComPorts.begin(); it != ComPorts.end(); it++, i++) {
		std::string & s = it->second;
		int nComPort = it->first;

		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(s.c_str());
		pElement->SetTag(nComPort);
		pCombo->Add(pElement);

		if ( nComPort == m_nComPort ) {
			nSel = i;
		}
	}

	if ( nSel >= 0 ) {
		pCombo->SelectItem(nSel);
		CListLabelElementUI * pElement = (CListLabelElementUI *)pCombo->GetItemAt(nSel);
		m_comport->m_edt->SetText(pElement->GetText());
	}
	else {
		if (m_nComPort > 0) {
			CDuiString strText;
			strText.Format("com%d", m_nComPort);
			m_comport->SetText(strText);
		}
	}
}

LRESULT  CSettingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CSettingDlg::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyTree")) {
		return new CMyTreeCfgUI(100); 
	}
	return WindowImplBase::CreateControl(pstrClass);
}

int   CSettingDlg::GetComPort(CDuiString strCom) {
	char buf[256];
	Str2Lower(strCom, buf, sizeof(buf));
	int nComPort = 0;
	int ret = sscanf(buf, "com%d", &nComPort);
	if (ret == 1) {
		return nComPort;
	}
	else {
		return 0;
	}
}

void  CSettingDlg::OnMyOk() {
	CDuiString strComPort = m_comport->m_edt->GetText();
	int nSel = m_comport->m_cmb->GetCurSel();
	if ( nSel >= 0 ) {
		CListLabelElementUI * pElement = (CListLabelElementUI *)m_comport->m_cmb->GetItemAt(nSel);
		CDuiString strComPort_1 = pElement->GetText();
		if ( strComPort_1 == strComPort ) {
			m_nComPort = pElement->GetTag();
		}
		else {
			m_nComPort = GetComPort(strComPort);
		}
	}
	else {
		m_nComPort = GetComPort(strComPort);
	}

	if (m_nComPort <= 0) {
		MessageBox(GetHWND(), "串口格式不对", "错误", 0);
		return;
	}

	PostMessage(WM_CLOSE);   
}

