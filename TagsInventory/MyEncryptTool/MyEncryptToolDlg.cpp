
// MyEncryptToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyEncryptTool.h"
#include "MyEncryptToolDlg.h"
#include "afxdialogex.h"
#include "MyEncrypt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyEncryptToolDlg 对话框
void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	char buf[8192];

	if (msg.sType == _T("click"))
	{
		CDuiString sendName = msg.pSender->GetName();
		// 确定按钮
		if (sendName == _T("btnOk"))
		{
			BOOL bEncrpyt = GetEncrypt();
			CDuiString sText = m_PaintManager.FindControl(_T("edText"))->GetText();

			if (bEncrpyt) {
				MyEncrypt(sText, sText.GetLength() + 1, buf, sizeof(buf));
				m_PaintManager.FindControl(_T("edResult"))->SetText(buf);
			}
			else {
				DWORD dwLen = sText.GetLength();
				int ret = MyDecrypt(sText, buf, dwLen);
				if (0 == ret&&buf[dwLen-1] == '\0') {
					m_PaintManager.FindControl(_T("edResult"))->SetText(buf);
				}
				else {
					::MessageBox(m_hWnd, "failed to MyDecrypt!", "错误", 0);
				}
			}
		}
	}
}


CMyEncryptToolDlg::CMyEncryptToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MYENCRYPTTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyEncryptToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT1, m_Src);
	//DDX_Control(pDX, IDC_EDIT2, m_Dest);
	//DDX_Control(pDX, IDC_RADIO1, m_btnEncrypt);
}

BEGIN_MESSAGE_MAP(CMyEncryptToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BUTTON1, &CMyEncryptToolDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMyEncryptToolDlg 消息处理程序

BOOL CMyEncryptToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
//	m_btnEncrypt.SetCheck(BST_CHECKED);
//
//#if ENABLE_DECRYPT
//	GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
//#endif

	CRect rect;
	GetClientRect(rect);
	m_duiFrame.Create(*this, _T("DUIWnd"), UI_WNDSTYLE_CHILD, 0, 0, 0, rect.Width(), rect.Height());
	m_duiFrame.ShowWindow(TRUE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyEncryptToolDlg::OnPaint()
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
HCURSOR CMyEncryptToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//void CMyEncryptToolDlg::OnBnClickedButton1()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	int status = m_btnEncrypt.GetCheck();
//	CString strText;
//	char buf[8192];
//
//	m_Src.GetWindowText(strText);
//	// 加密
//	if (BST_CHECKED == status) {
//		MyEncrypt(strText, strText.GetLength()+1, buf, sizeof(buf));
//		m_Dest.SetWindowText(buf);
//	}
//	// 解密
//	else {
//		DWORD dwLen = strText.GetLength();
//		int ret = MyDecrypt(strText, buf, dwLen);
//		if (0 == ret) {
//			m_Dest.SetWindowText(buf);
//		}
//		else {
//			MessageBox("failed to MyDecrypt!");
//		}
//	}
//}
