#pragma once

#include <vector>
#include <map>
#include <algorithm>
#include <iterator> 
#include <assert.h>
#include <time.h>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"
#include "LmnTelSvr.h"
#include "sigslot.h"

#include "UIlib.h"
using namespace DuiLib;

#include "Xml2Chart.h"

#define   PROJ_NAME               "temp_monitor"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")
#define   GLOBAL_LOCK_NAME        PROJ_NAME

#ifdef _DEBUG
#define   TEST_MEMORY_LEAK_FLAG   0              // 测试随时间变化，内存消耗会不会增加
#endif

#define  HANDLE_WRONG_FORMAT_1     1             // 主要针对连续测温，结束字符不是FF

#define  TRI_TAGS_FLAG             1              // 三联
#define  TRI_TAGS_COLOR_0          0xFFFFFFFF
#define  TRI_TAGS_COLOR_1          0xFF999999
#define  TRI_TAGS_COLOR_2          0xFF00FF00

// #define  CUBE_TEST_FLAG            1
#define  CUBE_ALTERNATIVE_BKCOLOR   0xFF666666
#define  CUBE_WARNING_COLOR         0xFFF8992F
#define  CUBE_FADE_TEXTCOLOR        0xFF999999
#define  CUBE_REFRESH_TEXTCOLOR     0xFFFFFFFF

// 15分钟内测量算连续测量，次数为一次
#ifdef _DEBUG
#define  TAKE_TEMPERATURE_SPAN_TIME    60
#else
#define  TAKE_TEMPERATURE_SPAN_TIME    900
#endif


// 窗格温度文字透明度
#define   GRID_LABLE_TRANSPARENT_PARAM       0xCC

#define   MAX_GRID_COUNT           1              // 最多几个格子
#define   MAX_READERS_PER_GRID     6              // 每个格子最多几个测温点
#define   MAX_COLUMNS_COUNT        8              // 每个页面最大几列
#define   MAX_ROWS_COUNT           8              // 每个页面最大几行
#define   DRAG_DROP_DELAY_TIME     100            // drag drop操作的延时时间
#define   DEF_GET_TEMPERATURE_DELAY    10000      // 术中读卡器默认10秒后再次读取温度
#define   MAX_TIME_NEEDED_BY_SUR_TEMP  5000       // 一次获取术中读卡器温度的软件最大等待时间，如果超过，则认为超时失败(单位：毫秒)
#define   READ_LAUNCH_INTERVAL_TIME    1000
#define   SELECTED_READER_BG_COLOR     0xFF555555
#define   HIGH_TEMP_TEXT_COLOR        0xFFDF455F
#define   LOW_TEMP_TEXT_COLOR         0xFF01AFC3
#define   NORMAL_TEMP_TEXT_COLOR      0xFF4E8B20
#define   DISCONNECTED_IMAGE          "disconnected.png"
#define   MIN_MYIMAGE_VMARGIN         40
#define   SCALE_RECT_WIDTH            50          // 刻度区域的宽度
#define   LABEL_SCALE_RECT_WIDTH      40          // 刻度区域的宽度
#define   BRIGHT_DARK_INTERVAL        30          // 明暗刻度的间隔
#define   EDT_REMARK_WIDTH            200
#define   EDT_REMARK_HEIGHT           30
#define   EDT_REMARK_Y_OFFSET         10
#define   TAG_UI_HEIGHT               110
#ifdef _DEBUG
#define   TAG_PNAME_OVERTIME          10       // 单位：秒 
#else
#define   TAG_PNAME_OVERTIME          3600     // 单位：秒 
#endif
#define   MAX_TAG_PNAME_LENGTH        20

#define   VERSION                     "1.0.0"
#define   COMPILE_TIME                "2020-02-11 18:00"
#ifdef _DEBUG
#define   MAX_ITEMS_PER_PAGE          2
#else
#define   MAX_ITEMS_PER_PAGE          10
#endif
// 是否显示住院和出院两个选项块
#define  SHOW_IN_HOSPITAL_FLAG        0

