
// FeverSurveilDlg.h : ͷ�ļ�
//

#pragma once

#include "UIlib.h"
using namespace DuiLib;

class CDuiFrameWnd : public WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder() { return _T(""); }

	//CButtonUI* m_btnClose;

	virtual void     InitWindow() {
	}

	void ClearList();
	void ShowTagData(std::vector<TagData *> * pVec);
	int  GetSelectedHospital();

	void SetControlText(const char * szControlName, CDuiString strText);

	virtual void    Notify(TNotifyUI& msg);
};

// CFeverSurveilDlg �Ի���
class CFeverSurveilDlg : public CDialogEx
{
// ����
public:
	CFeverSurveilDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FEVERSURVEIL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;	
	ComStatus  m_eMainStatus;
	ComStatus  m_eSubStatus;
	BOOL       m_bPrepared;
	DbStatus   m_eDbStatus;
	CDuiFrameWnd m_duiFrame;

	LRESULT  OnSerialPortStatusMsg(WPARAM wParam, LPARAM lParam);
	LRESULT  OnSyncResultMsg(WPARAM wParam, LPARAM lParam);
	LRESULT  OnSyncMsg(WPARAM wParam, LPARAM lParam);
	LRESULT  OnDbStatusMsg(WPARAM wParam, LPARAM lParam);

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
