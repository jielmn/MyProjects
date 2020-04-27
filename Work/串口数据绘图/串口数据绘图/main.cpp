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
	m_cmbBaud = 0;
	m_chart = 0;
	m_bStartPaint = FALSE;
	m_bBusy = FALSE;
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
	m_btnPaint    = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnPaint"));
	m_cmbBaud     = static_cast<CComboUI *>(m_PaintManager.FindControl("cmBaud"));

	int arrBauds[5] = { 9600,19200, 38400,57600,115200 };
	CDuiString  strText;
	for (int i = 0; i < 5; i++) {
		CListLabelElementUI * pElement = new CListLabelElementUI;
		m_cmbBaud->Add(pElement);
		strText.Format("%d", arrBauds[i]);
		pElement->SetText(strText);
		pElement->SetTag(arrBauds[i]);
	}
	m_cmbBaud->SelectItem(0);

	InitCmbLuaFiles();
	OnDeviceChanged();

	m_chart->AddData(0, 100);
	m_chart->AddData(0, 200);
	m_chart->AddData(0, 250);
	m_chart->AddData(0, 300);

	for (int i = 0; i < 100; i++) {
		m_chart->AddData(4, 200);
		m_chart->AddData(4, 300);
		m_chart->AddData(4, 450);
		m_chart->AddData(4, 2500);
	}
	

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
	CDuiString  strName = msg.pSender->GetName();

	if (msg.sType == "hslider_changed") {
		//JTelSvrPrint("hslider_changed: %d, %d", (int)msg.wParam, (int)msg.lParam);
		m_chart->SetPosAndLen(msg.wParam / 1000.0f, msg.lParam / 1000.0f);
	}
	else if (msg.sType == "vslider_changed") {
		//JTelSvrPrint("vslider_changed: %d, %d", (int)msg.wParam, (int)msg.lParam);
		m_chart->SetPosAndLen(msg.wParam / 1000.0f, msg.lParam / 1000.0f, FALSE);
	}
	else if ( msg.sType == "click" ) {
		if (strName == "btnPaint") {
			OnStartPaint();
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_OPEN_COM_RET ) {
		OnOpenComRet(wParam);
	}
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

void  CDuiFrameWnd::OnStartPaint() {
	if ( !m_bStartPaint ) {
		int nSel = m_cmbComPorts->GetCurSel();
		if ( nSel < 0 ) {
			MessageBox(GetHWND(), "没有选择串口", "错误", 0);
			return;
		}

		int nCom = m_cmbComPorts->GetItemAt(nSel)->GetTag();
		nSel = m_cmbBaud->GetCurSel();
		int nBaud = m_cmbBaud->GetItemAt(nSel)->GetTag();

		nSel = m_cmbLuaFiles->GetCurSel();
		if (nSel < 0) {
			MessageBox(GetHWND(), "没有选择lua文件", "错误", 0);
			return;
		}

		CBusiness::GetInstance()->OpenComAsyn(nCom, nBaud);

		m_btnPaint->SetText("停止绘图");
		m_bStartPaint = TRUE;
		SetBusy();
	}
	else {
		m_btnPaint->SetText("开始绘图");
		m_bStartPaint = FALSE;  
		SetBusy();
	}
}

void  CDuiFrameWnd::SetBusy(BOOL bBusy /*= TRUE*/) {
	m_bBusy = bBusy;
	if ( m_bBusy ) {
		m_btnPaint->SetEnabled(false);
		m_cmbComPorts->SetEnabled(false);
		m_cmbBaud->SetEnabled(false);
		m_cmbLuaFiles->SetEnabled(false);
	}
	else {
		m_btnPaint->SetEnabled(true);
		m_cmbComPorts->SetEnabled(true);
		m_cmbBaud->SetEnabled(true);
		m_cmbLuaFiles->SetEnabled(true);
	}
}

void  CDuiFrameWnd::OnOpenComRet(BOOL bRet) {
	if ( bRet ) {
		SetBusy(FALSE);
	}
	else {
		MessageBox(GetHWND(), "打开串口失败", "错误", 0);
		m_btnPaint->SetText("开始绘图");
		m_bStartPaint = FALSE;
		SetBusy(FALSE);
	}
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