// 控件id
#define   TABS_ID                  "switch"
#define   TAB_INDEX_MONITOR        0
#define   TAB_INDEX_READER         1
#define   TAB_INDEX_INHOSPITAL     2
#define   TAB_INDEX_OUTHOSPITAL     3
#define   LAYOUT_MAIN_NAME         "layGrids"
#define   GRID_XML_FILE            "grid.xml"
#define   GRID_NAME                "MyGrid"
#define   GRID_BORDER_SIZE         4
#define   GRID_BORDER_COLOR        0xFF4F4F4F
//#define   GRID_BORDER_COLOR        0xFFFF0000
#define   LAYOUT_PAGES            "layPages"
#define   BUTTON_PREV_PAGE        "btnPrevPage"
#define   BUTTON_NEXT_PAGE        "btnNextPage"
#define   BTN_BED_NO              "btnBed"
#define   BTN_BED_NO_M            "btnBedM"
#define   LBL_READER_NO           "lblReaderNo"
#define   LAY_READERS             "layReaders"
#define   GRID_TABS               "gridtabs"
#define   GRID_HEAD_LAYOUT        "layGridHead"
#define   DRAG_DROP_GRID          "DragDropGrid"
#define   GRID_HILIGHT_BORDER_COLOR 0xFFCAF100
#define   BTN_MENU                "menubtn"
#define   LBL_VERSION             "lblVersion"
#define   MYTREE_CLASS_NAME       "MyTree"
#define   MYTREE_CONFIG_NAME      "CfgTree"
#define   SETTING_FRAME_NAME      "DUISettingFrame"
#define   SETTING_FILE            "Setting.xml"
#define   READER_FILE_NAME        "reader.xml"
#define   LBL_READER_INDICATOR    "lblIndicator"
#define   READER_INDICATOR        "indicator"
#define   LBL_BAR_TIPS            "lblBarTips"
#define   LBL_LAUNCH_STATUS       "lblLaunchStatus"
#define   MODE_BUTTON             "btnMode"
#define   OPT_READER_SELECTED     "opn_reader_switch"
#define   CST_EDT_BTN_BodyPart    "cstEdtBtnBodyPart"
#define   CTR_READER_STATE        "ReaderState"
#define   CTR_CUR_READER_STATE    "CurReaderState"
#define   LBL_PROC_TIPS           "lblProcessTips"
#define   CST_IMG_LBL_TEMP        "cstImgLblTemp"
#define   CST_PATIENT_NAME        "cstEdtBtnPatientName"
#define   CST_PATIENT_NAME_M      "cstEdtBtnPatientNameM"
#define   LBL_READER_TEMP         "lblReaderTemp"
#define   LAY_READER              "layReader"
#define   LBL_ELAPSED             "lblElapsed"
#define   LBL_READER_ID           "lblReaderId"
#define   LBL_TAG_ID              "lblTagId"
#define   CST_IMAGE               "cstMyImage"
#define   DRAG_DROP_CTL           "dragdrop"
#define   EDIT_REMARK             "edRemark"
#define   EDIT_HAND_REMARK        "edHandRemark"
#define   CST_HAND_IMAGE          "cstMyImageHand"
#define   TAG_FILE_NAME           "tag.xml"
#define   LAYOUT_TAGS             "layTags"
#define   LBL_HAND_READER_ID      "lblHandReaderId"
#define   LBL_HAND_TAG_ID         "lblHandTagId"
#define   LBL_HAND_TEMP_TIME      "lblHandTempTime"
#define   CST_HAND_P_NAME         "cstEdtBtnPName"
#define   LBL_HAND_TEMP           "lblHandTagTemp"
#define   LBL_HAND_BINDING        "lblBindingGrid"
#define   OPT_DEFAULT             "rdDefaultOrder"
#define   OPT_TIME                "rdTimeOrder"
#define   HAND_TABS_ID            "layHand"
#define   LAY_DRAGDROP_GRIDS      "layGridsView"
#define   BTN_PRINT               "btnPrint"
#define   PATIENT_DATA_FRAME_NAME  "DUIPatientDataFrame"
#define   PATIENT_DATA_FILE       "PatientData.xml"
#define   MYTREE_PATIENT_DATA_NAME    "CfgTree"
#define   LAYOUT_SIX_GRIDS_NAME    "laySixGrids"
#define   PATIENT_DATA_END_DATE    "DateTime2"

// CONFIG
#define   CFG_MAIN_LAYOUT_COLUMNS           "main layout columns"
#define   CFG_MAIN_LAYOUT_ROWS              "main layout rows"
#define   DEFAULT_MAIN_LAYOUT_COLUMNS       2
#define   DEFAULT_MAIN_LAYOUT_ROWS          2
#define   CFG_MAIN_LAYOUT_GRIDS_COUNT       "main layout grids count"
#define   DEFAULT_MAIN_LAYOUT_GRIDS_COUNT   4
#define   CFG_GRIDS_ORDER                   "grids order"
#define   AREA_CFG_FILE_NAME                "area.cfg"
#define   MAX_AREA_COUNT                    20
#define   CFG_AREA_NAME                     "area name"
#define   CFG_AREA_NO                       "area no"
#define   MAX_AREA_ID                       100
#define   CFG_AREA_ID_NAME                  "area id"
#define   CFG_ALARM_VOICE_SWITCH            "alarm voice switch"
#define   DEFAULT_ALARM_VOICE_SWITCH        TRUE
#define   CFG_AUTO_SAVE_EXCEL               "auto save excel"
#define   CFG_CROSS_ANCHOR                  "cross anchor"
#define   MIN_TEMP_IN_SHOW                  20
#define   MAX_TEMP_IN_SHOW                  42
#define   DEFAULT_MIN_TEMP_IN_SHOW          28
#define   DEFAULT_MAX_TEMP_IN_SHOW          40
#define   LOW_TEMP_ALARM                    2000
#define   HIGH_TEMP_ALARM                   4200
#define   DEFAULT_LOW_TEMP_ALARM            3500
#define   DEFAULT_HIGH_TEMP_ALARM           3800
#define   CFG_HAND_MIN_TEMP_IN_SHOW         "hand min temperature"
#define   CFG_HAND_MAX_TEMP_IN_SHOW         "hand max temperature"
#define   CFG_HAND_LOW_TEMP_ALARM           "hand low temperature alarm"
#define   CFG_HAND_HIGH_TEMP_ALARM          "hand max temperature alarm"
#define   CFG_COLLECT_INTERVAL              "collect interval"
#define   MAX_COLLECT_INTERVAL_INDEX        5
#define   CFG_GRID_MIN_TEMP                 "grid min temperature"
#define   CFG_GRID_MAX_TEMP                 "grid max temperature"
#define   CFG_READER_SWITCH                 "reader switch"
#define   DEFAULT_READER_SWITCH             FALSE
#define   CFG_LOW_TEMP_ALARM                "low temperature alarm"
#define   CFG_HIGH_TEMP_ALARM               "high temperature alarm"
#define   CFG_GRID_MODE                     "grid mode"
#define   CFG_LAUNCH_COM_PORT               "launch com ports"
#define   CFG_READER_NAME                   "reader name"
#define   CFG_HAND_READER_LOW_TEMP_ALARM    "hand reader low temperature alarm"
#define   CFG_HAND_READER_HIGH_TEMP_ALARM   "hand reader high temperature alarm"
#define   CFG_HAND_READER_NAME              "hand reader name"
#define   CFG_PATIENT_NAME                  "patient name"

