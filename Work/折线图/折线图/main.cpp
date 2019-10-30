// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"

#pragma comment(lib, "Shell32.lib") 

CDuiFrameWnd::CDuiFrameWnd() {
	m_cmbComPorts = 0;
	m_btnOpen = 0;
	m_btnBrowse = 0;
	m_bOpend = FALSE;
	m_bBusy = FALSE;
	m_rich = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = m_hWnd;

	m_cmbComPorts = (CComboUI *)m_PaintManager.FindControl("cmComPort");
	m_btnOpen = (CButtonUI *)m_PaintManager.FindControl("btnOpen");
	m_btnBrowse = (CButtonUI *)m_PaintManager.FindControl("btnBrowse");
	m_rich = (CRichEditUI *)m_PaintManager.FindControl("rchData");

	OnDeviceChanged(0, 0); 
	WindowImplBase::InitWindow();    
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString name = msg.pSender->GetName();
	if ( msg.sType == "click" ) {
		if ( name == "btnOpen" ) {
			OnMyOpen();
		}
		else if (name == "btnBrowse") {
			ShellExecute(NULL, "open", "result\\data.html", NULL, NULL, SW_SHOW);
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_OPEN_COM_RET ) {
		BOOL bRet = (BOOL)wParam;
		if ( bRet ) {
			m_bOpend = TRUE;
			m_btnOpen->SetText("关闭");
			m_buf.Clear();
			m_rich->SetText("");
			SetBusy(FALSE);
		}
		else {
			SetBusy(FALSE);
			MessageBox(GetHWND(), "打开串口失败", "系统提示", 0);    
		}
	}
	else if (uMsg == UM_CLOSE_COM_RET) {
		BOOL bRet = (BOOL)wParam;
		if (bRet) {
			m_bOpend = FALSE;
			m_btnOpen->SetText("打开");
			SetBusy(FALSE);
			// 保存数据
			SaveData();
		}
		else {
			SetBusy(FALSE);
			MessageBox(GetHWND(), "关闭串口失败", "系统提示", 0);
		}
	}
	else if (uMsg == UM_READ_COM_RET) {
		BYTE * pData = (BYTE *)wParam;
		DWORD  dwSize = lParam;

		if ( dwSize > 0 )
			m_buf.Append(pData, dwSize);

		if (pData) {
			delete[] pData;
		}

		char buf[8192];
		DWORD dwDataLen = m_buf.GetDataLength();
		if (dwDataLen <= 512) {
			DebugStream(buf, sizeof(buf), m_buf.GetData(), dwDataLen); 
			m_rich->SetText(buf);
		}
		else {
			DebugStream(buf, sizeof(buf), (const BYTE *)m_buf.GetData() + (dwDataLen - 512), 512);
			m_rich->SetText(buf);
		}
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

void  CDuiFrameWnd::OnMyOpen() {
	if (!m_bOpend) {
		int nSel = m_cmbComPorts->GetCurSel();
		if (nSel < 0) {
			MessageBox(GetHWND(), "没有选中串口", "系统提示", 0);
			return;
		}

		int nCom = m_cmbComPorts->GetItemAt(nSel)->GetTag();
		CBusiness::GetInstance()->OpenComPortAsyn(nCom);
		SetBusy(TRUE);
	}
	else {
		CBusiness::GetInstance()->CloseComPortAsyn();
		SetBusy(TRUE);
	}
}

void  CDuiFrameWnd::SetBusy(BOOL bBusy /*= TRUE*/) {
	m_bBusy = bBusy;
	if (m_bBusy) {
		m_cmbComPorts->SetEnabled(false);
		m_btnOpen->SetEnabled(false);
		m_btnBrowse->SetEnabled(false);
	}
	else {
		if ( m_bOpend )
			m_cmbComPorts->SetEnabled(false);
		else
			m_cmbComPorts->SetEnabled(true);

		m_btnOpen->SetEnabled(true);

		if (m_bOpend)
			m_btnBrowse->SetEnabled(false);
		else
			m_btnBrowse->SetEnabled(true);
	}
}

void  CDuiFrameWnd::SaveData() {
	std::vector<DWORD> v1;
	std::vector<DWORD> v2;
	std::vector<DWORD>::iterator it;
	DWORD  wMin = 0xFFFFFFFF;
	int index = 0;
	char buf[256];

	DWORD  dwDataLen = m_buf.GetDataLength();
	DWORD  dwItemCnt = dwDataLen / 5;
	const BYTE * pData = (const BYTE *)m_buf.GetData();
	for ( DWORD i = 0; i < dwItemCnt; i++ ) {
		const BYTE * pItem = pData + 5 * i;
		if ( pItem[0] == 1 ) {
			v1.push_back( (pItem[1] << 24) | (pItem[2] << 16) | (pItem[3] << 8) | pItem[4] );
		}
		else if (pItem[0] == 2) {
			v2.push_back( (pItem[1] << 24) | (pItem[2] << 16) | (pItem[3] << 8) | pItem[4] );
		}
	}

	FILE * fp = fopen("result\\js\\data.js", "wb");
	if (0 == fp) {
		return;
	}

	const char * s = 0;

	s = "var temp_data = \r\n";
	fwrite(s, 1, strlen(s), fp);

	s = "[\r\n";
	fwrite(s, 1, strlen(s), fp);

	s = "\t{\r\n";
	fwrite(s, 1, strlen(s), fp);

	s = "\t\t's1':[\r\n";
	fwrite(s, 1, strlen(s), fp);

	for ( it = v1.begin(), index = 1; it != v1.end(); ++it, index++ ) {
		SNPRINTF(buf, sizeof(buf), "\t\t\t\t[%d,%d],\r\n", index, (int)*it );
		fwrite(buf, 1, strlen(buf), fp);
		if (*it < wMin) {
			wMin = *it;
		}
	}

	s = "\t\t\t],\r\n";
	fwrite(s, 1, strlen(s), fp);

	s = "\r\n";
	fwrite(s, 1, strlen(s), fp);

	s = "\t\t's2':[\r\n";
	fwrite(s, 1, strlen(s), fp);

	for (it = v2.begin(), index = 1; it != v2.end(); ++it, index++) {
		SNPRINTF(buf, sizeof(buf), "\t\t\t\t[%d,%d],\r\n", index, (int)*it);
		fwrite(buf, 1, strlen(buf), fp);
		if (*it < wMin) {
			wMin = *it;
		}
	}

	s = "\t\t\t],\r\n";
	fwrite(s, 1, strlen(s), fp);

	WORD r = wMin % 10;
	if ( r > 0 ) {
		wMin -= r;
	}

	SNPRINTF(buf, sizeof(buf), "\t\tmin:%d,\r\n", (int)wMin );
	fwrite(buf, 1, strlen(buf), fp);

	s = "\t},\r\n";
	fwrite(s, 1, strlen(s), fp);

	s = "];\r\n";
	fwrite(s, 1, strlen(s), fp);

	fclose(fp);
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


