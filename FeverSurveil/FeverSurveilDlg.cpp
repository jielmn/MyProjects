
// FeverSurveilDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FeverSurveil.h"
#include "FeverSurveilDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CDuiString  CDuiFrameWnd::GetSkinFile() {
	//char buf[8192];
	return "main.xml";
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	if (msg.sType == _T("click"))
	{
		CDuiString sendName = msg.pSender->GetName();
		if (sendName == _T("sync"))
		{
			::PostMessage(g_hwndMain, UM_SYNC, 0, 0);
		}
	}
}

void CDuiFrameWnd::ClearList() {
	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_data")));
	if (pList) {
		pList->RemoveAll();
	}
}

void CDuiFrameWnd::SetControlText(const char * szControlName, CDuiString strText) {
	if (0 == szControlName) {
		return;
	}

	CControlUI * pCtrl = 0;
	pCtrl = m_PaintManager.FindControl(szControlName);
	if (0 == pCtrl) {
		return;
	}

	pCtrl->SetText(strText);
}

void CDuiFrameWnd::ShowTagData(std::vector<TagData *> * pVec) {
	if (0 == pVec) {
		return;
	}

	CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("list_data")));
	if ( 0 == pList) {
		return;
	}

	std::vector<TagData *>::iterator it;
	int i = 0;
	char buf[8192];
	CDuiString txt;

	for (it = pVec->begin(), i=0; it != pVec->end(); it++, i++) {
		TagData * pItemData = *it;

		CListTextElementUI* pListElement = new CListTextElementUI;
		pList->Add(pListElement);
	
		txt.Format("%d", i + 1);
		pListElement->SetText(0, txt);

		GetUid(buf, sizeof(buf), pItemData->abyTagId, sizeof(pItemData->abyTagId), '-');
		pListElement->SetText(2, buf);

		struct tm timeinfo;
		localtime_s(&timeinfo, &pItemData->tTime);
		txt.Format("%d-%d-%d %d:%d:00", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min);
		pListElement->SetText(4, txt);

		txt.Format("%.2f", pItemData->dwTemperatur / 100.0);
		pListElement->SetText(6, txt);
	}

	CLabelUI* pCount = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("lblCount")));
	if ( pCount) {
		txt.Format("总数：%lu个", pVec->size());
		pCount->SetText(txt);;
	}
}

int  CDuiFrameWnd::GetSelectedHospital() {
	COptionUI * pOp = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("guiyang")));
	// 贵阳医院
	if (pOp) {
		if (pOp->IsSelected()) {
			return 0;
		}
	}

	// 六盘水医院
	pOp = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("liupanshui")));
	if (pOp) {
		if (pOp->IsSelected()) {
			return 1;
		}
	}
	return 0;
}





// CFeverSurveilDlg 对话框



CFeverSurveilDlg::CFeverSurveilDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FEVERSURVEIL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_eMainStatus = COM_STATUS_CLOSED;
	m_eSubStatus = COM_STATUS_IDLE;
	m_bPrepared = FALSE;

	m_eDbStatus = DB_STATUS_CLOSED;
}

void CFeverSurveilDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFeverSurveilDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(UM_SERIAL_PORT_STATE, OnSerialPortStatusMsg)
	ON_MESSAGE(UM_SYNC_RESULT, OnSyncResultMsg)
	ON_MESSAGE(UM_SYNC, OnSyncMsg)	
	ON_MESSAGE(UM_DB_STATE, OnDbStatusMsg)
END_MESSAGE_MAP()


// CFeverSurveilDlg 消息处理程序

