
// TestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"
#include "afxdialogex.h"

#include "39BarCode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestDlg �Ի���



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
		_T("����")
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


// CTestDlg ��Ϣ�������

BOOL CTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTestDlg::OnPaint()
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
		CPaintDC  dc(this);
		CRect rect;
		GetClientRect(rect);

		::Rectangle(dc.m_hDC, 0, 0, rect.Width(), rect.Height());

		//DrawBarcode(dc.m_hDC, 100, 100, 600, 100, "WHET201805020045", m_font, 50, "���кţ�");
		int ret  = DrawBarcode128(dc.m_hDC, 100,  50, 600, 100, "WHET201805020044", m_font, 50, "���кţ�");
		int ret1 = DrawBarcode128(dc.m_hDC, 100, 200, 600, 100, "WHET201805020045", m_font, 50, "���кţ�");
		int ret2 = DrawBarcode128(dc.m_hDC, 100, 350, 600, 100, "WHET201805020046", m_font, 50, "���кţ�");
		int a = 100;
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strText;
	GetDlgItemText(IDC_EDIT1, strText);
	if (strText.GetLength() > 0) {
		MessageBox(strText);
	}

	CDialogEx::OnOK();
}
