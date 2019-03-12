
// TimeConvertDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ʱ������ת��.h"
#include "TimeConvertDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTimeConvertDlg �Ի���



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


// CTimeConvertDlg ��Ϣ�������

BOOL CTimeConvertDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_stError.SetWindowText("");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTimeConvertDlg::OnPaint()
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
HCURSOR CTimeConvertDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTimeConvertDlg::OnBnClickedButton2String()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString  strText;
	m_edInt.GetWindowText(strText);

	if ( strText.GetLength() == 0 ) {
		m_stError.SetWindowText("����������");
		return;
	}

	if (strText.GetLength() > 10 ) {
		m_stError.SetWindowText("����10���ַ�");
		return;
	}

	time_t  ts = 0;
	int ret = sscanf(strText, "%llu", &ts );
	if ( ret <= 0 ) {
		m_stError.SetWindowText("����������");
		return;
	}

	char buf[8192];
	Date2String( buf, sizeof(buf), &ts );

	m_edString.SetWindowText(buf);
	m_stError.SetWindowText("");
}


void CTimeConvertDlg::OnBnClickedButton2Int()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString  strText;
	m_edString.GetWindowText(strText);

	if (strText.GetLength() == 0) {
		m_stError.SetWindowText("�������ַ���");
		return;
	}

	if (strText.GetLength() != 19) {
		m_stError.SetWindowText("�������ʽ��yyyy-mm-dd hh:mm:ss");
		return;
	}

	time_t ts = String2Date(strText);
	strText.Format("%llu", ts);

	m_edInt.SetWindowText(strText);
	m_stError.SetWindowText("");
}
