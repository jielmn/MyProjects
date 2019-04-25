#pragma once

#include <vector>
#include <algorithm>
#include <assert.h>
#include <time.h>

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

#define   MAX_GRID_COUNT           64             // ��༸������
#define   MAX_READERS_PER_GRID     6              // ÿ��������༸�����µ�
#define   MAX_COLUMNS_COUNT        5              // ÿ��ҳ�������
#define   MAX_ROWS_COUNT           5              // ÿ��ҳ�������
#define   DRAG_DROP_DELAY_TIME     100            // drag drop��������ʱʱ��

#define   VERSION                  "3.0.1"

// �ؼ�id
#define   TABS_ID                  "switch"
#define   TAB_INDEX_MONITOR        0
#define   TAB_INDEX_READER         1
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
#define   CFG_GRID_SWITCH                   "grid switch"
#define   DEFAULT_GRID_SWITCH               FALSE
#define   CFG_COLLECT_INTERVAL              "collect interval"
#define   MAX_COLLECT_INTERVAL_INDEX        5
#define   CFG_GRID_MIN_TEMP                 "grid min temperature"
#define   CFG_GRID_MAX_TEMP                 "grid max temperature"
#define   CFG_READER_SWITCH                 "reader switch"
#define   DEFAULT_READER_SWITCH             FALSE
#define   CFG_LOW_TEMP_ALARM                "low temperature alarm"
#define   CFG_HIGH_TEMP_ALARM               "high temperature alarm"

// thread��Ϣ


// windows ��Ϣ



// ����duilib�ؼ��Ļص�
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

#define  MAX_AREA_NAME_LENGTH   64
typedef struct tagArea {
	char   szAreaName[MAX_AREA_NAME_LENGTH];
	DWORD  dwAreaNo;
}TArea;

// ���ж�����
typedef struct tagReaderCfg
{
	BOOL      m_bSwitch;                             // ����������
	DWORD     m_dwLowTempAlarm;                      // ���±���(��λ�����϶�)
	DWORD     m_dwHighTempAlarm;                     // ���±���(��λ�����϶�)
}ReaderCfg;

// ���д���
typedef struct tagGridCfg
{
	BOOL        m_bSwitch;                              // �ܿ���
	DWORD       m_dwCollectInterval;                    // �ɼ����(��λ:��)
	DWORD       m_dwMinTemp;                            // ��ʾ������¶�(��λ:��)
	DWORD       m_dwMaxTemp;                            // ��ʾ������¶�(��λ:��)
	ReaderCfg   m_ReaderCfg[MAX_READERS_PER_GRID];
}GridCfg;

typedef struct tagCfgData {
	DWORD     m_dwAreaNo;                               // ���в�����
	DWORD     m_dwLayoutColumns;                        // ����
	DWORD     m_dwLayoutRows;                           // ����
	DWORD     m_dwLayoutGridsCnt;                       // ��λ����  (���ܳ���MAX_GRID_COUNT)
	BOOL      m_bAlarmVoiceOff;                         // ������������
	BOOL      m_bAutoSaveExcel;                         // �Զ�����Excel
	BOOL      m_bCrossAnchor;                           // ʮ��ê
	GridCfg   m_GridCfg[MAX_GRID_COUNT];                // ���и�������
	DWORD     m_GridOrder[MAX_GRID_COUNT];              // ����˳������: 2,1,3,4,5,6,30,7,8,9...

	DWORD     m_dwHandReaderMinTemp;                    // �ֳֶ�������ʾ����¶�
	DWORD     m_dwHandReaderMaxTemp;                    // �ֳֶ�������ʾ����¶�
	DWORD     m_dwHandReaderLowTempAlarm;               // �ֳֶ��������±���
	DWORD     m_dwHandReaderHighTempAlarm;              // �ֳֶ��������±���	
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
extern std::vector<TArea *>  g_vArea;
extern DWORD g_ReaderIndicator[MAX_READERS_PER_GRID];

extern LONG WINAPI pfnUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);
extern DWORD GetGridOrderByGridId(DWORD  dwId);
extern char * GetDefaultGridOrder(char * buf, DWORD  dwBufLen, DWORD  dwGridsCnt);
extern char * GetGridOrder(char * buf, DWORD  dwBufLen, DWORD  dwGridsCnt, DWORD * pOrder);
void   ResetGridOrder();
extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern char * Date2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern char * DateTime2String(char * szDest, DWORD dwDestSize, const time_t * t);

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