// thread消息
#define MSG_CHECK_LAUNCH_STATUS             1001
#define MSG_PRINT_STATUS                    1002
#define MSG_RESTART_LAUNCH                  1003
#define MSG_READ_LAUNCH                     1004
#define MSG_GET_GRID_TEMP                   1100
#define MSG_GET_GRID_TEMP_MAX               1199
#define MSG_SAVE_SUR_TEMP                   2000
#define MSG_QUERY_TEMP_BY_TAG               2001
#define MSG_SAVE_REMARK                     2002
#define MSG_SAVE_HAND_TEMP                  2003
#define MSG_CHECK_SQLITE                    2004
#define MSG_SAVE_LAST_SUR_TAG_ID            2005
#define MSG_PREPARE                         2006
#define MSG_BINDING_TAG_GRID                2007
#define MSG_QUERY_TEMP_BY_HAND_TAG          2008
#define MSG_REMOVE_GRID_BINDING             2009
#define MSG_SAVE_TAG_PNAME                  2010
#define MSG_ALARM                           2011
#define MSG_WRITE_TEMP_2_EXCEL              2012
#define MSG_SAVE_EXCEL                      2013
#define MSG_INIT_EXCEL                      2014
#define MSG_QUERY_PATIENT_INFO              2015
#define MSG_QUERY_PATIENT_DATA              2016
#define MSG_SAVE_PATIENT_INFO               2017
#define MSG_SAVE_PATIENT_DATA               2018
#define MSG_SAVE_PATIENT_EVENTS             2019
#define MSG_QUERY_BINDING_BY_TAG            2020
#define MSG_DEL_TAG                         2021
#define MSG_QUERY_INHOSPITAL                2022
#define MSG_QUERY_OUTHOSPITAL               2023
#define MSG_SAVE_CUBE_BED                   2024
#define MSG_READ_COM_PORTS                  2025
#define MSG_CUBE_BINDING_TAG                2026
#define MSG_CUBE_SAVE_TEMP                  2027
#define MSG_CUBE_QUERY_TEMP                 2028
#define MSG_UPDATE_CUBE_ITEM                2029
#define MSG_CUBE_DISMISS_BINDING            2030


// windows 消息
#define UM_LAUNCH_STATUS                     (WM_USER+1)
#define UM_TRY_SUR_READER                    (WM_USER+2)
#define UM_SUR_READER_STATUS                 (WM_USER+3)
#define UM_SUR_READER_TEMP                   (WM_USER+4)
#define UM_QUERY_TEMP_BY_TAG_ID_RET          (WM_USER+5)
#define UM_HAND_READER_TEMP                  (WM_USER+6)
#define UM_PREPARED                          (WM_USER+7)
#define UM_ALL_HAND_TAG_TEMP_DATA            (WM_USER+8)
#define UM_BINDING_TAG_GRID_RET              (WM_USER+9)
#define UM_QUERY_HAND_TEMP_BY_TAG_ID_RET     (WM_USER+10)
#define UM_TAG_NAME_CHANGED                  (WM_USER+11)
#define UM_PATIENT_INFO                      (WM_USER+12)
#define UM_PATIENT_DATA                      (WM_USER+13)
#define UM_QUERY_BINDING_BY_TAG_RET          (WM_USER+14)
#define UM_DEL_TAG_RET                       (WM_USER+15)
#define UM_QUERY_INHOSPITAL_RET              (WM_USER+16)
#define UM_QUERY_OUTHOSPITAL_RET             (WM_USER+17)
// 检查打开了哪些串口
#define UM_CHECK_COM_PORTS_RET               (WM_USER+18)
#define UM_GET_ALL_CUBE_ITEMS                (WM_USER+19)
#define UM_ADD_CUBE_ITEMS_RET                (WM_USER+20)
// 串口数据
#define UM_CUBE_TEMP_ITEM                    (WM_USER+21)
#define UM_CUBE_BINDING_RET                  (WM_USER+22)
// 获取方舱床位号的温度数据
#define UM_CUBE_QUERY_TEMP_RET               (WM_USER+23)
// 更新数据
#define UM_UPDATE_CUBE_ITEM_RET              (WM_USER+24)
// 解除绑定
#define UM_CUBE_DISMISS_BINDING_RET          (WM_USER+25)


#define XML_CHART_WIDTH              764
#define XML_CHART_HEIGHT             1080

#define CUR_BINDING_GRID_BKCOLOR       0xFFFCfB9C
#define CUR_BINDING_GRID_TEXTCOLOR     0xFF000000
#define CUR_BINDING_GRID_BORDERCOLOR   0xFFCAF100

#define UNBINDING_GRID_BKCOLOR       0xFF192431
#define UNBINDING_GRID_TEXTCOLOR     0xFFFFFFFF
#define UNBINDING_GRID_BORDERCOLOR   0xFFFFFFFF

#define HILIGHT_BINDING_GRID_BKCOLOR       0xFFCCCCCC
#define HILIGHT_BINDING_GRID_TEXTCOLOR     0xFF000000
#define HILIGHT_BINDING_GRID_BORDERCOLOR   0xFFCAF100


// 创建duilib控件的回调
class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CDialogBuilderCallbackEx(DuiLib::CPaintManagerUI *pManager) {
		m_pManager = pManager;
	}
	CControlUI* CreateControl(LPCTSTR pstrClass);

