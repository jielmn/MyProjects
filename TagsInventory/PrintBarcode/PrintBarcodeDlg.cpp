
// PrintBarcodeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PrintBarcode.h"
#include "PrintBarcodeDlg.h"
#include "afxdialogex.h"
#include "39BarCode.h"
#include "LmnString.h"

#if BAR_CODE == 1
#include "39BarCode.h"
#endif



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



void CDuiFrameWnd::DrawQrImg(CDC * pDc, int nWidth, int nHeight, int nBold) {

	DWORD dwPaperXOffset = 0;
	DWORD dwPaperYOffset = 0;

	int nOffsetX = 0, nOffsetY = 0;

	nOffsetX = (int)(nWidth - nBold * 25) / 2;
	nOffsetY = (int)(nHeight - nBold * 25) / 2;

	nOffsetX += (int)dwPaperXOffset;
	nOffsetY += (int)dwPaperYOffset;

	if (0 == m_qrcode) {
		return;
	}

	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			if (m_qrcode->data[i * 25 + j] & 0x01)
			{
				CRect rect(i*nBold + nOffsetX, j*nBold + nOffsetY, (i + 1)*nBold + nOffsetX, (j + 1)*nBold + nOffsetY);
				pDc->FillRect(rect, &m_brush);
			}
		}
	}

}


void CDuiFrameWnd::Notify(TNotifyUI& msg) {
	CDuiString sendName = msg.pSender->GetName();

	if (msg.sType == _T("click"))
	{	
		// 打印按钮
		if (sendName == _T("btnPrint"))
		{
#if BAR_CODE == 1
			PrintInventorySmall();
#elif BAR_CODE == 0
			Print2DCode();
#else
			PrintInventoryBig();
#endif
		}
	}
}

