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
	void SetStatus(int nStatus);                          // ���ù���״̬
	CDuiString  GetControlText(const char * szControlName);
	void ClearList();

	void SetControlText(const char * szControlName, CDuiString strText);

	virtual void    Notify(TNotifyUI& msg);

	void SetStatus(int nStatus, int nType);               // ����״̬��
};

// CMainDlg �Ի���

class CMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMainDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN };
#endif

protected:
	int           m_nStatus;
	CDuiFrameWnd1 m_duiFrame;
	HFONT         m_font;

	// ����С����˳������
	std::vector<TagItem *>   m_tag_items;                        // �Ѿ���ʾ������tags
	std::vector<TagItem *>   m_buf_tag_items;                    // ��ʱ������������û����ʾ��tags

	QRcode * m_qrcode;                                           // ��ά��
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
	LRESULT OnStatusMessage(WPARAM wParam, LPARAM lParam);          // ���ݿ�Ͷ�����״̬
	LRESULT OnQueryMessage(WPARAM wParam, LPARAM lParam);

	void DrawQrImg(CDC * pDc);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
};
