#pragma once
#include "UIlib.h"
#include "sigslot.h"
#include "LmnOdbc.h"
#include "InvOutCommon.h"
#include "MyProgress.h"

class CDuiFrameWndBuildCallback : public DuiLib::IDialogBuilderCallback
{
public:
	virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);

	DuiLib::CPaintManagerUI*     m_PaintManager;
};

class CDuiFrameWnd : public DuiLib::WindowImplBase, public sigslot::has_slots<>
{
public:
	CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("main.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void      InitWindow();
	virtual void      Notify(DuiLib::TNotifyUI& msg);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

	// �����Զ�����Ϣ
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	
	DuiLib::CLabelUI *           m_lblLoginUser;

	DuiLib::CTabLayoutUI *        m_tabs;
	DuiLib::CLabelUI *            m_lblDbStatus;
	DuiLib::CListUI *             m_lstAgencies;

	// �̵�
	DuiLib::COptionUI *           m_optSales;
	DuiLib::CEditUI *             m_edTarget;
	DuiLib::CListUI *             m_lstPackages;
	DuiLib::CLabelUI *            m_lblSmallCnt;
	DuiLib::CLabelUI *            m_lblBigCnt;
	DuiLib::CEditUI *             m_edPackageId;
	DuiLib::CButtonUI *           m_btnInvOk;

	// ��ѯ
	DuiLib::CTabLayoutUI *        m_tabs_query;

	// ״̬��
	CMyProgress *                 m_progress;

private:
	void  OnDbStatusChange(CLmnOdbc::DATABASE_STATUS eNewStatus);
	void  OnDbStatusChangeMsg(CLmnOdbc::DATABASE_STATUS eNewStatus);

	void  OnGetAllAgency(int, const std::vector<AgencyItem *> &);
	void  OnGetAllAgencyMsg(int, const std::vector<AgencyItem *> &);

	void  OnDeleteAgencyRet(int, DWORD);
	void  OnDeleteAgencyMsg(int, DWORD);

	void  OnTimerRet(DWORD);
	void  OnTimerMsg(DWORD);

	void  OnSaveInvOutRet(int);
	void  OnSaveInvOutMsg(int);

private:
	void  AddAgencyItem2List(DuiLib::CListTextElementUI* pListElement, AgencyItem * pItem, BOOL bSetTag = TRUE);
	void  OnAddAgency();
	void  OnModifyAgency();
	void  OnDeleteAgency();
	void  OnSelectTarget();

	// �̵��յ�char
	void OnInvOutChar(char ch);
	int  OnInvOutBarCode(const DuiLib::CDuiString & strBarCode);
	
	// ����̵��б�
	void OnClearInvList();
	// �������
	void OnAddPackage();
	// 
	void OnInvOk();

private:
	DuiLib::CDuiString                 m_strInvOutBuf;                      // �̵��ַ��Ļ���
	int                                m_nSmallCount;
	int                                m_nBigCount;

	CDuiFrameWndBuildCallback          m_Callback;
	BOOL                               m_bBusy;

	void  ClearInvOut();
	void  SetBusy( BOOL bBusy = TRUE );
};