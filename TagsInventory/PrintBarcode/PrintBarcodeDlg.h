
// PrintBarcodeDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "qrencode.h"

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

	QRcode * m_qrcode;                                           // ��ά��
	CBrush   m_brush;

	// ���ɵ���Ϣӳ�亯��
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
