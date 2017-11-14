
// MyEncryptToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyEncryptTool.h"
#include "MyEncryptToolDlg.h"
#include "afxdialogex.h"
#include "MyEncrypt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyEncryptToolDlg �Ի���



CMyEncryptToolDlg::CMyEncryptToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MYENCRYPTTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyEncryptToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Src);
	DDX_Control(pDX, IDC_EDIT2, m_Dest);
	DDX_Control(pDX, IDC_RADIO1, m_btnEncrypt);
}

BEGIN_MESSAGE_MAP(CMyEncryptToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMyEncryptToolDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMyEncryptToolDlg ��Ϣ�������

BOOL CMyEncryptToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_btnEncrypt.SetCheck(BST_CHECKED);

#if ENABLE_DECRYPT
	GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
#endif


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyEncryptToolDlg::OnPaint()
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
HCURSOR CMyEncryptToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyEncryptToolDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int status = m_btnEncrypt.GetCheck();
	CString strText;
	char buf[8192];

	m_Src.GetWindowText(strText);
	// ����
	if (BST_CHECKED == status) {
		MyEncrypt(strText, strText.GetLength()+1, buf, sizeof(buf));
		m_Dest.SetWindowText(buf);
	}
	// ����
	else {
		DWORD dwLen = strText.GetLength();
		int ret = MyDecrypt(strText, buf, dwLen);
		if (0 == ret) {
			m_Dest.SetWindowText(buf);
		}
		else {
			MessageBox("failed to MyDecrypt!");
		}
	}
}
