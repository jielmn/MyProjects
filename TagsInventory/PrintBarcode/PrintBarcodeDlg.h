
// PrintBarcodeDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "qrencode.h"


#include "UIlib.h"
using namespace DuiLib;

#include "LmnCommon.h"
#include "LmnConfig.h"

extern IConfig * g_cfg;

class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile() { 
#if BAR_CODE == 1 || BAR_CODE == 0
		return _T("PrintBarcode.xml");
#else
		return _T("PrintBarcode_1.xml");
#endif
	}
	virtual CDuiString GetSkinFolder() { return _T(""); }

#if 1
	virtual UILIB_RESOURCETYPE GetResourceType() const {
		return UILIB_ZIPRESOURCE;
	}

	virtual LPCTSTR GetResourceID() const {
		return MAKEINTRESOURCE(IDR_ZIPRES1);
	}
#endif

	//CButtonUI* m_btnClose;

	virtual void   InitWindow() {
		m_qrcode = 0;
		m_brush.CreateSolidBrush(RGB(0, 0, 0));

		CControlUI * pCtrl = 0;
		pCtrl = m_PaintManager.FindControl(_T("edWidth"));
		if (pCtrl) {
			pCtrl->SetText("300");
		}

		pCtrl = m_PaintManager.FindControl(_T("edHeight"));
		if (pCtrl) {
			pCtrl->SetText("320");
		}

		pCtrl = m_PaintManager.FindControl(_T("edBold"));
		if (pCtrl) {
			pCtrl->SetText("10");
		}

		DWORD dwFontSize = 0;
		g_cfg->GetConfig("font size", dwFontSize, 28);

		m_font = CreateFont(
			(int)dwFontSize,               // nHeight
			0,                             // nWidth
			0,                             // nEscapement
			0,                             // nOrientation
			FW_BOLD,                       // nWeight
			FALSE,                         // bItalic
			FALSE,                         // bUnderline
			0,                             // cStrikeOut
			ANSI_CHARSET,                  // nCharSet
			OUT_DEFAULT_PRECIS,            // nOutPrecision
			CLIP_DEFAULT_PRECIS,           // nClipPrecision
			DEFAULT_QUALITY,               // nQuality
			DEFAULT_PITCH | FF_SWISS,      // nPitchAndFamily
			_T("黑体")
		);

		g_cfg->GetConfig("big inv font size", dwFontSize, 80);
		m_font1 = CreateFont(
			(int)dwFontSize,               // nHeight
			0,                             // nWidth
			0,                             // nEscapement
			0,                             // nOrientation
			FW_NORMAL,                       // nWeight
			FALSE,                         // bItalic
			FALSE,                         // bUnderline
			0,                             // cStrikeOut
			ANSI_CHARSET,                  // nCharSet
			OUT_DEFAULT_PRECIS,            // nOutPrecision
			CLIP_DEFAULT_PRECIS,           // nClipPrecision
			DEFAULT_QUALITY,               // nQuality
			DEFAULT_PITCH | FF_SWISS,      // nPitchAndFamily
			_T("宋体")
		);

		char buf[8192];
		DuiLib::CDateTimeUI * dateValid = (DuiLib::CDateTimeUI *)m_PaintManager.FindControl(_T("date_2"));

		SYSTEMTIME t = dateValid->GetTime();
		t.wYear += 3;
		dateValid->SetTime(&t);
		SNPRINTF(buf, sizeof(buf), "%04d-%02d-%02d", t.wYear, t.wMonth, t.wDay);
		dateValid->SetText(buf);
	}

	virtual void    Notify(TNotifyUI& msg);

	void DrawQrImg(CDC * pDc, int nWidth, int nHeight, int nBold);
	void PrintInventorySmall();
	void PrintInventoryBig();
	void Print2DCode();

protected:
	QRcode * m_qrcode;                                           // 二维码
	CBrush   m_brush;

	HFONT                    m_font;	
	HFONT                    m_font1;
};


// CPrintBarcodeDlg 对话框
class CPrintBarcodeDlg : public CDialogEx
{
// 构造
public:
	CPrintBarcodeDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRINTBARCODE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	void DrawQrImg(CDC * pDc, int nWidth, int nHeight, int nBold);	

	CDuiFrameWnd  m_duiFrame;
	//QRcode * m_qrcode;                                           // 二维码
	// CBrush   m_brush;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//CEdit m_edPaperWidth;
	//CEdit m_edPaperHeight;
	//CEdit m_edThreadBold;
	//CEdit m_edText;
	//afx_msg void OnBnClickedButton1();
};
