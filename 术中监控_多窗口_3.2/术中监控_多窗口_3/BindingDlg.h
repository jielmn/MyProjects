#pragma once

#include "common.h"
#include "business.h"
#include "UIlib.h"
#include "MyTreeCfgUI.h"

class CBindingDlg : public DuiLib::WindowImplBase
{
public:
	typedef  struct tagTagItem{
		char    m_szTagId[20];
		DWORD   m_dwSubIndex;
	}TagItem;

	TagItem      m_tTagItem[MAX_READERS_PER_GRID];
	DWORD        m_dwTagItemCnt;
	DWORD        m_dwPatientId;
	DWORD        m_dwIndex;

	DWORD        m_dwBindingPatientId;
	char         m_szBindingPatientName[20];
	char         m_szTagName[MAX_READERS_PER_GRID][20];

	CBindingDlg();

	virtual LPCTSTR    GetWindowClassName() const { return _T(BINDING_FRAME_NAME); }
	virtual DuiLib::CDuiString GetSkinFile() { return _T(BINDING_FILE); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void   Notify(DuiLib::TNotifyUI& msg);
	virtual void   InitWindow();
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual DuiLib::CControlUI * CreateControl(LPCTSTR pstrClass);

private:

	DuiLib::CListUI *               m_lstTags;
	DuiLib::CTileLayoutUI *         m_layTags;
	CControlUI *                    m_pTags[MAX_READERS_PER_GRID];
	CLabelUI *                      m_lblTagPos[MAX_READERS_PER_GRID];
	CLabelUI *                      m_lblTagId[MAX_READERS_PER_GRID];
	CComboUI *                      m_cmbTagName[MAX_READERS_PER_GRID];
	DuiLib::CListUI *               m_lstPatients;

private:
	void   OnSize(WPARAM wParam, LPARAM lParam);
	void   OnAllPatientsRet(WPARAM wParam, LPARAM lParam);
	void   OnBtnOk();
	void   OnBindTagsRet(WPARAM wParam, LPARAM lParam);
};

