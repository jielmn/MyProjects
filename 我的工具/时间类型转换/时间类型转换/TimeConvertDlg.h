
// TimeConvertDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CTimeConvertDlg �Ի���
class CTimeConvertDlg : public CDialogEx
{
// ����
public:
	CTimeConvertDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edInt;
	CEdit m_edString;
	afx_msg void OnBnClickedButton2String();
	CStatic m_stError;
	afx_msg void OnBnClickedButton2Int();
};
