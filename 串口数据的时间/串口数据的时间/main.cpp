// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "ComDataUI.h"
#include <time.h>

CDuiFrameWnd::CDuiFrameWnd() {
	m_bComOpened = FALSE;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = GetHWND();

	m_cmbCom = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbComPort"));
	m_btnHandle = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnHandle"));
	m_cmbBaud = static_cast<DuiLib::CComboUI*>(m_PaintManager.FindControl("cmbBaud"));
	m_progress = static_cast<CMyProgress *>(m_PaintManager.FindControl("progress"));
	m_layMain = static_cast<DuiLib::CVerticalLayoutUI *>(m_PaintManager.FindControl("layMain"));

	m_btnHandle->SetText("打开");
	m_bComOpened = FALSE;

	CListLabelElementUI * pElement = 0;
	pElement = new CListLabelElementUI;
	pElement->SetText("9600波特率");
	pElement->SetTag(9600);
	m_cmbBaud->Add(pElement);

	pElement = new CListLabelElementUI;
	pElement->SetText("19200波特率");
	pElement->SetTag(19200);
	m_cmbBaud->Add(pElement); 

	pElement = new CListLabelElementUI;
	pElement->SetText("38400波特率");
	pElement->SetTag(38400);
	m_cmbBaud->Add(pElement);

	pElement = new CListLabelElementUI;
	pElement->SetText("57600波特率");
	pElement->SetTag(57600);
	m_cmbBaud->Add(pElement);

	pElement = new CListLabelElementUI;
	pElement->SetText("115200波特率"); 
	pElement->SetTag(115200);
	m_cmbBaud->Add(pElement);
	m_cmbBaud->SelectItem(0);

	OnDeviceChanged(0,0);   
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	if (0 == strcmp(pstrClass, "MyProgress")) {
		return new CMyProgress(&m_PaintManager, "progress_fore.png");
	}
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString  name = msg.pSender->GetName();
	if ( msg.sType == "click" ) {
		if ( name == "btnHandle" ) {
			OnBtnHandle();
		}
	}
	WindowImplBase::Notify(msg); 
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_DEVICECHANGE) {
		OnDeviceChanged(wParam, lParam);
	}
	else if (uMsg == UM_OPEN_COM_PORT_RET) {
		OnOpenComRet(wParam, lParam);
	}
	else if (uMsg == UM_CLOSE_COM_PORT_RET) {
		OnCloseComRet(wParam, lParam);
	}
	else if (uMsg == UM_GET_COM_DATA_RET) {
		OnComData(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}
  
void  CDuiFrameWnd::OnDeviceChanged(WPARAM wParm, LPARAM  lParam) {
	std::vector<std::string> vComPorts;
	EnumPortsWdm(vComPorts);

	SetComboCom(m_cmbCom, vComPorts); 
}

void  CDuiFrameWnd::OnBtnHandle() {
	// 如果没有打开
	if (!m_bComOpened) {
		int nSel = m_cmbCom->GetCurSel();
		if (nSel < 0) {
			MessageBox(GetHWND(), "没有选中串口", "错误", 0);
			return;
		}

		int nCom = m_cmbCom->GetItemAt(nSel)->GetTag();
		nSel = m_cmbBaud->GetCurSel();
		int nBaud = m_cmbBaud->GetItemAt(nSel)->GetTag();

		CBusiness::GetInstance()->OpenComPortAsyn(nCom, nBaud);
		m_btnHandle->SetEnabled(false);
		m_cmbCom->SetEnabled(false);
		m_cmbBaud->SetEnabled(false);
		m_progress->SetVisible(true);
		m_progress->Start();		
	}
	// 已经打开
	else {
		CBusiness::GetInstance()->CloseComPortAsyn();
		m_btnHandle->SetEnabled(false);
		m_cmbCom->SetEnabled(false);
		m_cmbBaud->SetEnabled(false);
		m_progress->SetVisible(true);
		m_progress->Start();
	}
}

void  CDuiFrameWnd::OnOpenComRet(WPARAM wParm, LPARAM  lParam) {
	m_progress->Stop();
	m_progress->SetVisible(false);
	m_btnHandle->SetEnabled(true);

	int nRet = wParm;
	if (0 == nRet) {
		m_btnHandle->SetText("关闭");
		m_bComOpened = TRUE;
	}
	else {
		MessageBox(GetHWND(), "打开串口失败", "错误", 0);
	}
}

void  CDuiFrameWnd::OnCloseComRet(WPARAM wParm, LPARAM  lParam) {
	m_progress->Stop();
	m_progress->SetVisible(false);
	m_btnHandle->SetEnabled(true);
	m_cmbCom->SetEnabled(true);
	m_cmbBaud->SetEnabled(true);

	int nRet = wParm;
	if (0 == nRet) {
		m_btnHandle->SetText("打开");
		m_bComOpened = FALSE;
	}
	else {
		MessageBox(GetHWND(), "关闭串口失败", "错误", 0);
	}
}

void  CDuiFrameWnd::OnComData(WPARAM wParm, LPARAM  lParam) {
	char * pData = (char *)wParm;
	DWORD  dwDataLen = lParam;
	
	char buf[8192];
	DebugStream(buf, sizeof(buf), pData, dwDataLen);

	char szTime[256];
	time_t now = time(0);
	Date2String_1(szTime, sizeof(szTime), &now);

	CComDataUI * pItem = new CComDataUI;
	pItem->SetDataText(buf);
	pItem->SetTimeText(szTime);
	DWORD  dwLineCnt = (dwDataLen - 1) / 16 + 1;
	pItem->SetFixedHeight(dwLineCnt * 20 + 20);     
	//pItem->SetFixedWidth(880);
	 
	int nCnt = m_layMain->GetCount();
	if (nCnt >= g_data.m_nMaxItemsCnt) {
		m_layMain->RemoveAt(0);       
	}
	m_layMain->Add(pItem);  

	if ( pData )
		delete[] pData;
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


