#include "SettingDlg.h"
#include "LmnSerialPort.h"


CSettingDlg::CSettingDlg() {
	m_tree = 0;
	m_comport = 0;
}

void   CSettingDlg::Notify(DuiLib::TNotifyUI& msg) {
	if (msg.sType == "windowinit") {
		OnMyInited();
	}
	WindowImplBase::Notify(msg);                 
}

void   CSettingDlg::InitWindow() {
	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl("CfgTree");

	m_comport = new CEdtComboUI;
	m_tree->AddNode("´®¿Ú", 0, 0, m_comport, 3, 0xFF666666, 0, 0, 36);
	WindowImplBase::InitWindow();  
}

void  CSettingDlg::OnMyInited() {
	CComboUI * pCombo = m_comport->m_cmb;

	pCombo->RemoveAll();
	
	std::map<int, std::string> ComPorts;
	GetAllSerialPorts(ComPorts);

	std::map<int, std::string>::iterator it;
	int i = 0;
	for (it = ComPorts.begin(); it != ComPorts.end(); it++, i++) {
		std::string & s = it->second;
		int nComPort = it->first;

		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(s.c_str());
		pElement->SetTag(nComPort);
		pCombo->Add(pElement);
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

