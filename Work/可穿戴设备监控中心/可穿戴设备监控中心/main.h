#pragma once

#include "common.h"

#include "resource.h"

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
	DuiLib::CTileLayoutUI *                     m_layGrids;
	DuiLib::CVerticalLayoutUI *                 m_layList;
	DuiLib::CButtonUI *                         m_btnExpand;
	// 菜单按钮
	DuiLib::CButtonUI *                         m_btnMenu;
	// Grids Pages页
	DuiLib::CHorizontalLayoutUI *               m_layGridsPages;
	DuiLib::CTabLayoutUI *                      m_tabs;
	DuiLib::CListUI *                           m_lstItems;
	DuiLib::CEditUI *                           m_edName;
	int                                         m_nEditingNameIndex; // 正在编辑的列表的姓名的index
	CButtonUI *                                 m_btnPrev;
	CButtonUI *                                 m_btnNext;
	CListHeaderItemUI *                         m_Header[4];      // 姓名，心率，血氧，体温
	CLabelUI *                                  m_lblStatus;

private:
	std::vector<CWearItem *>                    m_data;
	int                                         m_nCurPageFirstItemIndex;   // 当前页的第一项的index
	int                                         m_nItemsPerPage;
	RECT                                        m_rcLayGrids;
	Sort                                        m_Sort[4];    // 姓名，心率，血氧，体温

private:
	void  OnExpand();
	void  OnMainMenu(TNotifyUI& msg);
	void  OnSetting();
	bool  OnGridsLayoutSize(void * pParam);
	void  OnCheckHistory();
	void  OnWarningPrepose();
	void  OnRecycle();
	void  OnAbout();
	// 双击事件
	void  OnDbClick(BOOL & bHandled);
	void  OnEdNameKillFocus();
	// 更新grids数据
	void  UpdateGrids();
	void  OnPrevPage();
	void  OnNextPage();
	// 更新List
	void  UpdateList();
	CDuiString  GetHeartBeatStr(CWearItem * pItem);
	CDuiString  GetOxyStr(CWearItem * pItem);
	CDuiString  GetTempStr(CWearItem * pItem);
	// 姓名，心率，血氧，体温
	void  Sort(int nIndex, BOOL bNeedUpdate = TRUE);
	void  SortByWarn();
	void  CancelSort();
	void  OnNewWearItem(CWearItem * pItem);
	void  CheckListItemWarning(CListTextElementUI * pListItem);
};

class CSortName {
public:
	CSortName(BOOL  bAscend) {
		m_bAscend = bAscend;
	}

	bool operator() (CWearItem * p1, CWearItem * p2) {
		if ( m_bAscend )
			return CharacterCompare(p1->m_szName, p2->m_szName) < 0 ? true : false;
		else
			return CharacterCompare(p1->m_szName, p2->m_szName) > 0 ? true : false;
	}

private:
	BOOL   m_bAscend;
};

class CSortHeartBeat {
public:
	CSortHeartBeat(BOOL  bAscend) {
		m_bAscend = bAscend;
	}

	bool operator() (CWearItem * p1, CWearItem * p2) {
		int n1 = p1->m_nHearbeat;
		int n2 = p2->m_nHearbeat;

		if (m_bAscend)
			return  n1 < n2 ? true : false;
		else
			return  n1 > n2 ? true : false;
	}

private:
	BOOL   m_bAscend;
};

class CSortOxy {
public:
	CSortOxy(BOOL  bAscend) {
		m_bAscend = bAscend;
	}

	bool operator() (CWearItem * p1, CWearItem * p2) {
		int n1 = p1->m_nOxy;
		int n2 = p2->m_nOxy;

		if (m_bAscend)
			return  n1 < n2 ? true : false;
		else
			return  n1 > n2 ? true : false;
	}

private:
	BOOL   m_bAscend;
};

class CSortTemp {
public:
	CSortTemp(BOOL  bAscend) {
		m_bAscend = bAscend;
	}

	bool operator() (CWearItem * p1, CWearItem * p2) {
		int n1 = p1->m_nTemp;
		int n2 = p2->m_nTemp;

		if (m_bAscend)
			return  n1 < n2 ? true : false;
		else
			return  n1 > n2 ? true : false;
	}

private:
	BOOL   m_bAscend;
};

class CSortWarnPrepose {
public:
	bool operator() (CWearItem * p1, CWearItem * p2) {
		int n1 = IsWarningItem(p1) ? 0 : 1;
		int n2 = IsWarningItem(p2) ? 0 : 1;
		return n1 < n2 ? true : false;
	}
};

class CFindWearItem {
public:
	CFindWearItem(CWearItem * pItem) {
		m_pItem = pItem;
	}

	bool operator() (CWearItem * p) {
		if (0 == strcmp(p->m_szDeviceId, m_pItem->m_szDeviceId)) {
			return true;
		}
		else {
			return false;
		}
	}

private:
	CWearItem *   m_pItem;
};

