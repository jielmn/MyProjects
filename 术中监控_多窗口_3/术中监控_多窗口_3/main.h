#pragma once

#include "common.h"
#include "MyImage.h"


class CDuiFrameWnd : public WindowImplBase
{
public:
	enum ENUM_GRID_STATUS {
		GRID_STATUS_GRIDS = 0,
		GRID_STATUS_MAXIUM
	};

	CDuiFrameWnd();
	~CDuiFrameWnd();

	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_CLASS_WINDOW_NAME); }
	virtual CDuiString GetSkinFile() { return _T(SKIN_FILE); }
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CDialogBuilderCallbackEx           m_callback;
	DuiLib::CTileLayoutUI *            m_layMain;
	DuiLib::CHorizontalLayoutUI *      m_layStatus;
	CControlUI *                       m_pGrids[MAX_GRID_COUNT];
	CHorizontalLayoutUI *              m_pLayFlex[MAX_GRID_COUNT];
	ENUM_GRID_STATUS                   m_eGridStatus;

private:
	void   OnSize(WPARAM wParam, LPARAM lParam);
	// 重新布局
	void   ReLayout(DWORD dwWidth, DWORD dwHeight);
	void   OnChangeSkin();
	void   OnDbClick();

};
