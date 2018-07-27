#pragma once

#include "common.h"
#include "MyImage.h"

class CDuiFrameWnd : public WindowImplBase
{
public:
	CDuiFrameWnd();
	virtual LPCTSTR    GetWindowClassName() const { return _T(MAIN_FRAME_NAME); }
	virtual CDuiString GetSkinFile() { return _T(MAIN_XML_FILE); }
	virtual CDuiString GetSkinFolder() { return _T(SKIN_FOLDER); }

	virtual void    Notify(TNotifyUI& msg);
	virtual void    InitWindow();
	virtual CControlUI * CreateControl(LPCTSTR pstrClass);
	virtual void     OnFinalMessage(HWND hWnd);

	// 处理自定义信息
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void   OnSize(WPARAM wParam, LPARAM lParam);
	void   OnChangeSkin();
	void   OnBtnMenu(TNotifyUI& msg);
	void   OnSetting();
	void   OnAbout();
	void   OnDbClick();
	// 重新布局
	void   ReLayout(DWORD dwWidth, DWORD dwHeight);
	// 重新布局(在设置更改之后)
	void   UpdateLayout();
	void   OnUpdateGridScroll(WPARAM wParam, LPARAM lParam);
	void   OnMyImageClick(const POINT * pPoint);

private:
	CDialogBuilderCallbackEx           m_callback;

	DuiLib::CVerticalLayoutUI *        m_layWindow;
	DuiLib::CTileLayoutUI *            m_layMain;
	DuiLib::CHorizontalLayoutUI *      m_layStatus;

	CControlUI *                       m_pGrids[MAX_GRID_COUNT];
	CLabelUI *                         m_pLblIndexes_small[MAX_GRID_COUNT];
	CLabelUI *                         m_pLblBed_small[MAX_GRID_COUNT];
	CLabelUI *                         m_pLblName_small[MAX_GRID_COUNT];
	CLabelUI *                         m_pLblCurTemp_small[MAX_GRID_COUNT];
	CLabelUI *                         m_pLblBedTitle_small[MAX_GRID_COUNT];
	CLabelUI *                         m_pLblNameTitle_small[MAX_GRID_COUNT];
	CLabelUI *                         m_pLblCurTempTitle_small[MAX_GRID_COUNT];
	CMyImageUI *                       m_pMyImage[MAX_GRID_COUNT];

	DuiLib::CButtonUI *                m_btnMenu;    // 菜单按钮

	int                                m_nState;     // 多格子状态，单格子状态
	int                                m_nMaxGridIndex; // 最大化格子的序号
};