public:
	DuiLib::CPaintManagerUI *  m_pManager;
};

#define  MAX_AREA_NAME_LENGTH   64
typedef struct tagArea {
	char   szAreaName[MAX_AREA_NAME_LENGTH];
	DWORD  dwAreaNo;
}TArea;

#define  MAX_READER_NAME_LENGTH             20
// 术中读卡器
typedef struct tagReaderCfg
{
	BOOL      m_bSwitch;                              // 读卡器开关
	DWORD     m_dwLowTempAlarm;                       // 低温报警(单位：摄氏度)
	DWORD     m_dwHighTempAlarm;                      // 高温报警(单位：摄氏度)
	char      m_szReaderName[MAX_READER_NAME_LENGTH]; // 名称(对应body part name)
}ReaderCfg;

// 术中窗格
typedef struct tagGridCfg
{
	DWORD       m_dwCollectInterval;                    // 采集间隔(单位:秒)
	DWORD       m_dwMinTemp;                            // 显示的最低温度(单位:℃)
	DWORD       m_dwMaxTemp;                            // 显示的最高温度(单位:℃)
	DWORD       m_dwGridMode;                           // 手持0，单点连续1，多点连续2
	ReaderCfg   m_ReaderCfg[MAX_READERS_PER_GRID];      // 术中Reader配置
	ReaderCfg   m_HandReaderCfg;                        // 手持Reader配置
	char        m_szPatientName[MAX_TAG_PNAME_LENGTH];  // 病人姓名
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

#if TRI_TAGS_FLAG
// 有源tag绑定
typedef  struct tagBatteryBinding {
	char     m_szDeviceId[20];
	int      m_nIndex;
	int      m_nGridIndex;
} BatteryBinding;
#endif

#define   MAX_COM_PORTS_CNT            4

class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_launch;
	LmnToolkits::Thread *     m_thrd_sqlite;
	LmnToolkits::Thread *     m_thrd_work;
	LmnToolkits::Thread *     m_thrd_excel;
	// 方舱
	LmnToolkits::Thread *     m_thrd_launch_cube;
	LmnToolkits::Thread *     m_thrd_sqlite_cube;
	BOOL                      m_bMultipleComport;                 // 是否可以多串口数据(不管是还是不是，自动找寻460com)
	BOOL                      m_bSpecifiedComports;               // 是否使用指定串口
	int                       m_nComports[MAX_COM_PORTS_CNT];     // 使用哪些指定串口
	int                       m_nComportsCnt;
	int                       m_nCubeHighTemp;                    // 多少温度以上认为高温区

	CfgData                   m_CfgData;
	char                      m_szLaunchPort[16];      // 配置的，调试用的串口
	BOOL                      m_bClosing;              // 应用程序是否正在退出
	HWND                      m_hWnd;
	HCURSOR                   m_hCursor;
	CControlUI *              m_DragDropCtl;
	DuiLib::CEditUI *         m_edRemark;
	DuiLib::CEditUI *         m_edHandRemark;
	int                       m_nScreenWidth;
	int                       m_nScreenHeight;

	// 术中读卡器是否连接上
	BOOL                      m_bSurReaderConnected[MAX_GRID_COUNT][MAX_READERS_PER_GRID];

	// 温度曲线的垂直margin
	int                       m_nVMargin;
	// 体温单脚注
	char                      m_TempChartFooter[20];

	BOOL                      m_bDelta[3];
#if TRI_TAGS_FLAG
	BatteryBinding            m_battery_binding[MAX_GRID_COUNT];
	DWORD                     m_dwBatteryBindCnt;
#endif

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_launch = 0;
		m_thrd_sqlite = 0;
		m_thrd_work   = 0;
		m_thrd_excel  = 0;
		// 方舱
		m_thrd_launch_cube = 0;
		m_thrd_sqlite_cube = 0;
		m_bMultipleComport = FALSE;
		memset(m_nComports, 0, sizeof(m_nComports));
		m_nComportsCnt = 0;
		m_nCubeHighTemp = 0;

		m_bClosing = FALSE;	
		m_hWnd = 0;
		m_hCursor = 0;
		m_DragDropCtl = 0;
		m_edRemark = 0;
		m_edHandRemark = 0;
		memset(m_bSurReaderConnected, 0, sizeof(m_bSurReaderConnected));

		m_nVMargin = 0;
		m_nScreenWidth = 0;
		m_nScreenHeight = 0;
		memset(m_TempChartFooter, 0, sizeof(m_TempChartFooter));

		memset(m_bDelta, 0, sizeof(m_bDelta));
#if TRI_TAGS_FLAG
		memset(m_battery_binding, 0, sizeof(m_battery_binding));
		m_dwBatteryBindCnt = 0;
#endif
	}
};

enum  ReaderStatus {
	ReaderStatus_Open = 0,
	ReaderStatus_Close
};

#define  MAX_TAG_ID_LENGTH          20
#define  MAX_READER_ID_LENGTH       20
#define  MAX_REMARK_LENGTH          26
// 温度数据
typedef struct  tagTempItem {
	DWORD   m_dwDbId;                                        // 数据库的Id
	DWORD   m_dwTemp;                                        // 温度
	time_t  m_time;                                          // 时间
	char    m_szTagId[MAX_TAG_ID_LENGTH];                    // tag id
	char    m_szReaderId[MAX_READER_ID_LENGTH];              // reader id
	char    m_szRemark[MAX_REMARK_LENGTH];                   // 注释
}TempItem;

