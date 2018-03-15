#pragma once

// ���װ�̵�
class CDuiFrameWndP : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame2"); }
	virtual CDuiString GetSkinFile() {
		return "main.xml";
	}
	virtual CDuiString GetSkinFolder() { return _T(""); }

#ifndef _DEBUG
	virtual UILIB_RESOURCETYPE GetResourceType() const {
		return UILIB_ZIPRESOURCE;
	}

	virtual LPCTSTR GetResourceID() const {
		return MAKEINTRESOURCE(IDR_ZIPRES1);
	}
#endif
	

	virtual void   InitWindow();
};



// CMain1Dlg �Ի���

class CMain1Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMain1Dlg)

public:
	CMain1Dlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMain1Dlg();

	CDuiFrameWndP m_duiFrame;

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN1 };
#endif

protected:
	BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
