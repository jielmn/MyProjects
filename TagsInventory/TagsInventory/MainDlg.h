#pragma once

#include "UIlib.h"
using namespace DuiLib;

#include "qrencode.h"
#include "QueryDlg.h"

class CDuiFrameWnd1 : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame1"); }
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder() { return _T(""); }

	virtual UILIB_RESOURCETYPE GetResourceType() const {
		return UILIB_ZIPRESOURCE;
	}

	virtual LPCTSTR GetResourceID() const {
		return MAKEINTRESOURCE(IDR_ZIPRES1);
	}

	//CButtonUI* m_btnClose;

	virtual void   InitWindow();
	void SaveBatchId();
	void InsertTag(TagItem * pItem, int nIndex);
	void SetCount(int nCount);
	void SetStatus(int nStatus);                          // 设置过程状态
	CDuiString  GetControlText(const char * szControlName);
	void ClearList();

	void SetControlText(const char * szControlName, CDuiString strText);

	virtual void    Notify(TNotifyUI& msg);

	void SetStatus(int nStatus, int nType);               // 设置状态栏
};

// CMainDlg 对话框

class CMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN };
#endif

protected:
	int           m_nStatus;
	CDuiFrameWnd1 m_duiFrame;
	HFONT         m_font;

	// 按从小到大顺序排列
	std::vector<TagItem *>   m_tag_items;                        // 已经显示出来的tags
	std::vector<TagItem *>   m_buf_tag_items;                    // 临时保存起来，还没有显示的tags

	QRcode * m_qrcode;                                           // 二维码
	CBrush   m_brush;
	
	CQueryDlg *  m_query_dlg;

	void  StartInventory();
	BOOL OnInitDialog();
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnTagItemMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnNewMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnSaveMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnTwoDimCodeMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnDbErrMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnStatusMessage(WPARAM wParam, LPARAM lParam);          // 数据库和读卡器状态
	LRESULT OnQueryMessage(WPARAM wParam, LPARAM lParam);

	void DrawQrImg(CDC * pDc);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};