// 打印大包装条码
void CDuiFrameWnd::PrintInventoryBig() {
	DWORD   dwPaperWidth = 0;
	DWORD   dwPaperLength = 0;
	DWORD   dwLeft = 0;
	DWORD   dwTop = 0;
	DWORD   dwPrintWidth = 0;
	DWORD   dwPrintHeight = 0;
	DWORD   dwTextHeight = 0;
	DWORD   dwInterval = 0;
	char buf[8192];

	char  szFactoryId[64] = { 0 };
	g_cfg->GetConfig(FACTORY_CODE, szFactoryId, sizeof(szFactoryId), "WH");

	char  szProductId[64] = { 0 };
	g_cfg->GetConfig(PRODUCT_CODE, szProductId, sizeof(szProductId), "ET");

	DWORD  dwFactoryLen = strlen(szFactoryId);
	DWORD  dwProductLen = strlen(szProductId);

	g_cfg->Reload();
	g_cfg->GetConfig("big inv paper width 1", dwPaperWidth, 720);
	g_cfg->GetConfig("big inv paper height 1", dwPaperLength, 1200);
	g_cfg->GetConfig("big inv print left 1", dwLeft, 40);
	g_cfg->GetConfig("big inv print top 1", dwTop, 120);
	g_cfg->GetConfig("big inv print barcode width 1", dwPrintWidth, 600);
	g_cfg->GetConfig("big inv print barcode height 1", dwPrintHeight, 130);
	g_cfg->GetConfig("big inv bar code text height 1", dwTextHeight, 80);
	g_cfg->GetConfig("big inv print vertical interval 1", dwInterval, 130);

	DuiLib::CEditUI *  edtBigPackageId = (DuiLib::CEditUI *)m_PaintManager.FindControl(_T("edString"));
	DuiLib::CDuiString strBigPackageId = GET_CONTROL_TEXT(edtBigPackageId);
	if (strBigPackageId.GetLength() != dwFactoryLen + dwProductLen + 12) {
		SNPRINTF(buf, sizeof(buf), "格式不对或者不完整。正确格式例如：%s%s20180603A004", szFactoryId, szProductId);
		MessageBox(GetHWND(), buf, "错误", 0);
		return;
	}

	DuiLib::CDuiString strBigBatchId = strBigPackageId.Mid(dwFactoryLen + dwProductLen, 8);

	DuiLib::CDateTimeUI * dateProc  = (DuiLib::CDateTimeUI *)m_PaintManager.FindControl(_T("date_1"));
	DuiLib::CDateTimeUI * dateValid = (DuiLib::CDateTimeUI *)m_PaintManager.FindControl(_T("date_2"));

	PRINTDLG printInfo;
	ZeroMemory(&printInfo, sizeof(printInfo));  //清空该结构     
	printInfo.lStructSize = sizeof(printInfo);
	printInfo.hwndOwner = 0;
	printInfo.hDevMode = 0;
	printInfo.hDevNames = 0;
	//这个是关键，PD_RETURNDC 如果不设这个标志，就拿不到hDC了      
	//            PD_RETURNDEFAULT 这个就是得到默认打印机，不需要弹设置对话框     
	//printInfo.Flags = PD_RETURNDC | PD_RETURNDEFAULT;   
	printInfo.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_RETURNDEFAULT;
	printInfo.nCopies = 1;
	printInfo.nFromPage = 0xFFFF;
	printInfo.nToPage = 0xFFFF;
	printInfo.nMinPage = 1;
	printInfo.nMaxPage = 0xFFFF;

	//调用API拿出默认打印机     
	PrintDlg(&printInfo);
	//if (PrintDlg(&printInfo)==TRUE) 
	{
		LPDEVMODE lpDevMode = (LPDEVMODE)::GlobalLock(printInfo.hDevMode);

		if (lpDevMode) {
			lpDevMode->dmPaperSize = DMPAPER_USER;
			lpDevMode->dmFields = lpDevMode->dmFields | DM_PAPERSIZE | DM_PAPERLENGTH | DM_PAPERWIDTH;
			lpDevMode->dmPaperWidth = (short)dwPaperWidth;
			lpDevMode->dmPaperLength = (short)dwPaperLength;
			lpDevMode->dmOrientation = DMORIENT_LANDSCAPE; // DMORIENT_PORTRAIT;
		}
		GlobalUnlock(printInfo.hDevMode);
		ResetDC(printInfo.hDC, lpDevMode);

		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = _T("MyXPS");
		StartDoc(printInfo.hDC, &di);
		StartPage(printInfo.hDC);

		CDC *pDC = CDC::FromHandle(printInfo.hDC);
		pDC->SetMapMode(MM_ANISOTROPIC); //转换坐标映射方式

		CSize size = CSize(dwPaperWidth, dwPaperLength);
		pDC->SetWindowExt(size);
		pDC->SetViewportExt(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));


		SYSTEMTIME t1 = dateProc->GetTime();
		SYSTEMTIME t2 = dateValid->GetTime();



		DWORD  dwOffsetY = 0;
		DuiLib::CDuiString  strText;
		DuiLib::CDuiString  strFullBatchId = szProductId;
		strFullBatchId += strBigBatchId;

		::SelectObject(pDC->m_hDC, m_font1);

		strText = "产品名称：无线电子体温计测温标签";
		::TextOut(pDC->m_hDC, dwLeft, dwTop + dwOffsetY, (const char *)strText, strText.GetLength());
		dwOffsetY += dwInterval;

		strText = "产品型号：EASYTEMP";
		::TextOut(pDC->m_hDC, dwLeft, dwTop + dwOffsetY, (const char *)strText, strText.GetLength());
		dwOffsetY += dwInterval;

		strText = "产品规格：200mm×75mm";
		::TextOut(pDC->m_hDC, dwLeft, dwTop + dwOffsetY, (const char *)strText, strText.GetLength());
		dwOffsetY += dwInterval;

		strText.Format("产品批号：%s", strFullBatchId);
		::TextOut(pDC->m_hDC, dwLeft, dwTop + dwOffsetY, (const char *)strText, strText.GetLength());
		dwOffsetY += dwInterval;

		SNPRINTF(buf, sizeof(buf), "%04lu年%02lu月%02lu日", (DWORD)t1.wYear, (DWORD)t1.wMonth, (DWORD)t1.wDay);
		strText.Format("生产日期：%s", buf);
		::TextOut(pDC->m_hDC, dwLeft, dwTop + dwOffsetY, (const char *)strText, strText.GetLength());
		dwOffsetY += dwInterval;

		SNPRINTF(buf, sizeof(buf), "%04lu年%02lu月%02lu日", (DWORD)t2.wYear, (DWORD)t2.wMonth, (DWORD)t2.wDay);
		strText.Format("有效期至：%s", buf);
		::TextOut(pDC->m_hDC, dwLeft, dwTop + dwOffsetY, (const char *)strText, strText.GetLength());
		dwOffsetY += dwInterval;

		DrawBarcode128(pDC->m_hDC, dwLeft, dwTop + dwOffsetY, dwPrintWidth, dwPrintHeight, (const char *)strBigPackageId, m_font, dwTextHeight, "S/N:");


		EndPage(printInfo.hDC);
		EndDoc(printInfo.hDC);

		// Delete DC when done.
		DeleteDC(printInfo.hDC);
	}
}