class CGetGridTempParam : public LmnToolkits::MessageData {
public:
	CGetGridTempParam(DWORD dwIndex) : m_dwIndex(dwIndex) { }
	DWORD     m_dwIndex;
};

class CSaveSurTempParam : public LmnToolkits::MessageData {
public:
	CSaveSurTempParam(WORD wBedNo, const TempItem & item) {
		memcpy(&m_item, &item, sizeof(TempItem));
		m_wBedNo = wBedNo;
	}

	TempItem       m_item;
	WORD           m_wBedNo;
};

class CSaveHandTempParam : public LmnToolkits::MessageData {
public:
	CSaveHandTempParam(const TempItem & item) {
		memcpy(&m_item, &item, sizeof(TempItem));
	}

	TempItem       m_item;
};

class CQueryTempByTagParam : public LmnToolkits::MessageData {
public:
	CQueryTempByTagParam(const char * szTagId, WORD wBedNo) {
		STRNCPY(m_szTagId, szTagId, sizeof(m_szTagId));
		m_wBedNo = wBedNo;
	}

	char       m_szTagId[MAX_TAG_ID_LENGTH];
	WORD       m_wBedNo;
};

class CGraphicsRoundRectPath : public GraphicsPath
{
public:
	void AddRoundRect(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY);
};

class CSaveRemarkParam : public LmnToolkits::MessageData {
public:
	CSaveRemarkParam(DWORD  dwDbId, const char * szRemark) {
		m_dwDbId = dwDbId;
		STRNCPY(m_szRemark, szRemark, MAX_REMARK_LENGTH);
	}

	DWORD       m_dwDbId;
	char        m_szRemark[MAX_REMARK_LENGTH];
};

class CSaveLastSurTagId : public LmnToolkits::MessageData {
public:
	CSaveLastSurTagId(WORD wBedId, const char * szTagId) {
		m_wBedId = wBedId;
		STRNCPY(m_szTagId, szTagId, MAX_TAG_ID_LENGTH);
	}

	char        m_szTagId[MAX_TAG_ID_LENGTH];
	WORD        m_wBedId;
};

typedef  struct  tagLastSurTagItem {
	char    m_szTagId[MAX_TAG_ID_LENGTH];
	WORD    m_wBedId;
}LastSurTagItem;


typedef  struct  tagTagPName {
	char    m_szPName[MAX_TAG_PNAME_LENGTH];
	time_t  m_time;
	int     m_nParam0;       // 用于保存额外信息(绑定的grid index，从1开始,0为没有绑定)
}TagPName;

// 查询手持Tag的温度数据结果集合
typedef  struct  tagHandTagResult {
	vector<TempItem *> *      m_pVec;
	char                      m_szTagId[MAX_TAG_ID_LENGTH];
	char                      m_szTagPName[MAX_TAG_PNAME_LENGTH];
	int                       m_nBindingGridIndex;                     // 绑定格子号1，2...64（不包含A~F）
}HandTagResult;

class CBindingTagGrid : public LmnToolkits::MessageData {
public:
	CBindingTagGrid(const char * szTagId, int nGridIndex) {
		STRNCPY(m_szTagId, szTagId, MAX_TAG_ID_LENGTH);
		m_nGridIndex = nGridIndex;
	}

	char        m_szTagId[MAX_TAG_ID_LENGTH];
	int         m_nGridIndex;
};

typedef struct tagTagBindingGridRet {
	char       m_szTagId[MAX_TAG_ID_LENGTH];
	int        m_nGridIndex;
	char       m_szOldTagId[MAX_TAG_ID_LENGTH];
	int        m_nOldGridIndex;
}TagBindingGridRet;

class CQueryTempByHandTagParam : public LmnToolkits::MessageData {
public:
	CQueryTempByHandTagParam(const char * szTagId, int nGridIndex) {
		STRNCPY(m_szTagId, szTagId, sizeof(m_szTagId));
		m_nGridIndex = nGridIndex;
	}

	char       m_szTagId[MAX_TAG_ID_LENGTH];
	int        m_nGridIndex;
};

class CRemoveGridBindingParam : public LmnToolkits::MessageData {
public:
	CRemoveGridBindingParam(int nGridIndex) {
		m_nGridIndex = nGridIndex;
	}

	int        m_nGridIndex;
};

class CSaveTagPNameParam : public LmnToolkits::MessageData {
public:
	CSaveTagPNameParam(const char * szTagId, const char * szPName) {
		STRNCPY(m_szTagId, szTagId, MAX_TAG_ID_LENGTH);
		STRNCPY(m_szPName, szPName, MAX_TAG_PNAME_LENGTH);
	}

	char        m_szTagId[MAX_TAG_ID_LENGTH];
	char        m_szPName[MAX_TAG_PNAME_LENGTH];
};

class CWriteTemp2ExcelParam : public LmnToolkits::MessageData {
public:
	CWriteTemp2ExcelParam(DWORD i, DWORD  j, const TempItem * pTemp, const char * szPName) {
		STRNCPY(m_szTagId, pTemp->m_szTagId, MAX_TAG_ID_LENGTH);
		STRNCPY(m_szPName, szPName,          MAX_TAG_PNAME_LENGTH);
		m_dwTemp = pTemp->m_dwTemp;
		m_time = pTemp->m_time;
		m_i = i;
		m_j = j;
	}

	char        m_szTagId[MAX_TAG_ID_LENGTH];
	char        m_szPName[MAX_TAG_PNAME_LENGTH];
	DWORD       m_dwTemp;
	time_t      m_time;
	DWORD       m_i;
	DWORD       m_j;
};

