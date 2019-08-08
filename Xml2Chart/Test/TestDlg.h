
// TestDlg.h : ͷ�ļ�
//

#pragma once

#include "Xml2Chart.h"

// CTestDlg �Ի���
class CTestDlg : public CDialogEx
{
// ����
public:
	CTestDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CTestDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	CXml2ChartUI *   m_Xml2ChartUi;

	CPen             m_pen1;
	CPen             m_pen2;

	CXml2ChartFile   m_XmlChartFile;
	int              m_nOriginalHeight;
	float            m_fConstRatio;
	float            m_fZoom;

	void  OnMyPrint();

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
