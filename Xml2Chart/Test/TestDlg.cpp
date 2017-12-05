
// TestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestDlg 对话框



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


// CTestDlg 消息处理程序

BOOL CTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CFont a;
	LOGFONT b;
	a.CreateFontIndirect( &b );

	LOGFONT  logfont;
	//hdc = BeginPaint(hwnd, &ps);  //前面声明了LOGFONT lonfont;  
	GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &logfont);
	logfont.lfItalic = true;
	logfont.lfItalic = true;//使用斜体  
	logfont.lfHeight = 16;//字体高度为16PX  
	logfont.lfWeight = 20;//字体宽度为20PX  
	logfont.lfCharSet = GB2312_CHARSET;//国标2312  
	sprintf_s(logfont.lfFaceName, TEXT("%s"), TEXT("宋体"));
	HFONT hfont = CreateFontIndirect(&logfont);//字体为宋体  




	// TODO: 在此添加额外的初始化代码
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
		//CDialogEx::OnPaint();
		CPaintDC dc(this);
		DrawXml2ChartUI(dc.m_hDC, m_Xml2ChartUi);
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

