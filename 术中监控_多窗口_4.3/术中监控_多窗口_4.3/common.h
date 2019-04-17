#pragma once

#include <vector>
#include <algorithm>
#include <assert.h>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"
#include "sigslot.h"

#include "UIlib.h"
using namespace DuiLib;

#define   PROJ_NAME               "temp_monitor"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   MAX_GRID_COUNT           60             // 最多几个格子
#define   MAX_READERS_PER_GRID     6              // 每个格子最多几个测温点
#define   MAX_COLUMNS_COUNT        5              // 每个页面最大几列
#define   MAX_ROWS_COUNT           5              // 每个页面最大几行

// 控件id
#define   TABS_ID                  "switch"
#define   TAB_INDEX_MONITOR        0
#define   TAB_INDEX_READER         1
#define   LAYOUT_MAIN_NAME         "layGrids"
#define   GRID_XML_FILE            "grid.xml"
#define   GRID_NAME                "MyGrid"
#define   GRID_BORDER_SIZE         4
#define   GRID_BORDER_COLOR        0xFF4F4F4F
#define   LAYOUT_PAGES            "layPages"
#define   BUTTON_PREV_PAGE        "btnPrevPage"
#define   BUTTON_NEXT_PAGE        "btnNextPage"


// CONFIG
#define   CFG_MAIN_LAYOUT_COLUMNS           "main layout columns"
#define   CFG_MAIN_LAYOUT_ROWS              "main layout rows"
#define   DEFAULT_MAIN_LAYOUT_COLUMNS       2
#define   DEFAULT_MAIN_LAYOUT_ROWS          2
#define   CFG_MAIN_LAYOUT_GRIDS_COUNT       "main layout grids count"
#define   DEFAULT_MAIN_LAYOUT_GRIDS_COUNT   4
#define   CFG_GRIDS_ORDER                   "grids order"

// thread消息


// windows 消息



// 创建duilib控件的回调
class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CDialogBuilderCallbackEx(DuiLib::CPaintManagerUI *pManager) {
		m_pManager = pManager;
	}
	CControlUI* CreateControl(LPCTSTR pstrClass);
private:
	DuiLib::CPaintManagerUI *  m_pManager;
};

// 术中读卡器
typedef struct tagReaderCfg
{
	BOOL      m_bSwitch;                             // 读卡器开关
	DWORD     m_dwLowTempAlarm;                      // 低温报警(单位：摄氏度)
	DWORD     m_dwHighTempAlarm;                     // 高温报警(单位：摄氏度)
}ReaderCfg;

// 术中窗格
typedef struct tagGridCfg
{
	BOOL        m_bSwitch;                              // 总开关
	DWORD       m_dwCollectInterval;                    // 采集间隔(单位:秒)
	DWORD       m_dwMinTemp;                            // 显示的最低温度(单位:℃)
	DWORD       m_dwMaxTemp;                            // 显示的最高温度(单位:℃)
	ReaderCfg   m_ReaderCfg[MAX_READERS_PER_GRID];
}GridCfg;

typedef struct tagCfgData {
	DWORD     m_dwAreaNo;                               // 术中病区号
	DWORD     m_dwLayoutColumns;                        // 列数
	DWORD     m_dwLayoutRows;                           // 行数
	DWORD     m_dwLayoutGridsCnt;                       // 床位总数  (不能超过MAX_GRID_COUNT)
	BOOL      m_bAlarmVoiceOff;                         // 报警声音开关
	BOOL      m_bAutoSaveExcel;                         // 自动保存Excel
	BOOL      m_bCrossAnchor;                           // 十字锚
	GridCfg   m_GridCfg[MAX_GRID_COUNT];                // 术中格子配置
	DWORD     m_GridOrder[MAX_GRID_COUNT];              // 格子顺序，例如: 2,1,3,4,5,6,30,7,8,9...

	DWORD     m_dwHandReaderMinTemp;                    // 手持读卡器显示最低温度
	DWORD     m_dwHandReaderMaxTemp;                    // 手持读卡器显示最高温度
	DWORD     m_dwHandReaderLowTempAlarm;               // 手持读卡器低温报警
	DWORD     m_dwHandReaderHighTempAlarm;              // 手持读卡器高温报警	
}CfgData;

class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_timer;
	CfgData                   m_CfgData;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_timer = 0;
	}
};

extern CGlobalData  g_data;

extern LONG WINAPI pfnUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);

// templates
template <class T>
void ClearVector(std::vector<T> & v) {
	typedef std::vector<T>::iterator v_it;
	v_it it;
	for (it = v.begin(); it != v.end(); it++) {
		delete *it;
	}
	v.clear();
}