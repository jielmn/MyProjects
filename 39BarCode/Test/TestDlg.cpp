
// TestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"
#include "afxdialogex.h"

#include "39BarCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestDlg 对话框



CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_font = CreateFont(
		33,                            // nHeight
		0,                            // nWidth
		0,                            // nEscapement
		0,                            // nOrientation
		FW_NORMAL,                        // nWeight
		FALSE,                        // bItalic
		FALSE,                        // bUnderline
		0,                            // cStrikeOut
		ANSI_CHARSET,                    // nCharSet
		OUT_DEFAULT_PRECIS,            // nOutPrecision
		CLIP_DEFAULT_PRECIS,            // nClipPrecision
		DEFAULT_QUALITY,                // nQuality
		DEFAULT_PITCH | FF_SWISS,        // nPitchAndFamily
		_T("宋体")
	);
}

CTestDlg::~CTestDlg() {
	DeleteObject(m_font);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTestDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CTestDlg 消息处理程序

BOOL CTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestDlg::OnPaint()
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
		CPaintDC  dc(this);
		CRect rect;
		GetClientRect(rect);

		::Rectangle(dc.m_hDC, 0, 0, rect.Width(), rect.Height());

		//DrawBarcode(dc.m_hDC, 100, 100, 600, 100, "WHET201805020045", m_font, 50, "序列号：");
		int ret  = DrawBarcode128(dc.m_hDC, 100,  50, 600, 100, "WHET201805020044", m_font, 50, "序列号：");
		int ret1 = DrawBarcode128(dc.m_hDC, 100, 200, 600, 100, "WHET201805020045", m_font, 50, "序列号：");
		int ret2 = DrawBarcode128(dc.m_hDC, 100, 350, 600, 100, "WHET201805020046", m_font, 50, "序列号：");
		int a = 100;
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strText;
	GetDlgItemText(IDC_EDIT1, strText);
	if (strText.GetLength() > 0) {
		MessageBox(strText);
	}

	CDialogEx::OnOK();
}
