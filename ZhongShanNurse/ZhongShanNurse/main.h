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

	// 处理自定义信息
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

	int                        m_nLastInventoryRet_Patients;                     // 上一次的盘点结果，对应病人Tab
	TagItem                    m_LastInventoryRet_Patients;
	int                        m_nLastInventoryRet_Nurses;                       // 上一次的盘点结果，对应护士Tab
	TagItem                    m_LastInventoryRet_Nurses;

private:
	void  AddPatientItem2List( DuiLib::CListTextElementUI* pListElement, const PatientInfo * pPatient, BOOL bSetTag = TRUE);
	void  AddNurseItem2List(DuiLib::CListTextElementUI* pListElement, const NurseInfo * pNurse, BOOL bSetTag = TRUE);

	// 添加病人
	void   OnAddPatient();
	// 修改病人
	void   OnModifyPatient();
	// 删除病人
	void   OnDeletePatient();
	// 导入病人
	void   OnImportPatients();
	// 添加护士
	void   OnAddNurse();
	// 修改护士
	void   OnModifyNurse();
	// 删除护士
	void   OnDeleteNurse();
	// 导入护士
	void   OnImportNurses();

	// 处理UM_MESSAGE
	// 数据库连接上
	void   OnFistDbConnected();
	// 所有病人ret
	void   OnGetAllPatientsRet(const std::vector<PatientInfo*> & vRet );
	// 删除病人ret
	void   OnDeletePatientRet( int ret, DWORD dwId );
	// 导入病人ret
	void   OnImportPatientsRet(int ret, const std::vector<PatientInfo*> & vRet);
	// 所有护士ret
	void   OnGetAllNursesRet(const std::vector<NurseInfo*> & vRet);
	// 删除护士ret
	void   OnDeleteNurseRet(int ret, DWORD dwId);
	// 导入护士ret
	void   OnImportNursesRet(int ret, const std::vector<NurseInfo*> & vRet);
	// Inventory ret
	void   OnInventoryRet(const TagItem * pItem);
	// Check Tag binding ret
	void   OnCheckTagBindingRet(DWORD dwPatientId, const TagItem * pItem);
}; 

