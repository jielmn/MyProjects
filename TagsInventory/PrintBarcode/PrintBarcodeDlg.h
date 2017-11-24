
// PrintBarcodeDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "qrencode.h"


#include "UIlib.h"
using namespace DuiLib;

class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile() { return _T("PrintBarcode.xml"); }
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
	}

	virtual void    Notify(TNotifyUI& msg);

	void DrawQrImg(CDC * pDc, int nWidth, int nHeight, int nBold);

protected:
	QRcode * m_qrcode;                                           // ��ά��
	CBrush   m_brush;
};


// CPrintBarcodeDlg �Ի���
class CPrintBarcodeDlg : public CDialogEx
{
// ����
public:
	CPrintBarcodeDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRINTBARCODE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	void DrawQrImg(CDC * pDc, int nWidth, int nHeight, int nBold);

	CDuiFrameWnd  m_duiFrame;
	//QRcode * m_qrcode;                                           // ��ά��
	// CBrush   m_brush;

	// ���ɵ���Ϣӳ�亯��
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