BOOL CFeverSurveilDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	g_hwndMain = this->m_hWnd;

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CRect rect;
	GetClientRect(rect);
	m_duiFrame.Create(*this, _T("DUIWnd"), UI_WNDSTYLE_CHILD, 0, 0, 0, rect.Width(), rect.Height());
	m_duiFrame.ShowWindow(TRUE);

	DWORD dwWidth = 0, dwHeight = 0;
	DWORD dwCaptionHeight = 30;
	DWORD dwDlgBorder = 10;

	g_cfg->GetConfig("main width", dwWidth, 800);
	g_cfg->GetConfig("main height", dwHeight, 600);
	g_cfg->GetConfig("caption height", dwCaptionHeight, 30);
	g_cfg->GetConfig("dialog border", dwDlgBorder, 10);

	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	this->MoveWindow((screen_width - dwWidth) / 2, (screen_height - dwHeight) / 2, dwWidth, dwHeight);
	::MoveWindow(m_duiFrame.GetHWND(), 0, 0, dwWidth - 2 * dwDlgBorder, dwHeight - dwCaptionHeight - dwDlgBorder, TRUE);


	ReconnectDb(TRUE);
	ReconnectSerialPort( TRUE );

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFeverSurveilDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFeverSurveilDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT  CFeverSurveilDlg::OnSerialPortStatusMsg(WPARAM wParam, LPARAM lParam) {
	ComStatus eMainStatus = ComStatus(wParam >> 16);
	BOOL      bPrepared = wParam & 0x0000FFFF;
	ComStatus eSubStatus = (ComStatus)lParam;

	if (eMainStatus == COM_STATUS_OPEN) {		
		if (eSubStatus == COM_STATUS_UPLOADING) {
			m_duiFrame.SetControlText("serial_port_status", "正在同步数据......");
		}
		else if (eSubStatus == COM_STATUS_IDLE) {
			if ( bPrepared )
			{ 
				m_duiFrame.SetControlText("serial_port_status", "串口和Reader连接OK");
			}
		}
		else if (eSubStatus == COM_STATUS_PREPARING) {
			m_duiFrame.SetControlText("serial_port_status", "正在连接Reader......");
		}
		else if (eSubStatus == COM_STATUS_CLEARING) {
			m_duiFrame.SetControlText("serial_port_status", "正在清除Reader存储数据......");
		}
	} else if (eMainStatus == COM_STATUS_CLOSED ) {
		m_duiFrame.SetControlText("serial_port_status", "串口或Reader连接失败");
		ReconnectSerialPort();
	} 

	m_eMainStatus = eMainStatus;
	m_eSubStatus = eSubStatus;
	m_bPrepared = bPrepared;

	return 0;
}

LRESULT  CFeverSurveilDlg::OnSyncResultMsg(WPARAM wParam, LPARAM lParam) {
	std::vector<TagData *> * pVec = (std::vector<TagData *> *)wParam;
	m_duiFrame.ShowTagData(pVec);
	UpdateFeverData(m_duiFrame.GetSelectedHospital(), pVec->size());

	if (pVec->size() == 0) {
		MessageBox("读卡器的数据为0条。请先用读卡器扫描tag", PROJ_CAPTION);
	}
	else {
		ClearReader();
	}

	if (pVec) {
		ClearVector(*pVec);
		delete pVec;
	}
	return 0;
}

LRESULT  CFeverSurveilDlg::OnSyncMsg(WPARAM wParam, LPARAM lParam) {
	if (m_eMainStatus != COM_STATUS_OPEN) {
		MessageBox("串口没有打开！", PROJ_CAPTION);
		return 0;
	}	

	if (m_eSubStatus != COM_STATUS_IDLE) {
		MessageBox("串口正忙，请稍候！", PROJ_CAPTION);
		return 0;
	}

	if (!m_bPrepared) {
		MessageBox("Reader还没有连接上！", PROJ_CAPTION);
		return 0;
	}

	if (m_eDbStatus != DB_STATUS_OPEN) {
		MessageBox("数据库还没有连接上！", PROJ_CAPTION);
		return 0;
	}

	m_duiFrame.ClearList();
	Sync();

	m_eSubStatus = COM_STATUS_UPLOADING;
	return 0;
}

LRESULT  CFeverSurveilDlg::OnDbStatusMsg(WPARAM wParam, LPARAM lParam) {
	DbStatus  eStatus = (DbStatus)wParam;

	if (eStatus == DB_STATUS_OPEN) {
		m_duiFrame.SetControlText("db_status", "数据库连接OK");
	}
	else {
		m_duiFrame.SetControlText("db_status", "数据库连接断开");
		ReconnectDb();
	}

	m_eDbStatus = eStatus;
	return 0;
}