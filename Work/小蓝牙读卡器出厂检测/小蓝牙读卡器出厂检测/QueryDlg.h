#pragma once

#include "common.h"

#define   QUERY_FRAME_NAME      "DUIQueryFrame"
#define   QUERY_FILE            "query.xml"

class CQueryDlg : public DuiLib::WindowImplBase
{
public:
	CQueryDlg();

	virtual LPCTSTR    GetWindowClassName() const { return QUERY_FRAME_NAME; }
	virtual DuiLib::CDuiString GetSkinFile() { return QUERY_FILE; }
	virtual DuiLib::CDuiString GetSkinFolder() { return SKIN_FOLDER; }

	virtual void     Notify(DuiLib::TNotifyUI& msg);
	virtual void     InitWindow();
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);

private:
	void   OnQuery();

private:
	DuiLib::CDateTimeUI *             m_StartTime;
	DuiLib::CDateTimeUI *             m_EndTime;
	DuiLib::CEditUI *                 m_edMac;
	DuiLib::CListUI *                 m_lstResult;
	CButtonUI *                       m_btnQuery;
	CButtonUI *                       m_btnExport;
};

