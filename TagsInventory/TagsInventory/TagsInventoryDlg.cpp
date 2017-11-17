
// TagsInventoryDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TagsInventory.h"
#include "TagsInventoryDlg.h"
#include "afxdialogex.h"
#include "MainDlg.h"
#include "MyEncrypt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static HWND      s_hLogin = 0;
static CMainDlg  s_MainDlg;

// CTagsInventoryDlg 对话框

CDuiString  CDuiFrameWnd::GetSkinFile() {
	//char buf[8192];
	//g_cfg->GetConfig("login xml file", buf, sizeof(buf), "login.xml");
	//return buf;
	return "login.xml";
}

void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	if (msg.sType == _T("click"))
	{
#if DEBUG_FLAG
		AfxGetApp()->m_pMainWnd = &s_MainDlg;
		::PostMessage(s_hLogin, WM_CLOSE, 0, 0);
		s_MainDlg.DoModal();
#endif
	}
}


void CDuiFrameWnd::SetStatus( int nStatus, int nType ) {
	CLabelUI * pStatus = 0;
	const char * szType = 0;

	if (STATUS_TYPE_DATABASE == nType) {
		pStatus = static_cast <CLabelUI*> (m_PaintManager.FindControl(_T("lblStatusDb")));
		szType = "数据库";
	}
	else {
		pStatus = static_cast <CLabelUI*> (m_PaintManager.FindControl(_T("lblStatusReader")));
		szType = "读卡器";
	}
	
	if (0 == pStatus) {
		return;
	}
	
	CString strText;
	if (STATUS_OPEN == nStatus) {
		strText.Format("%s连接OK", szType);
	}
	else {
		strText.Format("%s没有连接上", szType);
	}

	pStatus->SetText(strText);
}




CTagsInventoryDlg::CTagsInventoryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TAGSINVENTORY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTagsInventoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTagsInventoryDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE( UM_SHOW_STATUS, OnStatusMessage )
	ON_MESSAGE( UM_TAG_ITEM,    OnTagItemMessage )
	ON_MESSAGE(UM_VERIFY_USER_RESULT, OnVerifyResultMessage)	
END_MESSAGE_MAP()


// CTagsInventoryDlg 消息处理程序

BOOL CTagsInventoryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CPaintManagerUI::SetInstance(AfxGetInstanceHandle());                    // 指定duilib的实例
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());    // 指定duilib资源的路径，这里指定为和exe同目录
	//::CoInitialize(NULL);  // 记得释放::CoUninitialize();

	CRect rect;
	GetClientRect(rect);
	m_duiFrame.Create(*this, _T("DUIWnd"), UI_WNDSTYLE_CHILD, 0, 0, 0, rect.Width(), rect.Height());
	m_duiFrame.ShowWindow(TRUE);

	s_hLogin = this->m_hWnd;
	g_hDlg = this->m_hWnd;

	DWORD dwWidth = 0, dwHeight = 0;
	DWORD dwCaptionHeight = 30;
	DWORD dwDlgBorder = 10;
	g_cfg->GetConfig("login width", dwWidth,  800);
	g_cfg->GetConfig("login height", dwHeight, 600);
	g_cfg->GetConfig("caption height", dwCaptionHeight, 30);
	g_cfg->GetConfig("dialog border", dwDlgBorder, 10);

	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	this->MoveWindow( (screen_width - dwWidth)/2, (screen_height - dwHeight) / 2, dwWidth, dwHeight);
	::MoveWindow(m_duiFrame.GetHWND(), 0, 0, dwWidth - 2 * dwDlgBorder, dwHeight - dwCaptionHeight - dwDlgBorder, TRUE);

	g_thrd_db->PostMessage( g_handler_db, MSG_RECONNECT_DB );
	g_thrd_reader->PostMessage(g_handler_reader, MSG_RECONNECT_READER);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

LRESULT CTagsInventoryDlg::OnStatusMessage(WPARAM wParam, LPARAM lParam) {
	m_duiFrame.SetStatus( lParam, wParam );
	return 0;
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTagsInventoryDlg::OnPaint()
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
HCURSOR CTagsInventoryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT  CTagsInventoryDlg::OnTagItemMessage(WPARAM wParam, LPARAM lParam) {
	static DWORD  dwLastTick = 0;
	TagItem * pItem = (TagItem *)wParam;

	DWORD dwCur = LmnGetTickCount();
	// 不要发送太频繁
	if (dwCur - dwLastTick < 1000) {
		delete pItem;
		return 0;
	}
	dwLastTick = dwCur;

	g_thrd_db->PostMessage(g_handler_db, MSG_VERIFY_USER, new CTagItem(pItem) );
	delete pItem;
	return 0;
}

LRESULT CTagsInventoryDlg::OnVerifyResultMessage(WPARAM wParam, LPARAM lParam) {
	BOOL bRet = (BOOL)wParam;
	// 验证失败
	if ( !bRet) {
		return 0;
	}

	AfxGetApp()->m_pMainWnd = &s_MainDlg;
	::PostMessage(s_hLogin, WM_CLOSE, 0, 0);
	s_MainDlg.DoModal();
	return 0;
}

