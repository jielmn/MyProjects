#pragma once

#include "common.h"
#include "LmnTemplates.h"

#include "UIlib.h"
using namespace DuiLib;

#include "resource.h"

class  CMyListData : public IListCallbackUI {
public:
	CRecycledItems<MyListItem, 10>      m_data;

private:
	LPCTSTR GetItemText(CControlUI* pControl, int iIndex, int iSubItem);
};

class CDuiFrameWnd : public WindowImplBase
{
public:
	CDuiFrameWnd();
	~CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_CLASS_WINDOW_NAME); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_FILE); }

#ifndef _DEBUG
	virtual CDuiString GetSkinFolder() { return _T(""); }
#else
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }
#endif

#ifndef _DEBUG
	virtual UILIB_RESOURCETYPE GetResourceType() const {
		return UILIB_ZIPRESOURCE;
	}

	virtual LPCTSTR GetResourceID() const {
		return MAKEINTRESOURCE(IDR_ZIPRES1);
	}
#endif

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void    OnBtnSurgery();
	void    OnBtnHand();	

private:
	CLabelUI *                   m_lblStatus;
	CButtonUI *                  m_btnSurgery;
	CButtonUI *                  m_btnHand;
	CListUI *                    m_lstReceive;
	CListUI *                    m_lstSend;

	CMyListData                  m_lstReceiveData;
	CMyListData                  m_lstSendData;
};