// 打印条码
void  CDuiFrameWnd::PrintInventorySmall() {

	DWORD   dwPaperWidth = 0;
	DWORD   dwPaperLength = 0;
	DWORD   dwLeft = 0;
	DWORD   dwTop = 0;
	DWORD   dwPrintWidth = 0;
	DWORD   dwPrintHeight = 0;
	DWORD   dwTextHeight = 0;

	g_cfg->Reload();
	g_cfg->GetConfig("paper width", dwPaperWidth, 600);
	g_cfg->GetConfig("paper length", dwPaperLength, 170);
	g_cfg->GetConfig("paper left", dwLeft, 0);
	g_cfg->GetConfig("paper top", dwTop, 0);
	g_cfg->GetConfig("print width", dwPrintWidth, dwPaperWidth);
	g_cfg->GetConfig("print height", dwPrintHeight, dwPaperLength);
	g_cfg->GetConfig("bar code text height", dwTextHeight, 50);


	PRINTDLG printInfo;
	ZeroMemory(&printInfo, sizeof(printInfo));  //清空该结构     
	printInfo.lStructSize = sizeof(printInfo);
	printInfo.hwndOwner = 0;
	printInfo.hDevMode = 0;
	printInfo.hDevNames = 0;
	//这个是关键，PD_RETURNDC 如果不设这个标志，就拿不到hDC了      
	//            PD_RETURNDEFAULT 这个就是得到默认打印机，不需要弹设置对话框     
	//printInfo.Flags = PD_RETURNDC | PD_RETURNDEFAULT;   
	printInfo.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_RETURNDEFAULT;
	printInfo.nCopies = 1;
	printInfo.nFromPage = 0xFFFF;
	printInfo.nToPage = 0xFFFF;
	printInfo.nMinPage = 1;
	printInfo.nMaxPage = 0xFFFF;

	//调用API拿出默认打印机     
	PrintDlg(&printInfo);
	//if (PrintDlg(&printInfo)==TRUE) 
	{
		LPDEVMODE lpDevMode = (LPDEVMODE)::GlobalLock(printInfo.hDevMode);

		if (lpDevMode) {
			lpDevMode->dmPaperSize = DMPAPER_USER;
			lpDevMode->dmFields = lpDevMode->dmFields | DM_PAPERSIZE | DM_PAPERLENGTH | DM_PAPERWIDTH;
			lpDevMode->dmPaperWidth = (short)dwPaperWidth;
			lpDevMode->dmPaperLength = (short)dwPaperLength;
			lpDevMode->dmOrientation = DMORIENT_PORTRAIT;
		}
		GlobalUnlock(printInfo.hDevMode);
		ResetDC(printInfo.hDC, lpDevMode);

		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = _T("MyXPS");
		StartDoc(printInfo.hDC, &di);
		StartPage(printInfo.hDC);

		CDC *pDC = CDC::FromHandle(printInfo.hDC);
		pDC->SetMapMode(MM_ANISOTROPIC); //转换坐标映射方式

		DWORD dwMultiple = 0;
		g_cfg->GetConfig("multiple", dwMultiple);

		CSize size = CSize(dwPaperWidth, dwPaperLength);
		pDC->SetWindowExt(size);
		pDC->SetViewportExt(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

		CDuiString  strBatchId;
		CControlUI * pCtrl = 0;
		pCtrl = m_PaintManager.FindControl(_T("edString"));
		if (pCtrl) {
			strBatchId = pCtrl->GetText();
		}

		char buf[8192];
		Str2Upper((const char*)strBatchId, buf, sizeof(buf));

		// 画条码
		DrawBarcode128(pDC->m_hDC, dwLeft, dwTop, dwPrintWidth, dwPrintHeight, buf, m_font, dwTextHeight, "S/N:");

		EndPage(printInfo.hDC);
		EndDoc(printInfo.hDC);

		// Delete DC when done.
		DeleteDC(printInfo.hDC);
	}
}

void CDuiFrameWnd::Print2DCode() {
	CDuiString  strText;
	CDuiString  strString;

	CControlUI * pCtrl = 0;
	pCtrl = m_PaintManager.FindControl(_T("edString"));
	if (pCtrl) {
		strString = pCtrl->GetText();
	}

	if (strString.GetLength() == 0) {
		::MessageBox(m_hWnd, "请输入要编入的字符串！", "错误", 0);
		return;
	}

	int nWidth = 0;
	int nHeight = 0;
	int nBold = 0;

	// width
	pCtrl = m_PaintManager.FindControl(_T("edWidth"));
	if (pCtrl) {
		strText = pCtrl->GetText();
	}
	sscanf_s((const char *)strText, " %d", &nWidth);

	// height
	pCtrl = m_PaintManager.FindControl(_T("edHeight"));
	if (pCtrl) {
		strText = pCtrl->GetText();
	}
	sscanf_s((const char *)strText, " %d", &nHeight);

	// bold
	pCtrl = m_PaintManager.FindControl(_T("edBold"));
	if (pCtrl) {
		strText = pCtrl->GetText();
	}
	sscanf_s((const char *)strText, " %d", &nBold);

	if (nWidth <= 0 || nHeight <= 0 || nBold <= 0) {
		::MessageBox(m_hWnd, "参数有误！", "错误", 0);
		return;
	}

	if (m_qrcode) {
		QRcode_free(m_qrcode);
		m_qrcode = 0;
	}
	m_qrcode = QRcode_encodeString((const char *)strString, 2, QR_ECLEVEL_L, QR_MODE_8, 0);

	PRINTDLG printInfo;
	ZeroMemory(&printInfo, sizeof(printInfo));  //清空该结构     
	printInfo.lStructSize = sizeof(printInfo);
	printInfo.hwndOwner = 0;
	printInfo.hDevMode = 0;
	printInfo.hDevNames = 0;
	//这个是关键，PD_RETURNDC 如果不设这个标志，就拿不到hDC了      
	//            PD_RETURNDEFAULT 这个就是得到默认打印机，不需要弹设置对话框     
	//printInfo.Flags = PD_RETURNDC | PD_RETURNDEFAULT;   
	printInfo.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_RETURNDEFAULT;
	printInfo.nCopies = 1;
	printInfo.nFromPage = 0xFFFF;
	printInfo.nToPage = 0xFFFF;
	printInfo.nMinPage = 1;
	printInfo.nMaxPage = 0xFFFF;

	//调用API拿出默认打印机     
	PrintDlg(&printInfo);
	//if (PrintDlg(&printInfo)==TRUE) 
	{
		LPDEVMODE lpDevMode = (LPDEVMODE)::GlobalLock(printInfo.hDevMode);

		if (lpDevMode) {
			lpDevMode->dmPaperSize = DMPAPER_USER;
			lpDevMode->dmFields = lpDevMode->dmFields | DM_PAPERSIZE | DM_PAPERLENGTH | DM_PAPERWIDTH;
			lpDevMode->dmPaperWidth = (short)nWidth;
			lpDevMode->dmPaperLength = (short)nHeight;
			lpDevMode->dmOrientation = DMORIENT_PORTRAIT;
		}
		GlobalUnlock(printInfo.hDevMode);
		ResetDC(printInfo.hDC, lpDevMode);

		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = _T("MyXPS");
		StartDoc(printInfo.hDC, &di);
		StartPage(printInfo.hDC);

		CDC *pDC = CDC::FromHandle(printInfo.hDC);
		pDC->SetMapMode(MM_ANISOTROPIC); //转换坐标映射方式

		CSize size = CSize(nWidth, nHeight);
		pDC->SetWindowExt(size);
		pDC->SetViewportExt(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

		DrawQrImg(pDC, nWidth, nHeight, nBold);

		EndPage(printInfo.hDC);
		EndDoc(printInfo.hDC);

		// Delete DC when done.
		DeleteDC(printInfo.hDC);

	}
}




// CPrintBarcodeDlg 对话框



CPrintBarcodeDlg::CPrintBarcodeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PRINTBARCODE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPrintBarcodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_EDIT1, m_edPaperWidth);
	//DDX_Control(pDX, IDC_EDIT2, m_edPaperHeight);
	//DDX_Control(pDX, IDC_EDIT3, m_edThreadBold);
	//DDX_Control(pDX, IDC_EDIT4, m_edText);
}

