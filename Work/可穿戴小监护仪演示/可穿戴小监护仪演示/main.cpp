// win32_1.cpp : 定义应用程序的入口点。
//

#ifdef _DEBUG
#include "vld.h"
#endif

#include "Windows.h"
#include "main.h"
#include "business.h"
#include "resource.h"
#include "SettingDlg.h"

#define   ITEM_WIDTH      240
#define   ITEM_HEIGHT     240  

CDuiFrameWnd::CDuiFrameWnd() {
	m_nColumns = 2;
}

CDuiFrameWnd::~CDuiFrameWnd() {
	 
}

void  CDuiFrameWnd::InitWindow() {
	g_data.m_hWnd = m_hWnd;
	PostMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	m_layMain = static_cast<DuiLib::CTileLayoutUI*>(m_PaintManager.FindControl("layGrids"));
	m_lblStatus = static_cast<DuiLib::CLabelUI*>(m_PaintManager.FindControl("lblDatabaseStatus"));
	m_lblStatus->SetText("没有打开串口");

#if TEST_FLAG
	// 添加窗格
	for (DWORD i = 0; i < MAX_GRID_COUNT; i++) {
		m_grids[i] = new CGridUI;
		m_grids[i]->SetName("grid");
		m_grids[i]->SetTag(i);
		m_grids[i]->SetIndex(i + 1);
		m_grids[i]->SetBeat((BYTE)(90 + i), 1);
		m_grids[i]->SetOxy((BYTE)(80 + i), 1);
		m_grids[i]->SetTemp(3600 + i * 10);
		m_grids[i]->SetPose((BYTE)i);
		m_grids[i]->SetNo((BYTE)i);
		m_grids[i]->SetFreq((BYTE)i);
		m_layMain->Add(m_grids[i]);
	}
#endif

	SIZE s;
	s.cx = ITEM_WIDTH;
	s.cy = ITEM_HEIGHT;
	m_layMain->SetItemSize(s);
	m_layMain->SetFixedColumns(m_nColumns);

	m_layMain->OnSize += MakeDelegate(this, &CDuiFrameWnd::OnMainSize);

	CBusiness::GetInstance()->OpenComAsyn();
	WindowImplBase::InitWindow();
}

CControlUI * CDuiFrameWnd::CreateControl(LPCTSTR pstrClass) {
	return WindowImplBase::CreateControl(pstrClass);
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	if (msg.sType == "click") {
		if (msg.pSender->GetName() == "menubtn") {
			CSettingDlg * pSettingDlg = new CSettingDlg;
			pSettingDlg->Create(this->m_hWnd, _T("设置"), UI_WNDSTYLE_FRAME | WS_POPUP, NULL, 0, 0, 0, 0);
			pSettingDlg->CenterWindow();
			int ret = pSettingDlg->ShowModal();

			// 如果是click ok
			if (0 == ret) {
				if (pSettingDlg->m_nComPort != g_data.m_nComPort) {
					g_data.m_nComPort = pSettingDlg->m_nComPort;
					DWORD  dwValue = g_data.m_nComPort;
					g_data.m_cfg->SetConfig("serial port", dwValue);
					g_data.m_cfg->Save();
					CBusiness::GetInstance()->OpenComAsyn();
					m_layMain->RemoveAll();
				}
			}
			delete pSettingDlg;
		}
	}
	WindowImplBase::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == UM_COM_STATUS) {
		BOOL bRet = wParam;
		CDuiString strText;
		if (bRet) {
			strText.Format("打开串口com%d成功", g_data.m_nComPort);
			m_lblStatus->SetText(strText);
		}
		else {
			strText.Format("打开串口com%d失败", g_data.m_nComPort);
			m_lblStatus->SetText(strText);
		}
	}
	else if (uMsg == UM_COM_DATA) {
		BYTE * pData = (BYTE *)wParam;
		DWORD  dwDataLen = lParam;
		assert(dwDataLen == 13);
		assert(pData);

		BYTE  byBeat  = pData[2];
		BYTE  byBeatV = pData[3];
		BYTE  byOxy   = pData[4];
		BYTE  byOxyV  = pData[5];
		int   nTemp   = pData[6] * 100 + pData[7];
		BYTE  byPose  = pData[8];
		BYTE  byFreq  = pData[9];
		BYTE  byNo    = pData[10];

		int cnt = m_layMain->GetCount();
		CGridUI * pItem = 0;
		for ( int i = 0; i < cnt; i++ ) {
			pItem = (CGridUI *)m_layMain->GetItemAt(i);
			// 找到
			if (pItem->GetFreq() == byFreq && pItem->GetNo() == byNo) {
				break;
			}
			else {
				pItem = 0;
			}
		}


		// 没有找到
		if ( pItem == 0 ) {
			pItem = new CGridUI;
			m_layMain->Add(pItem);
			pItem->SetNo(byNo);
			pItem->SetFreq(byFreq);
			pItem->SetIndex(cnt + 1);
		}

		pItem->SetBeat(byBeat, byBeatV);
		pItem->SetOxy(byOxy, byOxyV);
		pItem->SetTemp(nTemp);
		pItem->SetPose(byPose);

		BOOL bAbnormal = IsAbnormal(byBeat, byBeatV, byOxy, byOxyV, nTemp);
		pItem->SetAbnormal(bAbnormal);
		delete[] pData;
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);  
}

bool  CDuiFrameWnd::OnMainSize(void * pParam) {
	RECT r = m_layMain->GetPos();
	int width = r.right - r.left;
	int nColumns = width / ITEM_WIDTH;
	if (nColumns <= 0)
		nColumns = 1;

	m_nColumns = nColumns;
	m_layMain->SetFixedColumns(m_nColumns);

	SIZE  s;
	s.cx = width / m_nColumns;
	s.cy = ITEM_HEIGHT;
	m_layMain->SetItemSize(s);
	 
	return true;
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


