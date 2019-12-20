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
	// �˵���ť
	DuiLib::CButtonUI *                         m_btnMenu;
	// Grids Pagesҳ
	DuiLib::CHorizontalLayoutUI *               m_layGridsPages;
	DuiLib::CTabLayoutUI *                      m_tabs;
	DuiLib::CListUI *                           m_lstItems;
	DuiLib::CEditUI *                           m_edName;
	int                                         m_nEditingNameIndex; // ���ڱ༭���б��������index
	CButtonUI *                                 m_btnPrev;
	CButtonUI *                                 m_btnNext;

private:
	std::vector<CWearItem *>                    m_data;
	int                                         m_nCurPageFirstItemIndex;   // ��ǰҳ�ĵ�һ���index
	int                                         m_nItemsPerPage;
	RECT                                        m_rcLayGrids;

private:
	void  OnExpand();
	void  OnMainMenu(TNotifyUI& msg);
	void  OnSetting();
	bool  OnGridsLayoutSize(void * pParam);
	void  OnCheckHistory();
	void  OnWarningPrepose();
	void  OnRecycle();
	void  OnAbout();
	// ˫���¼�
	void  OnDbClick(BOOL & bHandled);
	void  OnEdNameKillFocus();
	// ����grids����
	void  FillData();
	void  OnPrevPage();
	void  OnNextPage();
};
