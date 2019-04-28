#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "resource.h"

#include "GridUI.h"
#include "DragDropUI.h"
#include "Launch.h"

class CDuiFrameWnd : public WindowImplBase , public sigslot::has_slots<>
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
	virtual LRESULT  HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void     OnFinalMessage(HWND hWnd);
	virtual LRESULT  OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT  OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual LRESULT  OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual LRESULT  OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

private:
	void   RemoveAllGrids();
	// 更新grids一页(比如点击了"上一页", "下一页"，初始化等等)
	void   RefreshGridsPage();
	// 更新grids size
	void   RefreshGridsSize(int width, int height);
	void   RefreshGridsSize();
	// 下一页
	void   NextPage();
	// 上一页
	void   PrevPage();
	// 双击事件
	void   OnDbClick();
	// 移动格子，调整顺序
	void   MoveGrid(const POINT & pt);
	// 停止移动格子，确定最终位置
	void   StopMoveGrid();
	// 格子移动过程中，经过的格子要高亮
	void   OnMoveGrid(const POINT & pt);
	// 拖动格子过程中，拖到最左或最右，导致翻页
	void   OnFlipPage();
	// 点击了“菜单”按钮
	void   OnBtnMenu(TNotifyUI& msg);
	// 点击了“设置”
	void   OnSetting();
	// 点击了“关于”
	void   OnAbout();
	// 保存格子配置
	void   SaveGrid(DWORD  dwIndex);
	// 删除格子配置
	void   RemoveGridCfg(DWORD  dwIndex);
	// 硬件设备变动，可能有串口变动
	void   CheckDevice();
	// 接收器状态
	void   OnLaunchStatus(WPARAM wParam, LPARAM  lParam);

public:
	// 接收器连接状态通知
	void   OnLauchStatusNotify(CLmnSerialPort::PortStatus e);

private:	
	CDialogBuilderCallbackEx                    m_callback;
	DWORD                                       m_dwCurSelGridIndex;      // 当前选择的格子序号
	ENUM_GRID_STATUS                            m_eGridStatus;            // 多格子态，还是最大化的单个格子

	DuiLib::CTabLayoutUI *                      m_tabs;
	DuiLib::CTileLayoutUI *                     m_layMain;
	CGridUI *                                   m_pGrids[MAX_GRID_COUNT];
	CHorizontalLayoutUI *                       m_layPages;
	CButtonUI *                                 m_btnPrevPage;
	CButtonUI *                                 m_btnNextPage;

	/**** drag drop操作  ****/
	int                                         m_nDgSourceIndex;         // drag drop 开始拖动的grid index
	int                                         m_nDgDestIndex;           // drag drop 拖动的目标grid index
	DWORD                                       m_dwDgStartTick;          // drag drop 开始的时间
	CControlUI *                                m_dragdropGrid;
	// 添加定时器，完成跨页间的格子拖动
	BOOL                                        m_bDragTimer;
	BOOL                                        m_bFlipPrevPage;

	// 菜单按钮
	DuiLib::CButtonUI *                         m_btnMenu;

	/******* 状态栏 ******/
	CLabelUI *                                  m_lblBarTips;
	CLabelUI *                                  m_lblLaunchStatus;
};
