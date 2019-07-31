
// TestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestDlg �Ի���



CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Xml2ChartUi = new CXml2ChartUI;
}

CTestDlg::~CTestDlg() {
	delete m_Xml2ChartUi;
	m_Xml2ChartUi = 0;
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CTestDlg ��Ϣ�������

BOOL CTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

#if 0
	CFont a;
	LOGFONT b;
	a.CreateFontIndirect( &b );

	LOGFONT  logfont;
	//hdc = BeginPaint(hwnd, &ps);  //ǰ��������LOGFONT lonfont;  
	GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &logfont);
	logfont.lfItalic = true;
	logfont.lfItalic = true;//ʹ��б��  
	logfont.lfHeight = 16;//����߶�Ϊ16PX  
	logfont.lfWeight = 20;//������Ϊ20PX  
	logfont.lfCharSet = GB2312_CHARSET;//����2312  
	sprintf_s(logfont.lfFaceName, TEXT("%s"), TEXT("����"));
	HFONT hfont = CreateFontIndirect(&logfont);//����Ϊ����  



	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_pen1.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	m_pen2.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));

	m_Xml2ChartUi->SetBorderPen(m_pen2.m_hObject, DIRECTION_LEFT);
	m_Xml2ChartUi->SetBorderPen(m_pen2.m_hObject, DIRECTION_RIGHT);
	m_Xml2ChartUi->SetBorderPen(m_pen1.m_hObject, DIRECTION_TOP);
	m_Xml2ChartUi->SetBorderPen(m_pen1.m_hObject, DIRECTION_BOTTOM);

	m_Xml2ChartUi->SetText("Hello World!");
	m_Xml2ChartUi->SetAlignType(ALIGN_RIGHT);
	m_Xml2ChartUi->SetVAlignType(VALIGN_BOTTOM);

	RECT rect;
	rect.left = 0;
	rect.right = 150;
	rect.top = 0;
	rect.bottom = 150;
	m_Xml2ChartUi->SetRect( rect );
	m_Xml2ChartUi->SetFont(hfont);
	m_Xml2ChartUi->SetTextColor(RGB(0, 0, 255));


	CXml2ChartUI * pChild1 = new CXml2ChartUI;
	m_Xml2ChartUi->AddChild(pChild1);
	pChild1->SetText("123");
	pChild1->SetAlignType(ALIGN_LEFT);
	pChild1->SetVAlignType(VALIGN_TOP);
	pChild1->SetBorderPen(m_pen2.m_hObject, DIRECTION_BOTTOM);

	pChild1->SetChildrenLayout(LAYOUT_HORIZONTAL);
	CXml2ChartUI * pChild2 = new CXml2ChartUI;
	pChild1->AddChild(pChild2);
	pChild2->SetText("xyz");
	pChild2->SetBorderPen(m_pen2.m_hObject, DIRECTION_RIGHT);

	pChild2 = new CXml2ChartUI;
	pChild1->AddChild(pChild2);
	pChild2->SetText("xyz");
	pChild2->SetBorderPen(m_pen2.m_hObject, DIRECTION_RIGHT);

	pChild2 = new CXml2ChartUI;
	pChild1->AddChild(pChild2);
	pChild2->SetText("xyz");
	pChild2->SetBorderPen(m_pen2.m_hObject, DIRECTION_RIGHT);

	CXml2ChartUI * pChild3 = new CXml2ChartUI;
	pChild2->AddChild(pChild3);
	pChild3->SetText("OK");
	pChild3->SetBorderPen(m_pen2.m_hObject, DIRECTION_BOTTOM);

	pChild3 = new CXml2ChartUI;
	pChild2->AddChild(pChild3);
	pChild3->SetText("OK");
	pChild3->SetBorderPen(m_pen2.m_hObject, DIRECTION_BOTTOM);

	pChild3 = new CXml2ChartUI;
	pChild2->AddChild(pChild3);
	pChild3->SetText("OK");
	pChild3->SetBorderPen(m_pen2.m_hObject, DIRECTION_BOTTOM);

	pChild3 = new CXml2ChartUI;
	pChild2->AddChild(pChild3);
	pChild3->SetText("OK");
	pChild3->SetBorderPen(m_pen2.m_hObject, DIRECTION_BOTTOM);

	pChild3 = new CXml2ChartUI;
	pChild2->AddChild(pChild3);
	pChild3->SetText("OK");
	pChild3->SetBorderPen(m_pen2.m_hObject, DIRECTION_BOTTOM);




	pChild1 = new CXml2ChartUI;
	m_Xml2ChartUi->AddChild(pChild1);
	pChild1->SetText("123");
	pChild1->SetAlignType(ALIGN_LEFT);
	pChild1->SetVAlignType(VALIGN_TOP);
	pChild1->SetBorderPen(m_pen2.m_hObject, DIRECTION_BOTTOM);

	pChild1 = new CXml2ChartUI;
	m_Xml2ChartUi->AddChild(pChild1);
	pChild1->SetText("123");
	pChild1->SetAlignType(ALIGN_LEFT);
	pChild1->SetVAlignType(VALIGN_TOP);
	pChild1->SetBorderPen(m_pen2.m_hObject, DIRECTION_BOTTOM);

	SplitPen splitPen;
	splitPen.HPen = m_pen1.m_hObject;
	splitPen.nSplitNum = 3;

	pChild1->SetSplitLine(TRUE, splitPen);
	splitPen.nSplitNum = 4;
	pChild1->SetSplitLine(FALSE, splitPen);

#endif

	m_XmlChartFile.ReadXmlChartFile("d:\\chart_1.xml");

	
	
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
		//CDialogEx::OnPaint();
		CPaintDC dc(this);
		//DrawXml2ChartUI(dc.m_hDC, m_Xml2ChartUi);

		SetBkMode(dc, TRANSPARENT);
		DrawXml2ChartUI(dc.m_hDC, m_XmlChartFile.m_ChartUI);

		CXml2ChartUI * p = m_XmlChartFile.FindChartUIByName("MainBlock");
		if (p != 0) {
			RECT r1 = p->GetAbsoluteRect();
			int w = r1.right - r1.left;
			int h = r1.bottom - r1.top;

			int h1 = h / 40;
			int w1 = w / 42;

			RECT r2;
			r2.left = r1.left;
			r2.right = r2.left + w1;
			r2.top = r1.top + h1 * 39;
			r2.bottom = r1.bottom;

			::TextOut(dc.m_hDC, r2.left, r2.top, "1111", 4 );
		}
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

