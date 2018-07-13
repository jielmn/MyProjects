#include "SettingDlg.h"


CSettingDlg::CSettingDlg() {

}

void   CSettingDlg::Notify(DuiLib::TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;
	//DWORD   dwTemp;
	double  dbTemp;

	if (msg.sType == "click") {
		if (name == "btnOK") {
			DWORD   dwInterval = 0;
			DWORD   dwLowAlarm = 0;
			DWORD   dwHighAlarm = 0;
			DWORD   dwMinTemp = 0;

			//strText = m_edInteval->GetText();
			//if (1 != sscanf_s(strText, "%lu", &dwTemp)) {
			//	::MessageBox(this->GetHWND(), "采集间隔请输入整数(5~60)", "设置", 0);
			//	return;
			//}
			//if ( dwTemp < 5 || dwTemp > 60) {
			//	::MessageBox(this->GetHWND(), "采集间隔请输入整数(5~60)", "设置", 0);
			//	return;
			//}
			//dwInterval = dwTemp;

			CMyTreeCfgUI::ConfigValue  cfgValue;
			bool bGetCfg = m_tree->GetConfigValue(0, cfgValue);
			if (0 == cfgValue.m_nComboSel) {
				dwInterval = 10;
			}
			else if (1 == cfgValue.m_nComboSel) {
				dwInterval = 60;
			}
			else if (2 == cfgValue.m_nComboSel) {
				dwInterval = 300;
			}
			else if (3 == cfgValue.m_nComboSel) {
				dwInterval = 900;
			}
			else if (4 == cfgValue.m_nComboSel) {
				dwInterval = 1800;
			}
			else {
				dwInterval = 3600;
			}

			bGetCfg = m_tree->GetConfigValue(1, cfgValue);
			if (0 == cfgValue.m_nComboSel) {
				dwMinTemp = 20;
			}
			else if (1 == cfgValue.m_nComboSel) {
				dwMinTemp = 24;
			}
			else if (2 == cfgValue.m_nComboSel) {
				dwMinTemp = 28;
			}
			else {
				dwMinTemp = 32;
			}

			bGetCfg = m_tree->GetConfigValue(2, cfgValue);
			//strText = m_edLowAlarm->GetText();
			strText = cfgValue.m_strEdit;
			if (1 != sscanf_s(strText, "%lf", &dbTemp)) {
				::MessageBox(this->GetHWND(), "低温报警请输入数字", "设置", 0);
				return;
			}
			if ( dbTemp > 42 || dbTemp < 20 ) {
				::MessageBox(this->GetHWND(), "低温报警请输入范围(20~42)", "设置", 0);
				return;
			}
			dwLowAlarm = (DWORD)(dbTemp * 100);

			bGetCfg = m_tree->GetConfigValue(3, cfgValue);
			//strText = m_edHighAlarm->GetText();
			strText = cfgValue.m_strEdit;
			if (1 != sscanf_s(strText, "%lf", &dbTemp)) {
				::MessageBox(this->GetHWND(), "高温报警请输入数字", "设置", 0);
				return;
			}
			if (dbTemp > 42 || dbTemp < 20) {
				::MessageBox(this->GetHWND(), "高温报警请输入范围(20~42)", "设置", 0);
				return;
			}
			if (dbTemp < dwLowAlarm / 100.0) {
				::MessageBox(this->GetHWND(), "高温报警请大于低温报警", "设置", 0);
				return;
			}
			dwHighAlarm = (DWORD)(dbTemp * 100);


			// 串口
			bGetCfg = m_tree->GetConfigValue(4, cfgValue);
			if (0 == cfgValue.m_nComboSel) {
				g_szComPort[0] = '\0';
			}
			else {
				STRNCPY( g_szComPort, m_cmbComPort->GetText(), sizeof(g_szComPort) );
			}

			g_dwCollectInterval = dwInterval;
			g_dwLowTempAlarm = dwLowAlarm;
			g_dwHighTempAlarm = dwHighAlarm;
			g_dwMinTemp = dwMinTemp;

			//strText = m_edAlarmPath->GetText();
			//strncpy_s(g_szAlarmFilePath, strText, sizeof(g_szAlarmFilePath));

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
	//m_edInteval   = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edInteval"));
	//m_edLowAlarm  = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edLowTempAlarm"));
	//m_edHighAlarm = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edHiTempAlarm"));
	//m_edAlarmPath = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edAlarm"));

	DuiLib::CDuiString  strText;
	//strText.Format( "%lu", g_dwCollectInterval );
	//m_edInteval->SetText( strText );

	//strText.Format("%.2f", g_dwLowTempAlarm / 100.0 );
	//m_edLowAlarm->SetText(strText);

	//strText.Format("%.2f", g_dwHighTempAlarm / 100.0 );
	//m_edHighAlarm->SetText(strText);

	//m_edAlarmPath->SetText(g_szAlarmFilePath);

	m_tree = (CMyTreeCfgUI *)m_PaintManager.FindControl("tree1"); 

	CComboUI * pCombo = new CComboUI;

	CListLabelElementUI * pElement = new CListLabelElementUI;
	pElement->SetText("10秒");
	pCombo->Add(pElement);

	pElement = new CListLabelElementUI;
	pElement->SetText("1分钟");
	pCombo->Add(pElement);

	pElement = new CListLabelElementUI;
	pElement->SetText("5分钟");
	pCombo->Add(pElement);

	pElement = new CListLabelElementUI;
	pElement->SetText("15分钟");
	pCombo->Add(pElement);

	pElement = new CListLabelElementUI;
	pElement->SetText("30分钟");
	pCombo->Add(pElement);

	pElement = new CListLabelElementUI;
	pElement->SetText("1小时");
	pCombo->Add(pElement);

	if (10 >= g_dwCollectInterval) {
		pCombo->SelectItem(0);
	}
	else if (60 >= g_dwCollectInterval) {
		pCombo->SelectItem(1);
	}
	else if (300 >= g_dwCollectInterval) {
		pCombo->SelectItem(2);
	}
	else if (900 >= g_dwCollectInterval) {
		pCombo->SelectItem(3);
	}
	else if (1800 >= g_dwCollectInterval) {
		pCombo->SelectItem(4);
	}
	else {
		pCombo->SelectItem(5);
	}
	
	pCombo->SetItemTextColor(0xFF386382);
	pCombo->SetHotItemTextColor(0xFF386382);
	pCombo->SetSelectedItemTextColor(0xFF386382);
	pCombo->SetItemFont(2);

	pCombo->SetAttributeList("normalimage=\"file = 'Combo_nor.bmp' corner = '2,2,24,2'\" hotimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" pushedimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" textpadding=\"5, 1, 5, 1\" ");

	m_tree->AddNode("采样间隔", 0, (void *)(1), pCombo);



	pCombo = new CComboUI;

	pElement = new CListLabelElementUI;
	pElement->SetText("20℃");
	pCombo->Add(pElement);

	pElement = new CListLabelElementUI;
	pElement->SetText("24℃");
	pCombo->Add(pElement);

	pElement = new CListLabelElementUI;
	pElement->SetText("28℃");
	pCombo->Add(pElement);

	pElement = new CListLabelElementUI;
	pElement->SetText("32℃");
	pCombo->Add(pElement);

	if (g_dwMinTemp < 24) {
		pCombo->SelectItem(0);
	}
	else if (g_dwMinTemp < 28) {
		pCombo->SelectItem(1);
	}
	else if (g_dwMinTemp < 32) {
		pCombo->SelectItem(2);
	}
	else {
		pCombo->SelectItem(3);
	}

	pCombo->SetItemTextColor(0xFF386382);
	pCombo->SetHotItemTextColor(0xFF386382);
	pCombo->SetSelectedItemTextColor(0xFF386382);
	pCombo->SetItemFont(2);

	pCombo->SetAttributeList("normalimage=\"file = 'Combo_nor.bmp' corner = '2,2,24,2'\" hotimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" pushedimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" textpadding=\"5, 1, 5, 1\" ");

	m_tree->AddNode("显示的最低温度", 0, (void *)(2), pCombo);

	CEditUI * pEdit = new CEditUI;
	pEdit->SetTextColor(0xFF386382);
	pEdit->SetFont(2);
	strText.Format("%.2f", g_dwLowTempAlarm / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode("低温报警", 0, (void *)(3), pEdit);

	pEdit = new CEditUI;
	pEdit->SetTextColor(0xFF386382);
	pEdit->SetFont(2); 
	strText.Format("%.2f", g_dwHighTempAlarm / 100.0);
	pEdit->SetText(strText);
	m_tree->AddNode("高温报警", 0, (void *)(4), pEdit);   



	// 串口
	char szPort[256];
	g_cfg->GetConfig("com port", szPort, sizeof(szPort), "");

	std::vector<std::string>  vCom;
	GetAllSerialPortName(vCom);

	std::vector<std::string>::iterator it;
	if (szPort[0] != 0) {
		for (it = vCom.begin(); it != vCom.end(); it++) {
			std::string & s = *it;
			if (0 == StrICmp(s.c_str(), szPort)) {
				break;
			}
		}
		if ( it == vCom.end() ) {
			vCom.push_back(szPort);
		}
	}

	
	m_cmbComPort = new CComboUI;
	pCombo = m_cmbComPort;
	pElement = new CListLabelElementUI;
	pElement->SetText("任意端口");
	pCombo->Add(pElement);

	for (it = vCom.begin(); it != vCom.end(); it++) {
		std::string & s = *it;
		pElement = new CListLabelElementUI;
		pElement->SetText(s.c_str());
		pCombo->Add(pElement);
	}

	if (szPort[0] == 0) {
		pCombo->SelectItem(0);
	}
	else {
		for (it = vCom.begin(); it != vCom.end(); it++) {
			std::string & s = *it;
			if ( 0 == StrICmp(s.c_str(), szPort) ) {
				pCombo->SelectItem( (it - vCom.begin()) + 1 );
			}
		}
	}
	
	pCombo->SetItemTextColor(0xFF386382);
	pCombo->SetHotItemTextColor(0xFF386382);
	pCombo->SetSelectedItemTextColor(0xFF386382);
	pCombo->SetItemFont(2);

	pCombo->SetAttributeList("normalimage=\"file = 'Combo_nor.bmp' corner = '2,2,24,2'\" hotimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" pushedimage=\"file = 'Combo_over.bmp' corner = '2,2,24,2'\" textpadding=\"5, 1, 5, 1\" ");

	m_tree->AddNode("读写串口", 0, (void *)(2), pCombo);



	DuiLib::WindowImplBase::InitWindow();
}

LRESULT  CSettingDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DuiLib::WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

DuiLib::CControlUI * CSettingDlg::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyTree")) {
		return new CMyTreeCfgUI();
	}
	return WindowImplBase::CreateControl(pstrClass);
}