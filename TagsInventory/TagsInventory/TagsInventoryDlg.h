
// TagsInventoryDlg.h : ͷ�ļ�
//

#pragma once

#include "UIlib.h"
using namespace DuiLib;

class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }

	virtual CDuiString GetSkinFile() {
		return "login.xml";
	}

	virtual CDuiString GetSkinFolder() {
		return _T("");
	}

#ifndef _DEBUG
	virtual UILIB_RESOURCETYPE GetResourceType() const {
		return UILIB_ZIPRESOURCE;
	}

	virtual LPCTSTR GetResourceID() const {
		return MAKEINTRESOURCE(IDR_ZIPRES1);
	}
#endif

	//CButtonUI* m_btnClose;

	virtual void       InitWindow()
	{
		//m_btnClose = static_cast <CButtonUI*> (m_PaintManager.FindControl(_T("btnHello")));
		//ASSERT(m_btnClose != NULL);
	}

	void SetStatus(int nStatus, int nType);

	virtual void    Notify(TNotifyUI& msg);
};

// CTagsInventoryDlg �Ի���
class CTagsInventoryDlg : public CDialogEx
{
// ����
public:
	CTagsInventoryDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAGSINVENTORY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CDuiFrameWnd m_duiFrame;

	LRESULT OnStatusMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnTagItemMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnVerifyResultMessage(WPARAM wParam, LPARAM lParam);

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