BEGIN_MESSAGE_MAP(CPrintBarcodeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BUTTON1, &CPrintBarcodeDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CPrintBarcodeDlg 消息处理程序

BOOL CPrintBarcodeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//m_brush.CreateSolidBrush(RGB(0, 0, 0));

	//m_edPaperWidth.SetWindowText("300");
	//m_edPaperHeight.SetWindowText("320");
	//m_edThreadBold.SetWindowText("10");


	CRect rect;
	GetClientRect(rect);
	m_duiFrame.Create(*this, _T("DUIWnd"), UI_WNDSTYLE_CHILD, 0, 0, 0, rect.Width(), rect.Height());
	m_duiFrame.ShowWindow(TRUE);	


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPrintBarcodeDlg::OnPaint()
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
HCURSOR CPrintBarcodeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*
void CPrintBarcodeDlg::DrawQrImg(CDC * pDc, int nWidth, int nHeight, int nBold) {

	DWORD dwPaperXOffset = 0;
	DWORD dwPaperYOffset = 0;

	int nOffsetX = 0, nOffsetY = 0;

	nOffsetX = (int)(nWidth - nBold * 25) / 2;
	nOffsetY = (int)(nHeight - nBold * 25) / 2;

	nOffsetX += (int)dwPaperXOffset;
	nOffsetY += (int)dwPaperYOffset;

	if (0 == m_qrcode) {
		return;
	}

	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			if (m_qrcode->data[i * 25 + j] & 0x01)
			{
				CRect rect(i*nBold + nOffsetX, j*nBold + nOffsetY, (i + 1)*nBold + nOffsetX, (j + 1)*nBold + nOffsetY);
				pDc->FillRect(rect, &m_brush);
			}
		}
	}

}
*/

