
// PrintBarcodeDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "qrencode.h"

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

	QRcode * m_qrcode;                                           // 二维码
	CBrush   m_brush;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edPaperWidth;
	CEdit m_edPaperHeight;
	CEdit m_edThreadBold;
	CEdit m_edText;
	afx_msg void OnBnClickedButton1();
};
