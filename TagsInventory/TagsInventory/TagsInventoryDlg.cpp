
// TagsInventoryDlg.cpp : ʵ���ļ�
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

// CTagsInventoryDlg �Ի���

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
		szType = "���ݿ�";
	}
	else {
		pStatus = static_cast <CLabelUI*> (m_PaintManager.FindControl(_T("lblStatusReader")));
		szType = "������";
	}
	
	if (0 == pStatus) {
		return;
	}
	
	CString strText;
	if (STATUS_OPEN == nStatus) {
		strText.Format("%s����OK", szType);
	}
	else {
		strText.Format("%sû��������", szType);
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


// CTagsInventoryDlg ��Ϣ�������

BOOL CTagsInventoryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CPaintManagerUI::SetInstance(AfxGetInstanceHandle());                    // ָ��duilib��ʵ��
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());    // ָ��duilib��Դ��·��������ָ��Ϊ��exeͬĿ¼
	//::CoInitialize(NULL);  // �ǵ��ͷ�::CoUninitialize();

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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

LRESULT CTagsInventoryDlg::OnStatusMessage(WPARAM wParam, LPARAM lParam) {
	m_duiFrame.SetStatus( lParam, wParam );
	return 0;
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTagsInventoryDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTagsInventoryDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT  CTagsInventoryDlg::OnTagItemMessage(WPARAM wParam, LPARAM lParam) {
	static DWORD  dwLastTick = 0;
	TagItem * pItem = (TagItem *)wParam;

	DWORD dwCur = LmnGetTickCount();
	// ��Ҫ����̫Ƶ��
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
	// ��֤ʧ��
	if ( !bRet) {
		return 0;
	}

	AfxGetApp()->m_pMainWnd = &s_MainDlg;
	::PostMessage(s_hLogin, WM_CLOSE, 0, 0);
	s_MainDlg.DoModal();
	return 0;
}

