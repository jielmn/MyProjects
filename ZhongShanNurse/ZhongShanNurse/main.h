#pragma once

#include "zsCommon.h"
#include "UIlib.h"

class CMyProgress : public DuiLib::CProgressUI
{
public:
	CMyProgress(DuiLib::CPaintManagerUI *p, DuiLib::CDuiString sForeImage);
	~CMyProgress();

	void DoEvent(DuiLib::TEventUI& event);
	void Stop();
	void Start();

private:
	DuiLib::CPaintManagerUI * m_pManager;
	int                       m_nPos;
	DuiLib::CDuiString        m_sForeImage;
};


class CTempDataBuilderCallback : public DuiLib::IDialogBuilderCallback
{
public:
	virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);

	DuiLib::CPaintManagerUI*     m_PaintManager;
};

class CDuiFrameWnd : public DuiLib::WindowImplBase
{
public:
	CDuiFrameWnd();
	~CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T("DUIMainFrame"); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T("main.xml"); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T("res"); }

	virtual void      InitWindow();
	virtual void      Notify(DuiLib::TNotifyUI& msg);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

	// �����Զ�����Ϣ
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	BOOL                       m_bFisrDbConnect;
	CTempDataBuilderCallback   m_Callback;

	DuiLib::CLabelUI *         m_lblDbStatus;
	DuiLib::CLabelUI *         m_lblBindingReaderStatus;
	DuiLib::CLabelUI *         m_lblSyncReaderStatus;

	// tabs
	DuiLib::CTabLayoutUI *   m_tabs;

	// sub tabs;
	DuiLib::CTabLayoutUI *   m_sub_tabs;

	// patients
	DuiLib::CListUI *           m_lstPatients;
	DuiLib::CButtonUI *         m_btnBindingPatient;

	// nurses
	DuiLib::CListUI *           m_lstNurses;
	DuiLib::CButtonUI *         m_btnBindingNurse;

	int                        m_nLastInventoryRet_Patients;                     // ��һ�ε��̵�������Ӧ����Tab
	TagItem                    m_LastInventoryRet_Patients;
	int                        m_nLastInventoryRet_Nurses;                       // ��һ�ε��̵�������Ӧ��ʿTab
	TagItem                    m_LastInventoryRet_Nurses;

	CMyProgress *              m_pSyncProgress;
	DuiLib::CButtonUI *        m_btnSync;
	DuiLib::CButtonUI *        m_btnUpdate;
	DuiLib::CButtonUI *        m_btnClear;
	DuiLib::CListUI *          m_lstSyncData;

	// ��ѯ����
	DuiLib::CListUI *          m_lstPatients_1;

	DuiLib::CListUI *          m_lstPureData;
	DuiLib::CDateTimeUI *      m_datQueryDatetime;
	DuiLib::CComboUI *         m_cmbTimeSpan;

private:
	void  AddPatientItem2List( DuiLib::CListTextElementUI* pListElement, const PatientInfo * pPatient, BOOL bSetTag = TRUE);
	void  AddPatientItem2List_1(DuiLib::CListTextElementUI* pListElement, const PatientInfo * pPatient, BOOL bSetTag = TRUE);
	void  AddNurseItem2List(DuiLib::CListTextElementUI* pListElement, const NurseInfo * pNurse, BOOL bSetTag = TRUE);
	void  SetSyncTabStatus( BOOL bEnabled = TRUE );

	// ��Ӳ���
	void   OnAddPatient();
	// �޸Ĳ���
	void   OnModifyPatient();
	// ɾ������
	void   OnDeletePatient();
	// ���벡��
	void   OnImportPatients();
	// ��ӻ�ʿ
	void   OnAddNurse();
	// �޸Ļ�ʿ
	void   OnModifyNurse();
	// ɾ����ʿ
	void   OnDeleteNurse();
	// ���뻤ʿ
	void   OnImportNurses();
	// ��
	void   OnBindingPatient();
	// �󶨻�ʿ�׿�
	void   OnBindingNurse();
	// ͬ��
	void   OnSynchronize();
	// ��ն�����
	void   OnClearReader();
	// �ϴ������ݿ�
	void   OnUpdate();
	// ��ѯ����Ķ�PatientsList item change
	void   OnShortPatiensListItemChange();
	// ��ѯ 
	void   OnQuery();

	// ����UM_MESSAGE
	// ���ݿ�������
	void   OnFistDbConnected();
	// ���в���ret
	void   OnGetAllPatientsRet(const std::vector<PatientInfo*> & vRet );
	// ɾ������ret
	void   OnDeletePatientRet( int ret, DWORD dwId );
	// ���벡��ret
	void   OnImportPatientsRet(int ret, const std::vector<PatientInfo*> & vRet);
	// ���л�ʿret
	void   OnGetAllNursesRet(const std::vector<NurseInfo*> & vRet);
	// ɾ����ʿret
	void   OnDeleteNurseRet(int ret, DWORD dwId);
	// ���뻤ʿret
	void   OnImportNursesRet(int ret, const std::vector<NurseInfo*> & vRet);
	// Inventory ret
	void   OnInventoryRet(const TagItem * pItem);
	// Check Tag binding ret
	void   OnCheckTagBindingRet(DWORD dwPatientId, const TagItem * pItem);
	// Binding Patient ret
	void   OnBindingPatientRet(int nError, CBindingPatientParam * pParam);
	// Check Card binding ret
	void   OnCheckCardBindingRet(DWORD dwNurseId, const TagItem * pItem);
	// Binding nurse ret
	void   OnBindingNurseRet(int nError, CBindingNurseParam * pParam);
	// Synchronize ret
	void   OnSynchronizeRet( int ret, std::vector<SyncItem*> * pvRet);
	// Clear ret
	void   OnClearReaderRet(int nError);
	//  Complete SyncData ret
	void   OnCompleteSyncDataRet(int nError);
	// update ret
	void   OnUpdateSyncDataRet(int nError);
	// query ret
	void   OnQueryRet(int nError, std::vector<QueryItem* > * pvRet);

private:
	BOOL                       m_bGetAllPatients;                     // �Ƿ��ȡ����
	std::vector<SyncItem*> *   m_pvSyncData;                          // һ��ͬ���Ľ�������ڱ��浽���ݿ�
}; 

