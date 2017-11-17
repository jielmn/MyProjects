
// MyEncryptToolDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#include "UIlib.h"
using namespace DuiLib;



class CDuiFrameWnd: public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile() { return _T("MyEncrypt.xml"); }
	virtual CDuiString GetSkinFolder() { return _T(""); }

	virtual UILIB_RESOURCETYPE GetResourceType() const {
		return UILIB_ZIPRESOURCE;
	}

	virtual LPCTSTR GetResourceID() const {
		return MAKEINTRESOURCE(IDR_ZIPRES1);
	}

	//CButtonUI* m_btnClose;

	virtual void   InitWindow() {
		CControlUI * pCtrl = 0;
#if	!(ENABLE_DECRYPT)
		pCtrl = m_PaintManager.FindControl(_T("dencrypt"));
		if (pCtrl) {
			pCtrl->SetEnabled(false);
		}
#endif		
	}

	virtual void    Notify(TNotifyUI& msg);

	BOOL  GetEncrypt() {
		COptionUI* pEncrypt = static_cast<COptionUI*>(m_PaintManager.FindControl(_T("encrypt")));
		if (pEncrypt && pEncrypt->IsSelected() ) {
			return TRUE;
		}
		return FALSE;
	}

protected:
	
};

// CMyEncryptToolDlg 对话框
class CMyEncryptToolDlg : public CDialogEx
{
// 构造
public:
	CMyEncryptToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYENCRYPTTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	

// 实现
protected:
	HICON m_hIcon;
	CDuiFrameWnd  m_duiFrame;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedButton1();
	//CEdit m_Src;
	//CEdit m_Dest;
	//CButton m_btnEncrypt;
};
