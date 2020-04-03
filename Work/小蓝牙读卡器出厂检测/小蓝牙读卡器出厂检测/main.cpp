// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "UIMenu.h"
#include "LmnSerialPort.h"

#include "business.h"
#include "resource.h"

                                                             
CDuiFrameWnd::CDuiFrameWnd() {
	m_bComConnected = FALSE;
	m_bAutoTestStarted = FALSE;
	m_bAutoTestFinished = FALSE;
	m_bStopAutoTestFinished = FALSE;
	m_bBleConnected = FALSE;

	m_lblStatus   = 0;
	m_edReaderMac = 0;
	m_btnClearMac = 0;
	m_btnAutoTest = 0;
	m_rchInfo     = 0;
	m_btnTemp     = 0;
	m_opPass      = 0;
	m_opNotPass   = 0;

	for (int i = 0; i < 3; i++) {
		m_opErrReason[i] = 0;
	}

	m_btnSaveResult = 0;
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {

	g_data.m_hWnd = GetHWND();

	m_lblStatus   = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblStatus"));
	m_edReaderMac = static_cast<DuiLib::CEditUI*>(m_PaintManager.FindControl("edReaderMac"));
	m_btnClearMac = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnClearMac"));
	m_btnAutoTest = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnAutoTest"));
	m_rchInfo     = static_cast<DuiLib::CRichEditUI*>(m_PaintManager.FindControl("rchInfo"));
	m_btnTemp     = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnTemp"));
	m_opPass      = static_cast<DuiLib::COptionUI*>(m_PaintManager.FindControl("radio1"));
	m_opNotPass   = static_cast<DuiLib::COptionUI*>(m_PaintManager.FindControl("radio2"));

	CDuiString  strText;
	for ( int i = 0; i < 3; i++ ) {
		strText.Format("option%d", i + 1);
		m_opErrReason[i] = static_cast<DuiLib::COptionUI*>(m_PaintManager.FindControl(strText));
	}

	m_btnSaveResult = static_cast<DuiLib::CButtonUI*>(m_PaintManager.FindControl("btnSaveResult"));

#if TEST_FLAG
	m_edReaderMac->SetText("18994455DDBB");
#endif
	
	CheckDevices(); 
	CheckControls();

	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString  strName;
	strName = msg.pSender->GetName();

	if ( msg.sType == "click" ) {
		if (strName == "btnSetting") {
			RECT r = msg.pSender->GetPos();
			POINT point = { r.left, r.bottom };

			CMenuWnd* pMenu = new CMenuWnd(m_hWnd, msg.pSender);
			// CDuiPoint point = msg.ptMouse;
			ClientToScreen(m_hWnd, &point); 

			//使用资源方式
			//STRINGorID xml(IDR_XML1);
			//pMenu->Init(NULL, xml, "xml", point);  

			//使用文件方式
			STRINGorID xml("menusetting.xml"); 
			pMenu->Init(NULL, xml, 0, point); 
		}
		else if (strName == "btnAutoTest") {
			OnAutoTest();
		}
	}
	else if (msg.sType == "textchanged") {
		
	}
	WindowImplBase::Notify(msg);
}

void CDuiFrameWnd::OnFinalMessage(HWND hWnd) {
	CBusiness::GetInstance()->DisconnectBleAsyn();
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_COM_STATUS ) {
		int nCom  = (int)wParam;
		BOOL bRet = (BOOL)lParam;
		CDuiString strText;

		if (nCom > 0) {
			if (bRet) {
				strText.Format("打开串口com%d成功", nCom);
				m_bComConnected = TRUE;
				m_bAutoTestStarted = FALSE;
			}
			else {
				strText.Format("打开串口com%d失败！", nCom);
				m_bComConnected = FALSE;
			}
		}
		else if (nCom == 0) {
			strText.Format("没有打开蓝牙模块串口");     
			m_bComConnected = FALSE;
		}
		else {
			strText.Format("找到多个蓝牙模块，请确保只用一个");
			m_bComConnected = FALSE;
		}

		m_lblStatus->SetText(strText);
		CheckControls();
	}
	else if (WM_DEVICECHANGE == uMsg) {
		CheckDevices();
	}
	else if (UM_INFO_MSG == uMsg) {
		OnInfoMsg(wParam, lParam);
	}
	else if (UM_BLUETOOTH_CNN_RET == uMsg) {
		OnBluetoothCnnMsg(wParam, lParam);
	}
	else if (UM_STOP_AUTO_TEST_RET == uMsg) {
		m_bAutoTestStarted = FALSE;
		CheckControls();
	}
	else if (UM_AUTO_TEST_FIN == uMsg) {
		m_bAutoTestFinished = TRUE;
		CheckControls();
	}
	else if (UM_BLE_DISCONNECTED == uMsg) {
		OnBleDisconnected(wParam, lParam);
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

void CDuiFrameWnd::CheckDevices() {	
	CBusiness::GetInstance()->CheckDevicesAsyn();
}

void  CDuiFrameWnd::CheckControls() {
	// 如果没有连接蓝牙模块的串口
	if (!m_bComConnected) {
		m_edReaderMac->SetEnabled(false);
		m_edReaderMac->SetBkColor(0xFFF1F1F1);
		m_btnClearMac->SetEnabled(false);
		m_btnAutoTest->SetEnabled(false);
		m_btnTemp->SetEnabled(false);
		m_opPass->SetEnabled(false);
		m_opNotPass->SetEnabled(false);
		for (int i = 0; i < 3; i++) {
			m_opErrReason[i]->SetEnabled(false);
		}
		m_btnSaveResult->SetEnabled(false);
	}
	else {
		// 自动测试开始了
		if (m_bAutoTestStarted) {

			if ( m_bAutoTestFinished ) {
				m_edReaderMac->SetEnabled(false);
				m_edReaderMac->SetBkColor(0xFFF1F1F1);
				m_btnClearMac->SetEnabled(false);
				m_btnAutoTest->SetEnabled(true);
				m_btnAutoTest->SetText("停止测试");
				m_btnTemp->SetEnabled(true);
				m_opPass->SetEnabled(true);
				m_opNotPass->SetEnabled(true);
				for (int i = 0; i < 3; i++) {
					m_opErrReason[i]->SetEnabled(true);
				}
				m_btnSaveResult->SetEnabled(true);
			}
			else {
				m_edReaderMac->SetEnabled(false);
				m_edReaderMac->SetBkColor(0xFFF1F1F1);
				m_btnClearMac->SetEnabled(false);
				m_btnAutoTest->SetEnabled(false);
				m_btnAutoTest->SetText("停止测试");
				m_btnTemp->SetEnabled(false);
				m_opPass->SetEnabled(false);
				m_opNotPass->SetEnabled(false);
				for (int i = 0; i < 3; i++) {
					m_opErrReason[i]->SetEnabled(false);
				}
				m_btnSaveResult->SetEnabled(false);
			}			
		}
		else {
			m_edReaderMac->SetEnabled(true);
			m_edReaderMac->SetBkColor(0xFFFFFFFF);
			m_btnClearMac->SetEnabled(true);
			m_btnAutoTest->SetEnabled(true);
			m_btnAutoTest->SetText("自动测试");
			m_btnTemp->SetEnabled(false);
			m_opPass->SetEnabled(false);
			m_opNotPass->SetEnabled(false);
			for (int i = 0; i < 3; i++) {
				m_opErrReason[i]->SetEnabled(false);
			}
			m_btnSaveResult->SetEnabled(false);
		}
	}
}

void   CDuiFrameWnd::OnAutoTest() {
	//if (m_bAutoTestStarted) {
	//	m_bAutoTestStarted = FALSE;
	//}
	//else {
	//	m_bAutoTestStarted = TRUE;
	//}
	//CheckControls();

	if (!m_bComConnected) {
		MessageBox(GetHWND(), "没有打开蓝牙模块的串口", "错误", 0);
		return;
	}

	// 如果还没有开始测试
	if (!m_bAutoTestStarted) {
		CDuiString strMac = m_edReaderMac->GetText();
		strMac.Trim();

		if ( strMac.GetLength() != 12 ) {
			MessageBox(GetHWND(), "小蓝牙Reader的Mac地址格式错误，正确格式为xxxxxxxxxxxx", "错误", 0);
			return;
		}

		for ( int i = 0; i < 12; i++ ) {
			char ch = Char2Lower(strMac.GetAt(i));
			if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f'))) {
				MessageBox(GetHWND(), "小蓝牙Reader的Mac地址格式错误，正确格式为xxxxxxxxxxxx", "错误", 0);
				return;
			}
		}

		CBusiness::GetInstance()->StartAutoTestAsyn( strMac );
		m_rchInfo->SetText("");
		m_opPass->Selected(false);
		m_opNotPass->Selected(false);
		for (int i = 0; i < 3; i++) {
			m_opErrReason[i]->Selected(false);
		}
		m_bAutoTestStarted = TRUE;
		m_bAutoTestFinished = FALSE;
		m_bBleConnected = FALSE;
		CheckControls();
	}
	else {
		CBusiness::GetInstance()->StopAutoTestAsyn();
	}
}

void   CDuiFrameWnd::OnInfoMsg(WPARAM wParam, LPARAM  lParam) {
	InfoType  infoType = (InfoType)wParam;
	CDuiString  strText = m_rchInfo->GetText();
	BOOL  bRet = FALSE;

	switch (infoType)
	{
	case CONNECTING:
	{
		strText += "正在连接蓝牙...\n";
		m_rchInfo->SetText(strText);
	}
	break;

	case TEMPING:
	{
		strText += "正在测温...\n";
		m_rchInfo->SetText(strText);
	}
	break;

	case TEMP_RET:
	{
		TempItem * pItem = (TempItem *)lParam;
		CDuiString  s;

		if ( pItem ) {			
			s.Format("测温成功：tag id: %s, 温度: %.2f℃\r\n", pItem->szTagId, pItem->dwTemp / 100.0);
			strText += s;
			m_rchInfo->SetText(strText);
			delete pItem;
			m_opPass->Selected(true);
		}
		else {
			s.Format("测温失败！\r\n");
			strText += s;
			m_rchInfo->SetText(strText);
			m_opNotPass->Selected(true);
		}
	}
	break;

	case SETTING_NOTIFYID:
	{
		strText += "正在设置NotifyId...\n";
		m_rchInfo->SetText(strText);
	}
	break;

	case SETTING_NOTIFYID_RET:
	{
		bRet = lParam;
		if (bRet) {
			strText += "设置NotifyId成功\n";
			m_rchInfo->SetText(strText);
		}
		else {
			strText += "设置NotifyId失败!\n";
			m_rchInfo->SetText(strText);
		}
	}
	break;

	case ATING:
	{
		strText += "正在发送AT命令...\n";
		m_rchInfo->SetText(strText);
	}
	break;

	case ATING_RET:
	{
		bRet = lParam;
		if (bRet) {
			strText += "AT命令成功\n";
			m_rchInfo->SetText(strText); 
		}
		else {
			strText += "AT命令失败!\n";
			m_rchInfo->SetText(strText);
		}
	}
	break;

	default:
		break;
	}
}

void   CDuiFrameWnd::OnBluetoothCnnMsg(WPARAM wParam, LPARAM  lParam) {
	BOOL  bRet = wParam;
	CDuiString  strText = m_rchInfo->GetText();

	if ( bRet ) {
		strText += "连接蓝牙成功\n";
		m_rchInfo->SetText(strText);
		m_bBleConnected = TRUE;
	}
	else {
		strText += "连接蓝牙失败！\n";
		m_rchInfo->SetText(strText);
		m_bBleConnected = FALSE;
	}
}

void   CDuiFrameWnd::OnBleDisconnected(WPARAM wParam, LPARAM  lParam) {
	m_bAutoTestStarted = FALSE;
	CDuiString  strText = m_rchInfo->GetText();
	strText += "蓝牙断开!\n";
	m_rchInfo->SetText(strText);
	CheckControls();
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


