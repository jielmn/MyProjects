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

private:
	// 添加病人
	void   OnAddPatient();
	// 修改病人
	void   OnModifyPatient();
	// 删除病人
	void   OnDeletePatient();

	// 数据库连接上
	void   OnFistDbConnected();
}; 

