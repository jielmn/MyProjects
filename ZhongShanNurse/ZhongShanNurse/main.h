#pragma once

#include "zsCommon.h"
#include "UIlib.h"

class CTempDataBuilderCallback : public DuiLib::IDialogBuilderCallback
{
public:
	virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);

	DuiLib::CPaintManagerUI*     m_PaintManager;
};

class CDuiFrameWnd : public DuiLib::WindowImplBase
{
public:
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

private:
	void  AddPatientItem2List( DuiLib::CListTextElementUI* pListElement, const PatientInfo * pPatient, BOOL bSetTag = TRUE);
	void  AddNurseItem2List(DuiLib::CListTextElementUI* pListElement, const NurseInfo * pNurse, BOOL bSetTag = TRUE);

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
}; 

