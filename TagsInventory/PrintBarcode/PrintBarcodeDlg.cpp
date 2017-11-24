
// PrintBarcodeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PrintBarcode.h"
#include "PrintBarcodeDlg.h"
#include "afxdialogex.h"

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