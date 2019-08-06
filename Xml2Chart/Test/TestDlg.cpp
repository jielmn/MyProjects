
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

#if 0
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

	SplitPen splitPen;
	splitPen.HPen = m_pen1.m_hObject;
	splitPen.nSplitNum = 3;

	pChild1->SetSplitLine(TRUE, splitPen);
	splitPen.nSplitNum = 4;
	pChild1->SetSplitLine(FALSE, splitPen);

#endif

	//m_XmlChartFile.ReadXmlChartFile("d:\\chart_1.xml");
	HRSRC hResource = ::FindResource(0, MAKEINTRESOURCE(IDR_XML1), "XML");
	if (hResource != NULL)
	{
		// 加载资源
		HGLOBAL hg = LoadResource(0, hResource);
		if (hg != NULL)
		{
			// 锁定资源
			LPVOID pData = LockResource(hg);
			if (pData != NULL)
			{
				// 获取资源大小
				DWORD dwSize = SizeofResource(0, hResource);
				m_XmlChartFile.ReadXmlChartStr( (const char *)pData, dwSize);
			}
		}
	}
	
	
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
		//DrawXml2ChartUI(dc.m_hDC, m_Xml2ChartUi);

		SetBkMode(dc, TRANSPARENT);
		DrawXml2ChartUI(dc.m_hDC, m_XmlChartFile.m_ChartUI);

		//CXml2ChartUI * p = m_XmlChartFile.FindChartUIByName("MainBlock");
		//if (p != 0) {
		//	RECT r1 = p->GetAbsoluteRect();
		//	int w = r1.right - r1.left;
		//	int h = r1.bottom - r1.top;

		//	int h1 = h / 40;
		//	int w1 = w / 42;

		//	RECT r2;
		//	r2.left = r1.left;
		//	r2.right = r2.left + w1;
		//	r2.top = r1.top + h1 * 39;
		//	r2.bottom = r1.bottom;

		//	::TextOut(dc.m_hDC, r2.left, r2.top, "1111", 4 );
		//}
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestDlg::OnMyPrint() {
	PRINTDLG printInfo;
	ZeroMemory(&printInfo, sizeof(printInfo));  //清空该结构     
	printInfo.lStructSize = sizeof(printInfo);
	printInfo.hwndOwner = 0;
	printInfo.hDevMode = 0;
	printInfo.hDevNames = 0;
	//这个是关键，PD_RETURNDC 如果不设这个标志，就拿不到hDC了      
	//            PD_RETURNDEFAULT 这个就是得到默认打印机，不需要弹设置对话框     
	//printInfo.Flags = PD_RETURNDC | PD_RETURNDEFAULT;   
	printInfo.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC;
	printInfo.nCopies = 1;
	printInfo.nFromPage = 0xFFFF;
	printInfo.nToPage = 0xFFFF;
	printInfo.nMinPage = 1;
	printInfo.nMaxPage = 0xFFFF;

	//调用API拿出默认打印机     
	//PrintDlg(&printInfo);
	if (PrintDlg(&printInfo) == TRUE)
	{
		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = _T("MyXPS");

		CDC *pDC = CDC::FromHandle(printInfo.hDC);
		pDC->SetMapMode(MM_ANISOTROPIC); //转换坐标映射方式

		CSize size = CSize(760, 1044);

		pDC->SetWindowExt(size);
		pDC->SetViewportExt(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

		StartDoc(printInfo.hDC, &di);

		for (int i = 0; i < 1; i++) {
			StartPage(printInfo.hDC);
			SetBkMode(printInfo.hDC, TRANSPARENT);
			DrawXml2ChartUI(printInfo.hDC, m_XmlChartFile.m_ChartUI);
			EndPage(printInfo.hDC);
		}

		EndDoc(printInfo.hDC);

		// Delete DC when done.
		DeleteDC(printInfo.hDC);

		//OnOK();
	}
}



void CTestDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	OnMyPrint();
}