class CQueryBindingByTag : public LmnToolkits::MessageData {
public:
	CQueryBindingByTag(const char * szTagId) {
		STRNCPY(m_szTagId, szTagId, MAX_TAG_ID_LENGTH);
	}

	char        m_szTagId[MAX_TAG_ID_LENGTH];
};

#define  MAX_OUTPATIENT_NO_LENGTH           20
#define  MAX_HOSPITAL_ADMISSION_NO_LENGTH   20
#define  MAX_MEDICAL_DEPARTMENT_LENGTH      20
#define  MAX_WARD_LENGTH                    20
#define  MAX_BED_NO_LENGTH                  20
#define  MAX_AGE_LENGTH                     20

// 病人的基础信息
typedef  struct  tagPatientInfo {
	char        m_szTagId[MAX_TAG_ID_LENGTH];
	char        m_szPName[MAX_TAG_PNAME_LENGTH];

	int         m_sex;
	char        m_age[MAX_AGE_LENGTH];
	char        m_szOutpatientNo[MAX_OUTPATIENT_NO_LENGTH];
	char        m_szHospitalAdmissionNo[MAX_HOSPITAL_ADMISSION_NO_LENGTH];
	time_t      m_in_hospital;
	time_t      m_out_hospital;
	char        m_szMedicalDepartment[MAX_MEDICAL_DEPARTMENT_LENGTH];
	char        m_szWard[MAX_WARD_LENGTH];
	char        m_szBedNo[MAX_BED_NO_LENGTH];
	char        m_szMedicalDepartment2[MAX_MEDICAL_DEPARTMENT_LENGTH];
	char        m_szWard2[MAX_WARD_LENGTH];
	char        m_szBedNo2[MAX_BED_NO_LENGTH];
}PatientInfo;

#define  PTYPE_SURGERY          1
#define  PTYPE_BIRTH            2
#define  PTYPE_TURN_IN          3
#define  PTYPE_TURN_OUT         4
#define  PTYPE_DEATH            5
#define  PTYPE_HOLIDAY          6

#define  PTYPE_IN_HOSPITAL      100             // 入院
#define  PTYPE_OUT_HOSPITAL     101             // 出院

// 病人的事件信息
typedef  struct  tagPatientEvent {
	int         m_nId;
	char        m_szTagId[MAX_TAG_ID_LENGTH];
	int         m_nType;
	time_t      m_time_1;
	time_t      m_time_2;
}PatientEvent;

#define  MAX_BLOOD_PRESSURE_LENGTH 20
#define  MAX_WEIGHT_LENGTH         20
#define  MAX_IRRITABILITY_LENGTH   20
#define  MAX_BREATH_LENGTH         20
#define  MAX_DEFECATE_LENGTH       20
#define  MAX_URINE_LENGTH          20
#define  MAX_INCOME_LENGTH         20
#define  MAX_OUTPUT_LENGTH         20
// 病人的非体温数据
typedef  struct   tagPatientData {
	char        m_szTagId[MAX_TAG_ID_LENGTH];
	time_t      m_date;

	int         m_pulse[6];
	char        m_breath[6][MAX_BREATH_LENGTH];
	char        m_defecate[MAX_DEFECATE_LENGTH];
	char        m_urine[MAX_URINE_LENGTH];
	char        m_income[MAX_INCOME_LENGTH];
	char        m_output[MAX_OUTPUT_LENGTH];
	char        m_szBloodPressure[MAX_BLOOD_PRESSURE_LENGTH];
	char        m_szWeight[MAX_WEIGHT_LENGTH];
	char        m_szIrritability[MAX_IRRITABILITY_LENGTH];

	int         m_temp[6];
	int         m_descend_temp[6];      // 降温温度
}PatientData;


class CQueryPatientInfoParam : public LmnToolkits::MessageData {
public:
	CQueryPatientInfoParam(const char * szTagId) {
		STRNCPY(m_szTagId, szTagId, MAX_TAG_ID_LENGTH);
	}

	char         m_szTagId[MAX_TAG_ID_LENGTH];
};

class CQueryPatientDataParam : public LmnToolkits::MessageData {
public:
	CQueryPatientDataParam(const char * szTagId, time_t tFirstDay) {
		STRNCPY(m_szTagId, szTagId, MAX_TAG_ID_LENGTH);
		m_tFirstDay = tFirstDay;
	}

	char         m_szTagId[MAX_TAG_ID_LENGTH];
	time_t       m_tFirstDay;
};

class CSavePatientInfoParam : public LmnToolkits::MessageData {
public:
	CSavePatientInfoParam(const PatientInfo * pInfo) {
		memcpy(&m_info, pInfo, sizeof(PatientInfo));
	}

	PatientInfo         m_info;
};

class CSavePatientDataParam : public LmnToolkits::MessageData {
public:
	CSavePatientDataParam(const PatientData * pData) {
		memcpy(&m_data, pData, sizeof(PatientData));
	}

	PatientData     m_data;
};

class CDelTag : public LmnToolkits::MessageData {
public:
	CDelTag(const char * szTagId) {
		STRNCPY(m_szTagId, szTagId, MAX_TAG_ID_LENGTH);
	}

	char        m_szTagId[MAX_TAG_ID_LENGTH];
};

typedef struct tagGridEvent {
	int        m_nType;
	time_t     m_tTime;
}GridEvent;

// 住院信息
#define  MAX_QEVENTS_COUNT  10
typedef struct tagInHospitalItem {
	char         m_szTagId[MAX_TAG_ID_LENGTH];
	char         m_szPName[MAX_TAG_PNAME_LENGTH];
	int          m_sex;
	char         m_age[MAX_AGE_LENGTH];
	char         m_szOutpatientNo[MAX_OUTPATIENT_NO_LENGTH];
	char         m_szHospitalAdmissionNo[MAX_HOSPITAL_ADMISSION_NO_LENGTH];
	time_t       m_in_hospital;
	PatientEvent m_events[MAX_QEVENTS_COUNT];
	int          m_events_cnt;
}InHospitalItem;

