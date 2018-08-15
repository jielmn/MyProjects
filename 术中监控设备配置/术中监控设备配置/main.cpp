// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include <locale.h>

CDuiFrameWnd::CDuiFrameWnd() {
	m_bBusy = FALSE;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_hWnd = GetHWND();
	this->PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("switch"));
	m_reader_com_ports = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbReaderComPort"));
	m_gw_com_ports = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmGwComPort"));

	m_edtArea1 = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtAreaNo1"));
	m_edtBedNo = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtBedNo"));
	m_edtArea2 = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtAreaNo2"));

	m_progress = (CMyProgress *)m_PaintManager.FindControl("progress");
	m_btnSetting1 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting1"));
	m_btnSetting2 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting2"));
	m_btnQuery2 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnQuery2"));

	m_edtQuery2Ret = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtAreaNo_Ret2"));

	OnFreshComPort_Reader();
	OnFreshComPort_Gw();

	WindowImplBase::InitWindow();
} 
   
CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp("reader_setting", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("reader_setting.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("gw_setting", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("gw_setting.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp(pstrClass, "MyProgress")) {
		return new CMyProgress(&m_PaintManager, "progress_fore.png");
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();
	DuiLib::CDuiString strText;

	if (msg.sType == _T("selectchanged"))
	{
		if (0 == m_tabs) {
			return;
		}

		if (name == _T("opn_reader_setting")) {
			m_tabs->SelectItem(0);
		}
		else if (name == _T("opn_gw_setting")) {
			m_tabs->SelectItem(1);
		}
	}
	else if ( msg.sType == "click" ) {
		if (name == "btnFresh1") {
			OnFreshComPort_Reader();
		}
		else if (name == "btnFresh2") {
			OnFreshComPort_Gw();
		}
		else if (name == "btnSetting1") {
			OnSettingReader();
		}
		else if (name == "btnSetting2" ) {
			OnSettingGw();
		}
		else if (name == "btnQuery2") {
			OnQueryGw();
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == UM_SETTTING_READER_RET) {
		OnSettingReaderRet(wParam, lParam);
	}
	else if (uMsg == WM_DEVICECHANGE) {
		OnDeviceChanged(wParam, lParam);
	}
	else if (uMsg == UM_SETTTING_GW_RET) {
		OnSettingGwRet(wParam, lParam);
	}
	else if (uMsg == UM_QUERY_GW_RET) {
		OnQueryGwRet(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}
                  
void  CDuiFrameWnd::OnFreshComPort_Reader() {
	m_reader_com_ports->RemoveAll();

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
		m_reader_com_ports->Add(pElement);

		int nComPort = 0;
		const char * pFind = strstr(s.c_str(), "COM");
		while ( pFind ) {
			if ( 1 == sscanf(pFind + 3, "%d", &nComPort) ) {
				pElement->SetTag(nComPort); 
				break;
			}
			pFind = strstr(pFind + 3, "COM");
		}

		if (!bFindReader) {
			char tmp[256];
			Str2Lower(s.c_str(), tmp, sizeof(tmp) );
			if ( 0 != strstr(tmp, "usb-serial ch340") ) {
				bFindReader = TRUE;
				nFindeIndex = i;
			}
		}
	}

	if (m_reader_com_ports->GetCount() > 0) {
		if (nFindeIndex >= 0) {
			m_reader_com_ports->SelectItem(nFindeIndex);
		}
		else {
			m_reader_com_ports->SelectItem(0);
		}
		
	}
}   

void  CDuiFrameWnd::OnFreshComPort_Gw() {
	m_gw_com_ports->RemoveAll();

	std::vector<std::string> vComPorts;
	EnumPortsWdm(vComPorts);

	BOOL  bFindReader = FALSE;
	int   nFindeIndex = -1;
	int   i = 0;
	std::vector<std::string>::iterator it;
	for (it = vComPorts.begin(); it != vComPorts.end(); it++, i++) {
		std::string & s = *it;

		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(s.c_str());
		m_gw_com_ports->Add(pElement);

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

	if (m_gw_com_ports->GetCount() > 0) {
		if (nFindeIndex >= 0) {
			m_gw_com_ports->SelectItem(nFindeIndex);
		}
		else {
			m_gw_com_ports->SelectItem(0);
		}
	}
}

void  CDuiFrameWnd::OnSettingReader() {
	if (m_bBusy) {
		MessageBox(GetHWND(), "正忙着配置或查询，请稍候", "忙碌", 0);
		return;
	}

	int nAreaNo = 0;
	int nBedNo = 0;
	int nPort = 0;
	DuiLib::CDuiString  strText;

	if (m_reader_com_ports->GetCurSel() < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	nPort = m_reader_com_ports->GetItemAt(m_reader_com_ports->GetCurSel())->GetTag();

	strText = m_edtArea1->GetText();
	if (1 != sscanf(strText, "%d", &nAreaNo)) {
		MessageBox(GetHWND(), "请输入病区号", "错误", 0);
		return;
	}

	if (nAreaNo <= 0) {
		MessageBox(GetHWND(), "病区号必须是正整数", "错误", 0);
		return;
	}

	if (nAreaNo > 100) {
		MessageBox(GetHWND(), "病区号的范围是1到100", "错误", 0);
		return;
	}

	strText = m_edtBedNo->GetText();
	if (1 != sscanf(strText, "%d", &nBedNo)) {
		MessageBox(GetHWND(), "请输入床位号", "错误", 0);
		return;
	}

	if (nBedNo <= 0) {
		MessageBox(GetHWND(), "床位号必须是正整数", "错误", 0);
		return;
	}

	if (nBedNo > 200) {
		MessageBox(GetHWND(), "床号的范围是1到200", "错误", 0);
		return;
	}

	CBusiness::GetInstance()->SettingReaderAsyn(nAreaNo, nBedNo, nPort);

	m_btnSetting1->SetEnabled(false);
	SetBusy(TRUE);
}

void  CDuiFrameWnd::OnSettingGw() {
	if (m_bBusy) {
		MessageBox(GetHWND(), "正忙着配置或查询，请稍候", "忙碌", 0);
		return;
	}

	int nAreaNo = 0;
	int nPort = 0;
	DuiLib::CDuiString  strText;

	if (m_gw_com_ports->GetCurSel() < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	nPort = m_gw_com_ports->GetItemAt(m_gw_com_ports->GetCurSel())->GetTag();

	strText = m_edtArea2->GetText();
	if (1 != sscanf(strText, "%d", &nAreaNo)) {
		MessageBox(GetHWND(), "请输入病区号", "错误", 0);
		return;
	}

	if (nAreaNo <= 0) {
		MessageBox(GetHWND(), "病区号必须是正整数", "错误", 0);
		return;
	}

	if (nAreaNo > 100) {
		MessageBox(GetHWND(), "病区号的范围是1到100", "错误", 0);
		return;
	}

	CBusiness::GetInstance()->SettingGwAsyn(nAreaNo, nPort);

	m_btnSetting2->SetEnabled(false);
	SetBusy(TRUE);
}
 
void  CDuiFrameWnd::SetBusy(BOOL bBusy) {
	if (bBusy) {
		m_progress->SetVisible(true);
		m_progress->Start();
		m_bBusy = TRUE;
	}
	else {
		m_progress->Stop();
		m_progress->SetVisible(false);
		m_bBusy = FALSE;
	}
}

void  CDuiFrameWnd::OnQueryGw() {
	if (m_bBusy) {
		MessageBox(GetHWND(), "正忙着配置或查询，请稍候", "忙碌", 0);
		return;
	}

	int nPort = 0;
	if (m_gw_com_ports->GetCurSel() < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	nPort = m_gw_com_ports->GetItemAt(m_gw_com_ports->GetCurSel())->GetTag();

	CBusiness::GetInstance()->QueryGwAsyn(nPort);

	m_edtQuery2Ret->SetText("");
	m_btnQuery2->SetEnabled(false);
	SetBusy(TRUE);
}

void  CDuiFrameWnd::OnSettingReaderRet(WPARAM wParm, LPARAM  lParam) {
	int ret = (int)wParm;
	SetBusy(FALSE);
	m_btnSetting1->SetEnabled(true);

	if (ret != 0) {
		MessageBox(GetHWND(), "设置Reader的过程中出错!", "错误", 0);
		return;
	}
}
      
void  CDuiFrameWnd::OnSettingGwRet(WPARAM wParm, LPARAM  lParam) {
	int ret = (int)wParm;
	SetBusy(FALSE);
	m_btnSetting2->SetEnabled(true);

	if (ret != 0) {
		MessageBox(GetHWND(), "设置发射器的过程中出错!", "错误", 0);
		return;
	}
	else {
		MessageBox(GetHWND(), "设置发射器成功", "成功", 0);
	}
}

void  CDuiFrameWnd::OnQueryGwRet(WPARAM wParm, LPARAM  lParam) {
	int ret = (int)wParm;
	SetBusy(FALSE);
	m_btnQuery2->SetEnabled(true);

	if (ret != 0) {
		MessageBox(GetHWND(), "获取发射器房间号的过程中出错!", "错误", 0);
		return;
	}
	else {
		DuiLib::CDuiString strText;
		strText.Format("%d", (int)lParam);
		m_edtQuery2Ret->SetText(strText);
		MessageBox(GetHWND(), "获取发射器房间号成功", "成功", 0);
	}
}

void  CDuiFrameWnd::OnDeviceChanged(WPARAM wParm, LPARAM  lParam) {
	if (m_bBusy) {
		return;
	}

	OnFreshComPort_Reader();
	OnFreshComPort_Gw();
}

                         
    
        
    


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{	
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

	CPaintManagerUI::SetInstance(hInstance);
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	CDuiFrameWnd * duiFrame = new CDuiFrameWnd;
	duiFrame->Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	duiFrame->CenterWindow();

	// 设置icon
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(duiFrame->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	duiFrame->ShowModal();
	delete duiFrame;

	g_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}
       

