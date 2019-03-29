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
	m_tabs = static_cast<DuiLib::CTabLayoutUI*>(m_PaintManager.FindControl("switch"));
	m_cmbHandReaderCom = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbComPort_1"));
	m_cmbSurgencyReaderCom = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbComPort_2"));
	m_cmbHandReaderAddr = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbAddr_1"));
	m_cmbHandReaderChannel = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbChannel_1"));
	m_btnHandReader = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSetting_1"));

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
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_DEVICECHANGE) {
		OnDeviceChanged(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}
             
void  CDuiFrameWnd::OnDeviceChanged(WPARAM wParm, LPARAM  lParam) {
	std::vector<std::string> vComPorts;
	EnumPortsWdm(vComPorts);

	SetComboCom(m_cmbHandReaderCom, vComPorts);
	SetComboCom(m_cmbSurgencyReaderCom, vComPorts);
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