typedef struct tagQueryInHospital {
	char         m_szPName[MAX_TAG_PNAME_LENGTH];
	int          m_sex;
	char         m_age[MAX_AGE_LENGTH];
	char         m_szOutpatientNo[MAX_OUTPATIENT_NO_LENGTH];
	char         m_szHospitalAdmissionNo[MAX_HOSPITAL_ADMISSION_NO_LENGTH];
	time_t       m_in_hospital_s;
	time_t       m_in_hospital_e;
}TQueryInHospital;

class CQueryInHospital : public LmnToolkits::MessageData {
public:
	CQueryInHospital(const TQueryInHospital * pQuery) {
		memcpy( &m_query, pQuery, sizeof(TQueryInHospital) );
	}

	TQueryInHospital        m_query;
};

typedef struct tagOutHospitalItem {
	char         m_szTagId[MAX_TAG_ID_LENGTH];
	char         m_szPName[MAX_TAG_PNAME_LENGTH];
	int          m_sex;
	char         m_age[MAX_AGE_LENGTH];
	char         m_szOutpatientNo[MAX_OUTPATIENT_NO_LENGTH];
	char         m_szHospitalAdmissionNo[MAX_HOSPITAL_ADMISSION_NO_LENGTH];
	time_t       m_in_hospital;
	time_t       m_out_hospital;
	PatientEvent m_events[MAX_QEVENTS_COUNT];
	int          m_events_cnt;
}OutHospitalItem;

typedef struct tagQueryOutHospital {
	char         m_szPName[MAX_TAG_PNAME_LENGTH];
	int          m_sex;
	char         m_age[MAX_AGE_LENGTH];
	char         m_szOutpatientNo[MAX_OUTPATIENT_NO_LENGTH];
	char         m_szHospitalAdmissionNo[MAX_HOSPITAL_ADMISSION_NO_LENGTH];
	time_t       m_in_hospital_s;
	time_t       m_in_hospital_e;
	time_t       m_out_hospital_s;
	time_t       m_out_hospital_e;
}TQueryOutHospital;

class CQueryOutHospital : public LmnToolkits::MessageData {
public:
	CQueryOutHospital(const TQueryOutHospital * pQuery) {
		memcpy(&m_query, pQuery, sizeof(TQueryOutHospital));
	}

	TQueryOutHospital        m_query;
};

#define  MAX_CUBE_NAME_LENGTH   20
#define  MAX_CUBE_PHONE_LENGTH  20

typedef struct tagCubeItem {
	int     nBedNo;
	char    szName[MAX_CUBE_NAME_LENGTH];
	char    szPhone[MAX_CUBE_PHONE_LENGTH];
	int     nTemp;
	time_t  time;
	char    szTagId[MAX_TAG_ID_LENGTH];
}CubeItem;

class CSaveCubeBedParam : public LmnToolkits::MessageData {
public:
	CSaveCubeBedParam(int nBedNo, const char * szName, const char * szPhone) {
		m_nBedNo = nBedNo;
		STRNCPY(m_szName,  szName,  MAX_CUBE_NAME_LENGTH);
		STRNCPY(m_szPhone, szPhone, MAX_CUBE_PHONE_LENGTH);
	}

	int     m_nBedNo;
	char    m_szName[MAX_CUBE_NAME_LENGTH];
	char    m_szPhone[MAX_CUBE_PHONE_LENGTH];
};

typedef struct tagCurTagData {
	char    szTagId[MAX_TAG_ID_LENGTH];
	int     nTemp;
	time_t  time;
}CurTagData;

class CCubeBindTagParam : public LmnToolkits::MessageData {
public:
	CCubeBindTagParam(int nBedNo, const char * szTagId) {
		m_nBedNo = nBedNo;
		STRNCPY(m_szTagId, szTagId, MAX_TAG_ID_LENGTH);
	}

	int     m_nBedNo;
	char    m_szTagId[MAX_TAG_ID_LENGTH];
};

class CCubeSaveTempParam : public LmnToolkits::MessageData {
public:
	CCubeSaveTempParam(int nBedNo, int nTemp, time_t time, BOOL bAdded, BOOL bChanged) {
		m_nBedNo = nBedNo;
		m_nTemp = nTemp;
		m_time = time;
		m_bAdded = bAdded;
		m_bChanged = bChanged;
	}

	int     m_nBedNo;
	int     m_nTemp;
	time_t  m_time;
	BOOL    m_bAdded;
	BOOL    m_bChanged;
};

class CCubeQueryTempParam : public LmnToolkits::MessageData {
public:
	CCubeQueryTempParam(int nBedNo) {
		m_nBedNo = nBedNo;
	}
	int     m_nBedNo;
};

// 方舱温度数据
typedef struct  tagCubeTempItem {
	int     m_nTemp;                                        // 温度
	time_t  m_time;                                         // 时间	
}CubeTempItem;

class CUpdateCubeItemParam : public LmnToolkits::MessageData {
public:
	CUpdateCubeItemParam(int nBedNo, const char * szName, const char * szPhone) {
		m_nBedNo = nBedNo;
		STRNCPY( m_szName,  szName,  MAX_CUBE_NAME_LENGTH );
		STRNCPY( m_szPhone, szPhone, MAX_CUBE_PHONE_LENGTH );
	}
	int     m_nBedNo;
	char    m_szName[MAX_CUBE_NAME_LENGTH];
	char    m_szPhone[MAX_CUBE_PHONE_LENGTH];
};

