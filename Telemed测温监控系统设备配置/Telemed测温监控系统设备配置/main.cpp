// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"

CDuiFrameWnd::CDuiFrameWnd() {

}   

CDuiFrameWnd::~CDuiFrameWnd() {
	  
}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = m_hWnd;

	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("switch"));

	// tab 1
	m_cmbHandReaderCom = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbComPort_1"));
	m_cmbHandReaderAddr = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbAddr_1"));
	m_cmbHandReaderChannel = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbChannel_1"));
	m_btnHandReader_1 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_1"));
	m_btnHandReader_2 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_2"));
	m_edHandReaderSn = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtSN_1"));

	// tab 2
	m_cmbSurgencyReaderCom = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbComPort_2"));
	m_cmbArea_1 = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbArea_1"));
	m_cmbBed_1 = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbBed_1"));
	m_cmbBed_2 = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbBed_2"));
	m_edSurgencyReaderSn = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edtSN_2"));
	m_btnSurgencyReader_1 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_3"));
	m_btnSurgencyReader_2 = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_4"));

	// tab 3
	m_cmbReceiverCom = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbComPort_3"));

	// progress
	m_progress = static_cast<CMyProgress*>(m_PaintManager.FindControl("progress"));

	CDuiString  strText;

#ifndef RESTRICTED_FLAG
	for ( int i = 1; i <= 255; i++ ) {
		strText.Format("%d", i);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(i);
		m_cmbHandReaderAddr->Add(pElement); 
	}

	for (int i = 1; i <= 91; i++ ) {
		strText.Format("%d", i);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(i);
		m_cmbHandReaderChannel->Add(pElement);
	}

#else
	for (int i = 1; i <= 10; i++) {
		strText.Format("%d", i);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(i);
		m_cmbHandReaderAddr->Add(pElement);
	}

	for (int i = 1; i <= 91; i += 10 ) {
		strText.Format("%d", i);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(i);
		m_cmbHandReaderChannel->Add(pElement);
	}
