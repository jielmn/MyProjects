
// MyEncryptToolDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CMyEncryptToolDlg �Ի���
class CMyEncryptToolDlg : public CDialogEx
{
// ����
public:
	CMyEncryptToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYENCRYPTTOOL_DIALOG };
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
	afx_msg void OnBnClickedButton1();
	CEdit m_Src;
	CEdit m_Dest;
	CButton m_btnEncrypt;
};
