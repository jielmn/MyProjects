
// TimeConvertDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "时间类型转换.h"
#include "TimeConvertDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTimeConvertDlg 对话框



CTimeConvertDlg::CTimeConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTimeConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TIME_INT, m_edInt);
	DDX_Control(pDX, IDC_EDIT_TIME_STRING, m_edString);
	DDX_Control(pDX, IDC_STATIC_ERROR, m_stError);
}

BEGIN_MESSAGE_MAP(CTimeConvertDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_2_STRING, &CTimeConvertDlg::OnBnClickedButton2String)
	ON_BN_CLICKED(IDC_BUTTON_2_INT, &CTimeConvertDlg::OnBnClickedButton2Int)
END_MESSAGE_MAP()


// CTimeConvertDlg 消息处理程序

BOOL CTimeConvertDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_stError.SetWindowText("");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTimeConvertDlg::OnPaint()
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
HCURSOR CTimeConvertDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTimeConvertDlg::OnBnClickedButton2String()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  strText;
	m_edInt.GetWindowText(strText);

	if ( strText.GetLength() == 0 ) {
		m_stError.SetWindowText("请输入数字");
		return;
	}

	if (strText.GetLength() > 10 ) {
		m_stError.SetWindowText("超过10个字符");
		return;
	}

	time_t  ts = 0;
	int ret = sscanf(strText, "%llu", &ts );
	if ( ret <= 0 ) {
		m_stError.SetWindowText("请输入数字");
		return;
	}

	char buf[8192];
	Date2String( buf, sizeof(buf), &ts );

	m_edString.SetWindowText(buf);
	m_stError.SetWindowText("");
}


void CTimeConvertDlg::OnBnClickedButton2Int()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  strText;
	m_edString.GetWindowText(strText);

	if (strText.GetLength() == 0) {
		m_stError.SetWindowText("请输入字符串");
		return;
	}

	if (strText.GetLength() != 19) {
		m_stError.SetWindowText("请输入格式：yyyy-mm-dd hh:mm:ss");
		return;
	}

	time_t ts = String2Date(strText);
	strText.Format("%llu", ts);

	m_edInt.SetWindowText(strText);
	m_stError.SetWindowText("");
}
