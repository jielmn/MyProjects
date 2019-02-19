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

CDuiFrameWnd::CDuiFrameWnd() {
	m_bBusy = FALSE;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	m_cmbComPorts = (CComboUI *)m_PaintManager.FindControl("cmComPort");
	m_edTemp = (CEditUI *)m_PaintManager.FindControl("edTemp");

	OnDeviceChanged(0,0);
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == "click") {
		if (name == "btnSend") {
			OnBtnSend();
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
			m_cmbComPorts->SelectItem(nFindeIndex);
		}
		else {
			m_cmbComPorts->SelectItem(0);
		}
	}
}

void  CDuiFrameWnd::OnBtnSend() {
	int nSel = m_cmbComPorts->GetCurSel();
	if (nSel < 0) {
		MessageBox(GetHWND(), "没有选中串口", "系统提示", 0);
		return;
	}

	CDuiString strTemp = m_edTemp->GetText();
	if (strTemp.GetLength() == 0) {
		MessageBox(GetHWND(), "请输入温度", "系统提示", 0);
		return;
	}

	float fTemp = 0.0f;
	int ret  = sscanf(strTemp, "%f", &fTemp);
	if (0 == ret) {
		MessageBox(GetHWND(), "温度数据不是数字", "系统提示", 0);
		return;
	}

	if ( fTemp > 99.9999f || fTemp < 0.000001f ) {
		MessageBox(GetHWND(), "温度数据超出范围", "系统提示", 0);
		return;
	}

	DWORD  dwTemp = (DWORD)(fTemp * 100);


	CLmnSerialPort  serial_port;

	char  szComPort[32];
	char  write_data[256];

	CListLabelElementUI * pItem =  (CListLabelElementUI *)m_cmbComPorts->GetItemAt(nSel);
	int nPort = pItem->GetTag();

	SNPRINTF(szComPort, sizeof(szComPort), "com%d", nPort);
	BOOL bRet = serial_port.OpenUartPort(szComPort);
	if (!bRet) {
		MessageBox(GetHWND(), "打开串口失败", "系统提示", 0);
		return;
	}

	const char * pData = "\xFF\xFF\x01\x55\x1A\x00\x05\x04\x45\x52\x00\x00\x03\x00\x00\x00\x00\x00\x01\x59\x3F\xD4\x93\xDD\xCD\x59\x02\xE0\x03\x00\x02\x03\xFF\xAA";
	DWORD  dwDataLen = 34;
	memcpy(write_data, pData, dwDataLen);

	DWORD  d = dwTemp;

	write_data[27] = (char)(d / 1000);
	d = d % 1000;
	write_data[28] = (char)(d / 100);
	d = d % 100;
	write_data[29] = (char)(d / 10);
	d = d % 10;
	write_data[30] = (char)d;
	serial_port.Write(write_data, dwDataLen);

	serial_port.CloseUartPort();
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


