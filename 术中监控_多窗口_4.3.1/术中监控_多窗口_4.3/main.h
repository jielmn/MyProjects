#pragma once

#include "common.h"

#include "UIlib.h"
using namespace DuiLib;

#include "resource.h"

#include "GridUI.h"
#include "DragDropUI.h"
#include "Launch.h"
#include "TagUI.h"

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
	enum  HandTagSortType {
		HandTagSortType_Default = 0,
		HandTagSortType_Time
	};

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
	void   OnDbClick(BOOL & bHandled);
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
	// 术中读卡器正在读数的提示
	void   OnTrySurReader(WPARAM wParam, LPARAM  lParam);
	// 术中读卡器状态
	void   OnSurReaderStatus(WPARAM wParam, LPARAM  lParam);
	// 术中读卡器温度
	void   OnSurReaderTemp(WPARAM wParam, LPARAM  lParam);
	// 查询温度结果(术中温度数据)
	void   OnQueryTempRet(WPARAM wParam, LPARAM  lParam);
	// 设置温度字体大小
	void   OnGridSizeChanged(const SIZE & s);
	//  1分钟定时器( 1. 定时更新逝去时间; 2. 删除一周前的温度数据 )
	void   On60SecondsTimer();	
	// 右键弹出菜单
	void   OnImageMenu(DWORD  dwGridIndex, const POINT & pt, CControlUI * pParent);
	// 导出Excel
	void   OnExportExcel(DWORD  dwIndex);
	// 打印Excel图表
	void   OnPrintExcel(DWORD  dwIndex);
	// 手持读卡器温度
	void   OnHandReaderTemp(WPARAM wParam, LPARAM  lParam);
	// 数据库的相关数据查询完毕
	void   OnPrepared(WPARAM wParam, LPARAM  lParam);
	// 获得当前的手持Tag排序
	HandTagSortType GetHandTagSortType();
	// 获得所有手持Tag温度数据
	void   OnAllHandTagTempData(WPARAM wParam, LPARAM  lParam);
	// 选中了手持Tag
	void   OnHandTagSelected(CControlUI * pTagUI);
	// 点击了按时间排序
	void   OnHandTagTimeOrder();
	void   OnHandTagTimeOrder(CTagUI * pTagUI);
	// 移动Tag UI，绑定窗格
	void   MoveTagUI(const POINT & pt);
	// 停止移动Tag UI，确定最终位置
	void   StopMoveTagUI();
	// Tag UI移动过程中，经过的格子要高亮
	void   OnMoveTagUI(const POINT & pt);
	// 重新设置LayoutGrids视图
	void   ResetDragdropGrids(int w = 0, int h = 0);
	// 获取DragdropGrids视图的行，列
	void   GetDragDropGridsParams(int & nCol, int & nRow );
	// 根据Size大小获取Label字体大小
	int    GetFontBySize(const SIZE & s);
	// tag 绑定 grid index结果
	void   OnTagBindingGridRet(WPARAM wParam, LPARAM  lParam);
	// 查询温度结果(手持Tag温度数据)
	void   OnQueryHandTempRet(WPARAM wParam, LPARAM  lParam);
	// tag name changed
	void   OnTagNameChanged(WPARAM wParam, LPARAM  lParam);
	// 右键弹出菜单
	void   OnHandImageMenu(const POINT & pt, CControlUI * pParent);
	// 导出Excel
	void   OnHanxExportExcel();
	// 打印Excel图表
	void   OnHandPrintExcel();
	// 温度打印
	void   OnBtnPrint(DWORD  dwIndex);

public:
	// 接收器连接状态通知
	void   OnLauchStatusNotify(CLmnSerialPort::PortStatus e);
	// 接收器尝试读取术中读卡器的温度数据
	void   OnTrySurReaderNotify(WORD wBed, BOOL bStart);
	// 接收到术中读卡器连接状态的通知
	void   OnSurReaderStatusNotify(WORD wBed, BOOL bConnected);
	// 术中读卡器的温度数据
	void   OnSurReaderTempNotify(WORD wBed, const TempItem & item);
	// 查询到温度结果(根据TagId)
	void   OnQueryTempRetNotify(const char *, WORD, std::vector<TempItem*> *);
	// 手持读卡器的温度数据
	void   OnHandReaderTempNotify(const TempItem & item, const char * tag_patient_name);
	// 数据库需要的数据已经查询完毕
	void   OnPreparedNotify();
	// 查询到的所有手持Tag温度数据
	void   OnAllHandTagTempDataNotify(std::vector<HandTagResult *> * pvRet);
	// 删除掉过时的手持Tag
	void   OnHandTagErasedNotify(const char * szTagId);	
	// 绑定结果的通知
	void   OnBindingRetNotify(const TagBindingGridRet & item);
	// 查询手持Tag结果通知
	void   OnQueryHandTagRetNotify(const char *, int, std::vector<TempItem*> *);

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

	CMyHandImage *                              m_cstHandImg;            // 手持读卡器的图
	CVerticalLayoutUI *                         m_layTags;               // 手持读卡器的tag列表

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
	CLabelUI *                                  m_lblSurReaderTry;

	/*手持Reader的排序*/
	COptionUI *                                 m_optDefaultSort;
	COptionUI *                                 m_optTimeSort;
	DuiLib::CTabLayoutUI *                      m_hand_tabs;            // 绑定tag的过程中用到
	CTileLayoutUI *                             m_layDragDropGrids;     // 绑定tag的过程中用到

	std::map<std::string, CTagUI *>             m_tags_ui;
	CDuiString                                  m_dragdrop_tag;
	int                                         m_dragdrop_tag_dest_index;
	DWORD                                       m_dragdrop_tag_timetick;

	// 定时保存excel
	time_t                                      m_LastSaveExcelTime;
};
