
// TestDlg.h : 头文件
//

#pragma once

#include "Xml2Chart.h"

// CTestDlg 对话框
class CTestDlg : public CDialogEx
{
// 构造
public:
	CTestDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CTestDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
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

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
