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

private:
	void  AddPatientItem2List( DuiLib::CListTextElementUI* pListElement, const PatientInfo * pPatient, BOOL bSetTag = TRUE);

	// ��Ӳ���
	void   OnAddPatient();
	// �޸Ĳ���
	void   OnModifyPatient();
	// ɾ������
	void   OnDeletePatient();
	// ���벡��
	void   OnImportPatients();

	// ����UM_MESSAGE
	// ���ݿ�������
	void   OnFistDbConnected();
	// ���в���ret
	void   OnGetAllPatientsRet(const std::vector<PatientInfo*> & vRet );
	// ɾ������ret
	void   OnDeletePatientRet( int ret, DWORD dwId );
	// ���벡��ret
	void   OnImportPatientsRet(int ret, const std::vector<PatientInfo*> & vRet);
}; 

