// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include <time.h>

#define  HAND_TAG_COUNT    10
static  DWORD  s_HandDelay[HAND_TAG_COUNT] = { 10000, 11000, 12000, 13000, 14000,
                                              15000, 16000, 17000, 18000, 19000 };
static  DWORD  s_HandTick[HAND_TAG_COUNT] = { 0 };

LPCTSTR  CMyListData::GetItemText( CControlUI* pControl, int iIndex, int iSubItem ) {
	char szBuf[256];
	assert(iIndex < 10);

	switch (iSubItem)
	{
	case 0:
		SNPRINTF(szBuf, sizeof(szBuf), "%d", iIndex + 1);
		break;

	case 1:
		Data2String(szBuf, sizeof(szBuf), m_data[iIndex].m_data, m_data[iIndex].m_dwDataLen);
		break;

	case 2:
		DateTime2String(szBuf, sizeof(szBuf), &m_data[iIndex].m_time);
		break;

	default:
		break;
	}

	pControl->SetUserData(szBuf);
	return pControl->GetUserData();
}




CDuiFrameWnd::CDuiFrameWnd() {
	   
}

CDuiFrameWnd::~CDuiFrameWnd() {

}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = GetHWND();

	m_lblStatus = static_cast<CLabelUI *>(m_PaintManager.FindControl("lblLaunchStatus"));
	m_btnSurgery = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnSurgery"));
	m_btnHand = static_cast<CButtonUI *>(m_PaintManager.FindControl("btnHand"));
	m_lstReceive = static_cast<CListUI *>(m_PaintManager.FindControl("lstReceive"));
	m_lstSend = static_cast<CListUI *>(m_PaintManager.FindControl("lstSend"));

	m_lstReceive->SetTextCallback(&m_lstReceiveData);
	m_lstSend->SetTextCallback(&m_lstSendData);

	for (int i = 0; i < HAND_TAG_COUNT; i++) {
		//s_HandTick[i] = LmnGetTickCount();
		s_HandTick[i] = 0;
	}

	CBusiness::GetInstance()->ConnectComPortAsyn();
	// 启动定时器，发送hand查询结果
	SetTimer( GetHWND(), 1, 100, 0 );
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString  name = msg.pSender->GetName();
	if ( msg.sType == "click" ) {
		if ( name == "btnSurgery") {
			OnBtnSurgery();
		}
		else if (name == "btnHand") {
			OnBtnHand();
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if ( uMsg == UM_STATUS ) {
		if ( wParam == TRUE ) {
			m_lblStatus->SetText("发射器连接OK");
		}
	}
	else if (uMsg == UM_READ_DATA) {
		BYTE  * pData = (BYTE *)wParam;
		DWORD   dwDataLen = lParam;

		if (dwDataLen <= 128) {
			MyListItem item;
			memcpy(item.m_data, pData, dwDataLen);
			item.m_dwDataLen = dwDataLen;
			item.m_time = time(0);
			m_lstReceiveData.m_data.Append(item);

			int nCnt = m_lstReceive->GetCount();
			if (nCnt < 10) {
				CListTextElementUI* pItem = new CListTextElementUI;
				m_lstReceive->Add(pItem);
			}
			m_lstReceive->Invalidate();
		}

		delete[] pData;
	}
	else if (uMsg == UM_SEND_DATA) {
		BYTE  * pData = (BYTE *)wParam;
		DWORD   dwDataLen = lParam;

		if (dwDataLen <= 128) {
			MyListItem item;
			memcpy(item.m_data, pData, dwDataLen);
			item.m_dwDataLen = dwDataLen;
			item.m_time = time(0);
			m_lstSendData.m_data.Append(item);

			int nCnt = m_lstSend->GetCount();
			if (nCnt < 10) {
				CListTextElementUI* pItem = new CListTextElementUI;
				m_lstSend->Add(pItem);
			}
			m_lstSend->Invalidate();
		}

		delete[] pData;
	}
	else if (uMsg == WM_TIMER) {
		if (wParam == 1) {
			DWORD  dwCurTick = LmnGetTickCount();
			for ( int i = 0; i < HAND_TAG_COUNT; i++ ) {
				DWORD  dwElapsed = dwCurTick - s_HandTick[i];
				// 如果时间到
				if ( dwElapsed >= s_HandDelay[i] ) {


					s_HandTick[i] = dwCurTick;
				}
			}
		}
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}
      
void  CDuiFrameWnd::OnBtnSurgery() {
	if ( g_data.m_bSurgery ) {
		g_data.m_bSurgery = FALSE;
		m_btnSurgery->SetText("术中监控(关闭)");
	}
	else {
		g_data.m_bSurgery = TRUE;
		m_btnSurgery->SetText("术中监控(开启)");
	}
}

void  CDuiFrameWnd::OnBtnHand() {
	if (g_data.m_bHand) {
		g_data.m_bHand = FALSE;
		m_btnHand->SetText("手持(关闭)");
	}
	else {
		g_data.m_bHand = TRUE;
		m_btnHand->SetText("手持(开启)");
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


