// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "LmnSerialPort.h"
#include "LmnTelSvr.h"

CDuiFrameWnd::CDuiFrameWnd() {
	m_cmbComPorts = 0;
	m_cmbLuaFiles = 0;
	m_chart = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = GetHWND();
	g_data.m_cursor_we = LoadCursor(NULL, IDC_SIZEWE);
	g_data.m_cursor_ns = LoadCursor(NULL, IDC_SIZENS);

	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_cmbComPorts = (CComboUI *)m_PaintManager.FindControl("cmComPort");
	m_cmbLuaFiles = static_cast<CComboUI *>(m_PaintManager.FindControl("cmLuaFile"));
	m_chart       = static_cast<CMyChartUI *>(m_PaintManager.FindControl("cstChart"));

	InitCmbLuaFiles();
	OnDeviceChanged();

	m_chart->AddData(0, 100);
	m_chart->AddData(0, 200);
	m_chart->AddData(0, 250);
	m_chart->AddData(0, 300);

	m_chart->AddData(1, 200);
	m_chart->AddData(1, 300);
	m_chart->AddData(1, 450);
	m_chart->AddData(1, 2500);

	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyChart")) {
		return new CMyChartUI;
	}
	else if (0 == strcmp(pstrClass, "MySlider")) {
		return new CMySliderUI;           
	}
	else if (0 == strcmp(pstrClass, "MyVSlider")) {
		return new CMyVSliderUI;  
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	if (msg.sType == "hslider_changed") {
		// JTelSvrPrint("hslider_changed: %d, %d", (int)msg.wParam, (int)msg.lParam);
	}
	else if (msg.sType == "vslider_changed") {
		// JTelSvrPrint("vslider_changed: %d, %d", (int)msg.wParam, (int)msg.lParam);
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void  CDuiFrameWnd::OnDeviceChanged() {
	m_cmbComPorts->RemoveAll();

	std::map<int, std::string> m;
	GetAllSerialPorts(m);

	BOOL  bFindReader = FALSE;
	int   nFindeIndex = -1;

	std::map<int, std::string>::iterator it;
	int i = 0;

	for (it = m.begin(); it != m.end(); it++, i++) {
		std::string & s = it->second;

		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(s.c_str());
		m_cmbComPorts->Add(pElement);

		int nComPort = it->first;
		pElement->SetTag(nComPort);

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
			m_cmbComPorts->SelectItem(nFindeIndex);
		}
		else {
			m_cmbComPorts->SelectItem(0);
		}
	}
}

void  CDuiFrameWnd::InitCmbLuaFiles() {
	WIN32_FIND_DATA FindData;
	HANDLE hFind;

	char szFilePathName[256];
	SNPRINTF(szFilePathName, sizeof(szFilePathName), ".\\Lua\\*.lua");

	hFind = FindFirstFile(szFilePathName, &FindData);
	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}

	do {
		CListLabelElementUI * pElement = new CListLabelElementUI;
		pElement->SetText(FindData.cFileName);
		m_cmbLuaFiles->Add(pElement);

		if (!::FindNextFile(hFind, &FindData)) {
			break;
		}
	} while (TRUE);

	FindClose(hFind);

	if (m_cmbLuaFiles->GetCount() > 0)
		m_cmbLuaFiles->SelectItem(0);
}






int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	DWORD  dwPort = 2020;
	JTelSvrStart((unsigned short)dwPort, 3);

	LmnToolkits::ThreadManager::GetInstance();
	CBusiness::GetInstance()->Init();
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "main begin.\n");

	CoInitialize(NULL);

	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

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


