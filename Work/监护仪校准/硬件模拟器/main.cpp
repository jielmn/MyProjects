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
	m_bOpened = FALSE;
}

CDuiFrameWnd::~CDuiFrameWnd() {
	        
}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = m_hWnd;

	m_cmbComPorts = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmComPort"));
	m_btnComPort = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnComPort"));
	m_btnComPort->SetText("打开");
	
	CheckDevice();
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	if (msg.sType == "click") {
		if ( msg.pSender->GetName() == "btnComPort" ) {
			// 开始打开
			if ( !m_bOpened ) {
				int nSel = m_cmbComPorts->GetCurSel();
				if (nSel < 0)
					return;
				int nCom = m_cmbComPorts->GetItemAt(nSel)->GetTag();
				CBusiness::GetInstance()->StartAsyn(nCom);
			}
			else {
				CBusiness::GetInstance()->StopAsyn();
			}
			m_cmbComPorts->SetEnabled(false);
			m_btnComPort->SetEnabled(false);
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == UM_START_RET) {
		m_bOpened = TRUE;
		m_btnComPort->SetText("关闭");
		m_btnComPort->SetEnabled(true);
	}
	else if (uMsg == UM_STOP_RET) {
		m_bOpened = FALSE;
		m_btnComPort->SetText("打开");
		m_btnComPort->SetEnabled(true);
		m_cmbComPorts->SetEnabled(true);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

// 检查硬件(串口)
void  CDuiFrameWnd::CheckDevice() {
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

	if (m_cmbComPorts->GetCount() > 0) {
		if (nFindeIndex >= 0) {
			m_cmbComPorts->SelectItem(nFindeIndex, false, false);
		}
		else {
			m_cmbComPorts->SelectItem(0, false, false);
		}
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