#endif

	m_cmbHandReaderAddr->SelectItem(0);  
	m_cmbHandReaderChannel->SelectItem(0);     

	for (int i = 0; i < MAX_SURGENCY_BEDS_CNT; i++) {
		strText.Format("%d", i+1);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(i);
		m_cmbBed_1->Add(pElement);		
	}
	m_cmbBed_1->SelectItem(0);

	for ( int i = 0; i < MAX_READERS_PER_BED; i++ ) {
		strText.Format("%c", 'A'+i);
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(strText);
		pElement->SetTag(i);
		m_cmbBed_2->Add(pElement);
	}
	m_cmbBed_2->SelectItem(0);

	OnDeviceChanged(0, 0);  
	WindowImplBase::InitWindow();     
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp("reader_setting", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("reader_setting.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("reader_setting_1", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("reader_setting_1.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI; 
	}
	else if (0 == strcmp("gw_setting", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("gw_setting.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;
	}
	else if (0 == strcmp("area_setting", pstrClass)) {
		DuiLib::CDialogBuilder builder;
		DuiLib::CControlUI * pUI = builder.Create(_T("area_setting.xml"), (UINT)0, 0, &m_PaintManager);
		return pUI;       
	}
	else if (0 == strcmp(pstrClass, "MyProgress")) {
		return new CMyProgress(&m_PaintManager, "progress_fore.png");
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	DuiLib::CDuiString name = msg.pSender->GetName();

	if (msg.sType == _T("selectchanged"))
	{
		if (name == _T("opn_reader_setting")) {
			m_tabs->SelectItem(0);
		} 
		else if (name == _T("opn_reader_setting_1")) {
			m_tabs->SelectItem(1);
		}
		else if (name == _T("opn_gw_setting")) {
			m_tabs->SelectItem(2);
		}
		else if (name == _T("opn_area_setting")) {
			m_tabs->SelectItem(3);
		}
	}
	else if (msg.sType == "click") {
		// 设置手持Reader
		if (name == "btnSetting_1") {
			OnSettingHandReader();
		}
		// 设置手持SN
		else if (name == "btnSetting_2") {
			OnSetHandReaderSn();
		}
		else if (name == "btnSetting_3") {

		}
		else if (name == "btnSetting_4") {
			  
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_DEVICECHANGE) {
		OnDeviceChanged(wParam, lParam);
	}
	else if (uMsg == UM_SETTING_HAND_READER_RET) {
		OnHandReaderSettingRet(wParam, lParam);
	}
	else if (uMsg == UM_SET_HAND_READER_SN_RET) {
		OnSetHandReaderSnRet(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}
             
void  CDuiFrameWnd::OnDeviceChanged(WPARAM wParm, LPARAM  lParam) {
	std::vector<std::string> vComPorts;
	EnumPortsWdm(vComPorts);

	SetComboCom(m_cmbHandReaderCom, vComPorts);
	SetComboCom(m_cmbSurgencyReaderCom, vComPorts);
	SetComboCom(m_cmbReceiverCom, vComPorts);                           	
}

void  CDuiFrameWnd::SetBusy(BOOL bBusy /*= TRUE*/) {
	if (bBusy) {
		m_btnHandReader_1->SetEnabled(false);
		m_btnHandReader_2->SetEnabled(false);
		m_btnSurgencyReader_1->SetEnabled(false);
		m_btnSurgencyReader_2->SetEnabled(false);
		m_progress->SetVisible(true);
		m_progress->Start();
	}
	else {
		m_btnHandReader_1->SetEnabled(true);
		m_btnHandReader_2->SetEnabled(true);		
		m_btnSurgencyReader_1->SetEnabled(true);
		m_btnSurgencyReader_2->SetEnabled(true);
		m_progress->Stop();
		m_progress->SetVisible(false);
	}
}

void  CDuiFrameWnd::OnSettingHandReader() {
	int nSel = m_cmbHandReaderCom->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}

	CControlUI * pCtl = m_cmbHandReaderCom->GetItemAt(nSel);
	int nCom = (int)pCtl->GetTag();

	nSel = m_cmbHandReaderChannel->GetCurSel();
	pCtl = m_cmbHandReaderChannel->GetItemAt(nSel);
	BYTE byChannel = (BYTE)pCtl->GetTag();

	nSel = m_cmbHandReaderAddr->GetCurSel();
	pCtl = m_cmbHandReaderAddr->GetItemAt(nSel);
	BYTE byAddr = (BYTE)pCtl->GetTag();

	CBusiness::GetInstance()->SetHandReaderAsyn(byChannel, byAddr, nCom);
	SetBusy();
}

void  CDuiFrameWnd::OnHandReaderSettingRet(WPARAM wParm, LPARAM  lParam) {
	SetBusy(FALSE);

	if (0 == wParm) {
		MessageBox(GetHWND(), "设置成功", "设置", 0);
	}
	else {
		MessageBox(GetHWND(), "设置失败", "设置", 0);
	}
}

void  CDuiFrameWnd::OnSetHandReaderSn() {
	int nSel = m_cmbHandReaderCom->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中串口", "错误", 0);
		return;
	}
	CControlUI * pCtl = m_cmbHandReaderCom->GetItemAt(nSel);
	int nCom = (int)pCtl->GetTag();

	CDuiString strText = m_edHandReaderSn->GetText();
	if (strText.GetLength() == 0) {
		MessageBox(GetHWND(), "请输入SN", "错误", 0);
		return;
	}

	DWORD  dwSn = 0;
	int ret = sscanf(strText, "%lu", &dwSn);
	if (0 == ret) {
		MessageBox(GetHWND(), "SN必须为数字", "错误", 0);
		return;
	}
	
	CBusiness::GetInstance()->SetHandReaderSnAsyn(dwSn, nCom);
	SetBusy();
}

void  CDuiFrameWnd::OnSetHandReaderSnRet(WPARAM wParm, LPARAM  lParam) {
	SetBusy(FALSE);

	if (0 == wParm) {
		MessageBox(GetHWND(), "设置SN成功", "设置", 0);
	}
	else {
		MessageBox(GetHWND(), "设置SN失败", "设置", 0);
	}
}








int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

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

	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main close.\n");

	CBusiness::GetInstance()->DeInit();
	delete CBusiness::GetInstance();
	LmnToolkits::ThreadManager::ReleaseInstance();

	return 0;
}


