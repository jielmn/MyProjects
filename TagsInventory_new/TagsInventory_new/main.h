#pragma once
#include "InvCommon.h"
#include "UIlib.h"
//using namespace DuiLib;

class CDuiFrameWnd : public DuiLib::WindowImplBase
{
public:
	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual DuiLib::CDuiString GetSkinFile()              { return _T("main.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder()            { return _T("res"); }	

	virtual void      InitWindow();
	virtual void      Notify(DuiLib::TNotifyUI& msg);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

	void  InitInventorySmall();
	void  StartInventorySmall();
	void  StopInventorySmall();
	void  SaveInventorySmall();
	void  PrintInventorySmall();

	void  InitInventoryBig();
	void  StartInventoryBig();
	void  StopInventoryBig();
	void  SaveInventoryBig();
	void  PrintInventoryBig();
	
	// TEST
	void  OnTest();

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnInventorySmallRet( const TagItem * pItem );
	void OnInventoryCheckRet(const TagItem * pItem);

	// ���̵��յ�char
	void OnInvBigChar(char ch);
	// ���̵��յ�����
	int OnInvBigBarCode(const CString & strBarCode );
	// �ֶ��������
	void OnManualAddBarcode();
	// ��ѯ(��С��װ)
	void OnQuery();
	// ��ѯ���
	void OnQueryResult(const QueryResult * pRet);
	// ���ѯ��listitem�ı�
	void OnQueryBigListChanged();
	// С��ѯ��listitem�ı�
	void OnQuerySmallListChanged();
	// ��ѯС��װ���
	void OnQuerySmallResult(int nError, const std::vector<QuerySmallResultItem*> * pVec);
	// ��ѯ���װ���
	void OnQueryBigResult(int nError, const std::vector<QueryResultItem*> * pVec);

	enum  INVENTORY_STATUS {
		STATUS_CLOSE = 0,
		STATUS_START,
		STATUS_STOP,
		STATUS_SAVING,
		STATUS_SAVED,
	};

private:

	DuiLib::CLabelUI *       m_lblDbStatus;
	DuiLib::CLabelUI *       m_lblReaderStatus;
	DuiLib::CLabelUI *       m_lblUser;
	DuiLib::CTabLayoutUI *   m_tabs;

	DuiLib::CLabelUI *       m_lblInvSmallSaveRet;
	DuiLib::CLabelUI *       m_lblCountSmall;
	DuiLib::CEditUI *        m_edtBatchId;
	DuiLib::CEditUI *        m_edtPackageId;
	DuiLib::CButtonUI *      m_btnStartSmall;
	DuiLib::CButtonUI *      m_btnStopSmall;
	DuiLib::CButtonUI *      m_btnSaveSmall;
	DuiLib::CButtonUI *      m_btnPrintSmall;
	INVENTORY_STATUS         m_InventorySmallStatus;
	std::vector<TagItem *>   m_vInventorySmall;

	HFONT                    m_font;
	HFONT                    m_font1;
	HFONT                    m_font_small;

	// check
	DuiLib::CEditUI *        m_lblCheckTagId;
	DuiLib::CLabelUI *       m_lblCheckTagRet;
	DuiLib::CLabelUI *       m_lblCheckCount;

	// ���װ
	DuiLib::CButtonUI *      m_btnStartBig;
	DuiLib::CButtonUI *      m_btnStopBig;
	DuiLib::CButtonUI *      m_btnSaveBig;
	DuiLib::CButtonUI *      m_btnPrintBig;
	DuiLib::CEditUI *        m_edtBigBatchId;
	DuiLib::CEditUI *        m_edtBigPackageId;
	DuiLib::CLabelUI *       m_lblInvBigSaveRet;
	DuiLib::CLabelUI *       m_lblCountBig;

	INVENTORY_STATUS        m_InventoryBigStatus;
	std::vector<CString *>  m_vInventoryBig;
	CString                 m_strInvBigBuf;                      // ���̵�Ļ���
	DuiLib::CListUI *       m_lstInvBig;
	DuiLib::CEditUI *       m_edtManualSmallPkgId;              // �ֶ����������
	DuiLib::CButtonUI *     m_btnManualSmallPkg;

	DuiLib::CDateTimeUI *   m_dateProc;    // ��������
	DuiLib::CDateTimeUI *   m_dateValid;   // ��Ч��

	// ��ѯ
	DuiLib::CDateTimeUI *   m_dtStart;
	DuiLib::CDateTimeUI *   m_dtEnd;
	DuiLib::CEditUI *       m_edtQueryBatchId;
	DuiLib::CEditUI *       m_edtQueryOperator;
	DuiLib::CComboUI *      m_cmbQueryType;
	DuiLib::CListUI *       m_lstQueryBig;
	DuiLib::CListUI *       m_lstQuerySmall;
	DuiLib::CListUI *       m_lstQueryTags;

	// test
	DuiLib::CButtonUI*      m_btnTest;	
};