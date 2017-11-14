#pragma once

class CDuiFrameWnd2 : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder() { return _T(""); }

	//CButtonUI* m_btnClose;

	virtual void       InitWindow()
	{
		
	}

	void SetStatus(int nStatus);
	void InsertItem(QueryResult * pItem);

	virtual void    Notify(TNotifyUI& msg);
};


// CQueryDlg 对话框

class CQueryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CQueryDlg)

public:
	CQueryDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CQueryDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_QUERY };
#endif

protected:
	CDuiFrameWnd2 m_duiFrame;

	BOOL OnInitDialog();
	LRESULT OnQueryMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnDbErrMessage(WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};