/*
void CPrintBarcodeDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  strText;
	m_edText.GetWindowText(strText);
	if (strText.GetLength() == 0) {
		MessageBox("请输入要编入的字符串！");
		return;
	}

	CString  strTmp;
	char buf[8192];
	int nWidth = 0;
	int nHeight = 0;
	int nBold = 0;

	m_edPaperWidth.GetWindowText(buf, sizeof(buf));
	sscanf_s(buf, " %d", &nWidth);

	m_edPaperHeight.GetWindowText(buf, sizeof(buf));
	sscanf_s(buf, " %d", &nHeight);

	m_edThreadBold.GetWindowText(buf, sizeof(buf));
	sscanf_s(buf, " %d", &nBold);

	if (nWidth <= 0 || nHeight <= 0 || nBold <= 0) {
		MessageBox("参数有误！");
		return;
	}

	if (m_qrcode) {
		QRcode_free(m_qrcode);
		m_qrcode = 0;
	}
	m_qrcode = QRcode_encodeString(strText, 2, QR_ECLEVEL_L, QR_MODE_8, 0);



	PRINTDLG printInfo;
	ZeroMemory(&printInfo, sizeof(printInfo));  //清空该结构     
	printInfo.lStructSize = sizeof(printInfo);
	printInfo.hwndOwner = 0;
	printInfo.hDevMode = 0;
	printInfo.hDevNames = 0;
	//这个是关键，PD_RETURNDC 如果不设这个标志，就拿不到hDC了      
	//            PD_RETURNDEFAULT 这个就是得到默认打印机，不需要弹设置对话框     
	//printInfo.Flags = PD_RETURNDC | PD_RETURNDEFAULT;   
	printInfo.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_RETURNDEFAULT;
	printInfo.nCopies = 1;
	printInfo.nFromPage = 0xFFFF;
	printInfo.nToPage = 0xFFFF;
	printInfo.nMinPage = 1;
	printInfo.nMaxPage = 0xFFFF;

	//调用API拿出默认打印机     
	PrintDlg(&printInfo);
	//if (PrintDlg(&printInfo)==TRUE) 
	{
		LPDEVMODE lpDevMode = (LPDEVMODE)::GlobalLock(printInfo.hDevMode);

		if (lpDevMode) {
			lpDevMode->dmPaperSize = DMPAPER_USER;
			lpDevMode->dmFields = lpDevMode->dmFields | DM_PAPERSIZE | DM_PAPERLENGTH | DM_PAPERWIDTH;
			lpDevMode->dmPaperWidth = (short)nWidth;
			lpDevMode->dmPaperLength = (short)nHeight;
			lpDevMode->dmOrientation = DMORIENT_PORTRAIT;
		}
		GlobalUnlock(printInfo.hDevMode);
		ResetDC(printInfo.hDC, lpDevMode);

		DOCINFO di;
		ZeroMemory(&di, sizeof(DOCINFO));
		di.cbSize = sizeof(DOCINFO);
		di.lpszDocName = _T("MyXPS");
		StartDoc(printInfo.hDC, &di);
		StartPage(printInfo.hDC);

		CDC *pDC = CDC::FromHandle(printInfo.hDC);
		pDC->SetMapMode(MM_ANISOTROPIC); //转换坐标映射方式

		CSize size = CSize(nWidth, nHeight);
		pDC->SetWindowExt(size);
		pDC->SetViewportExt(pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

		DrawQrImg(pDC, nWidth, nHeight, nBold);

		EndPage(printInfo.hDC);
		EndDoc(printInfo.hDC);

		// Delete DC when done.
		DeleteDC(printInfo.hDC);

		//OnOK();
	}
}
*/