class CDismissBindingParam : public LmnToolkits::MessageData {
public:
	CDismissBindingParam(int nBedNo) {
		m_nBedNo = nBedNo;
	}
	int     m_nBedNo;
};

extern CGlobalData  g_data;
extern std::vector<TArea *>  g_vArea;
extern DWORD g_ReaderIndicator[MAX_READERS_PER_GRID];
extern const char * g_BodyParts[MAX_READERS_PER_GRID];
extern const char * g_BodyPart;

extern LONG WINAPI pfnUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);
extern DWORD GetGridOrderByGridId(DWORD  dwId);
extern char * GetDefaultGridOrder(char * buf, DWORD  dwBufLen, DWORD  dwGridsCnt);
extern char * GetGridOrder(char * buf, DWORD  dwBufLen, DWORD  dwGridsCnt, DWORD * pOrder);
void   ResetGridOrder();
extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern char * Time2String_hm(char * szDest, DWORD dwDestSize, const time_t * t);
extern char * Date2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern char * Date2String_md(char * szDest, DWORD dwDestSize, const time_t * t);
extern char * DateTime2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern char * DateTime2StringCn(char * szDest, DWORD dwDestSize, const time_t * t);
// 今天的零点时间
extern time_t  GetTodayZeroTime();
// 任意时间的当天零点时间
extern time_t  GetAnyDayZeroTime(time_t);
extern int  GetCh340Count(char * szComPort, DWORD dwComPortLen);
extern BOOL  CheckComPortExist(int nCheckComPort);
// 采集间隔时间
extern DWORD GetCollectInterval(DWORD dwIndex);
extern void  SaveReaderSwitch(DWORD i, DWORD j);
extern void  SaveReaderName(DWORD i, DWORD j);
extern void  SaveAlarmTemp(DWORD i, DWORD j, int mode);
extern char *  GetTagId(char * szTagId, DWORD dwTagIdLen, const BYTE * pData, DWORD dwDataLen);
extern char *  GetSurReaderId(char * szReaderId, DWORD dwReaderIdLen, const BYTE * pData, DWORD dwDataLen);
extern int  GetWeekDay(time_t t);
extern const char * GetWeekDayName(int nWeekIndex);
extern const char * GetWeekDayShortName(int nWeekIndex);
extern void  OnEdtRemarkKillFocus();
extern void  OnEdtHandRemarkKillFocus();
char *  GetHandReaderId(char * szReaderId, DWORD dwReaderIdLen, const BYTE * pData);
extern void  SavePatientName(DWORD i);
extern time_t  GetTimeDiff(time_t t1, time_t t2);
extern const char * GetSexStr(int nSex);
// 打印预览时显示数字(正数)
extern char * PreviewNum(char * buf, DWORD dwSize, int nNum);
extern void GetDateStr(char * year, DWORD d1, char * month, DWORD d2, 
	                   char * day, DWORD d3, time_t t);

extern void PrepareXmlChart( CXml2ChartFile & xmlChart, PatientInfo * pInfo,
	                         PatientData * pData, DWORD dwSize, time_t tFirstDay, 
	                         const std::vector<PatientEvent * > & vEvents);
extern void PrintXmlChart( HDC hDC, CXml2ChartFile & xmlChart, int nOffsetX, int nOffsetY, 
	                       PatientData * pData, DWORD dwDataSize, time_t tFirstDay, 
	                       const std::vector<PatientEvent * > & vEvents,
	                       const PatientInfo & patient_info);
extern void LoadXmlChart(CXml2ChartFile & xmlChart);
extern BOOL IsToday(const SYSTEMTIME & s);
extern char * Date2String(char * szDest, DWORD dwDestSize, const SYSTEMTIME & s);
extern time_t SysTime2Time(const SYSTEMTIME & s);
extern SYSTEMTIME Time2SysTime(const time_t & t);
extern DWORD GetIntFromDb(const char * szValue, int nDefault = 0);
extern char * GetStrFromdDb(char * buf, DWORD dwSize, const char * szValue);
extern char * Time2String_hm_cn(char * szDest, DWORD dwDestSize, const time_t * t);
extern void  SetGridEvent(GridEvent events_type2[6 * 7][2], int nIndex, int nType, time_t t);
extern void  SortGridEvent(GridEvent events_type2[6 * 7][2]);
extern void  ConvertGridEvent(GridEvent events_type2[6 * 7][2], GridEvent events_type[6 * 7]);
extern const char * GetEventTypeStr(int nType);
extern char *  VlcTime2Str(char * szTime, DWORD dwTimeSize, int nTime);

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



class CSavePatientEventsParam : public LmnToolkits::MessageData {
public:
	CSavePatientEventsParam(const char * szTagId, const std::vector<PatientEvent*> & vEvents) {
		STRNCPY(m_szTagId, szTagId, MAX_TAG_ID_LENGTH);
		std::vector<PatientEvent*>::const_iterator it;
		for (it = vEvents.begin(); it != vEvents.end(); ++it) {
			PatientEvent* p = *it;
			PatientEvent* pNew = new PatientEvent;
			memcpy(pNew, p, sizeof(PatientEvent));
			m_vEvents.push_back(pNew);
		}
	}

	~CSavePatientEventsParam() {
		ClearVector(m_vEvents);
	}

	char                            m_szTagId[MAX_TAG_ID_LENGTH];
	std::vector<PatientEvent*>      m_vEvents;
};