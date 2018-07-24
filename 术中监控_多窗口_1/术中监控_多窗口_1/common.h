#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#include "UIlib.h"
using namespace DuiLib;

#include "skin.h"

//#ifdef  _DEBUG
//#define  TEST_FLAG      1
//#define  
//#endif


/* 宏 */
#define   LOG_FILE_NAME           "SurgerySurveil.log"
#define   CONFIG_FILE_NAME        "SurgerySurveil.cfg"
#define   MAIN_FRAME_NAME         "DUIMainFrame_surgery"
#define   GLOBAL_LOCK_NAME        "surgery_surveil"
#define   MAIN_XML_FILE           "main.xml"
#define   SKIN_FOLDER             "res"
#define   MYCHART_XML_FILE        "MyChart.xml"
#define   MAX_GRID_COUNT           50

// config file configuration name
#define   CFG_TELNET_PORT          "telnet port"
#define   CFG_MAIN_LAYOUT_COLUMNS  "main layout columns"
#define   CFG_MAIN_LAYOUT_ROWS     "main layout rows"
#define   CFG_TIME_UNIT_WIDTH      "time unit length"
#define   CFG_MAX_POINTS_COUNT     "max points count"
#define   CFG_ALARM_FILE           "alarm file"
#define   CFG_LOW_ALARM            "low alarm"
#define   CFG_HIGH_ALARM           "high alarm"
#define   CFG_COLLECT_INTERVAL     "collect interval"
#define   CFG_MIN_TEMP             "min temperature degree"
#define   CFG_COM_PORT             "com port"
#define   CFG_MYIMAGE_LEFT_BLANK   "my image left"
#define   CFG_MYIMAGE_RIGHT_BLANK  "my image right"
#define   CFG_MYIMAGE_TIME_TEXT_OFFSET_X   "time text offset x"
#define   CFG_MYIMAGE_TIME_TEXT_OFFSET_Y   "time text offset y"

#define   DEFAULT_MAIN_LAYOUT_COLUMNS   4
#define   DEFAULT_MAIN_LAYOUT_ROWS      4
#define   DEFAULT_TIME_UNIT_WIDTH       50
#define   DEFAULT_MAX_POINTS_COUNT      1500
#define   DEFALUT_MYIMAGE_LEFT_BLANK    50
#define   DEFALUT_MYIMAGE_RIGHT_BLANK   50
#define   DEFAULT_ALARM_FILE_PATH       "\\res\\1.wav"
#define   DEFAULT_LOW_ALARM             3500
#define   DEFAULT_HI_ALARM              4200
#define   DEFAULT_COLLECT_INTERVAL      10          // 采集间隔时间，单位秒
#define   DEFAULT_MIN_TEMP              28
#define   DEFAULT_MYIMAGE_TIME_TEXT_OFFSET_X   1
#define   DEFAULT_MYIMAGE_TIME_TEXT_OFFSET_Y   1

#define   LAYOUT_WINDOW_NAME       "layWindow"
#define   LAYOUT_MAIN_NAME         "layMain"
#define   LAYOUT_STATUS_NAME       "layStatus"
#define   LABEL_INDEX_NAME         "lblIndex"
#define   LABEL_INDEX_SMALL_NAME   "lblIndex_1"
#define   LABEL_BED_SMALL_NAME     "lblBed_1"
#define   LABEL_NAME_SMALL_NAME    "lblName_1"
#define   LABEL_CUR_TEMP_SMALL_NAME    "lblCur_1"
#define   LABEL_BED_TITLE_SMALL_NAME     "lblBedTitle_1"
#define   LABEL_NAME_TITLE_SMALL_NAME    "lblNameTitle_1"
#define   LABEL_CUR_TEMP_TITLE_SMALL_NAME    "lblCurTitle_1"
#define   MYIMAGE_NAME                       "my_image"
#define   MYIMAGE_CLASS_NAME                 "MyImage"

#define   SKIN_BLACK                   0
#define   SKIN_WHITE                   1

#define   LAYOUT_MAIN_BK_COLOR_INDEX    0
#define   LABEL_STATUS_BK_COLOR_INDEX   1
#define   GRID_BORDER_COLOR_INDEX       2
#define   COMMON_TEXT_COLOR_INDEX       3

#define   COMMON_THREAD_INDEX           0
#define   TEMPERATURE_THREAD_INDEX      1

#define   TEMPERATURE_BRUSH_INDEX       0

#define   MYIMAGE_STATE_GRID             0
#define   MYIMAGE_STATE_MAXIUM           1

#define   MAX_ALARM_PATH_LENGTH          256
#define   MAX_COM_PORT_LENGTH            8

#define   MAX_TEMPERATURE                42
#define   MIN_MYIMAGE_VMARGIN            10              // 图像的上、下至少留出的空白

#define   DWORD2RGB(a)                   ( ( ((a) & 0xFF) << 16 ) | ( (a) & 0xFF00 ) | ( ((a) & 0xFF0000) >> 16  ) )

#define   MIN_TEXT_INTERVAL              120
#define   DEFAULT_POINT_RADIUS           6

/* 结构体 */
typedef struct tagTempData {
	DWORD    dwTemperature;
	time_t   tTime;
}TempData;

class CAlarmParam : public LmnToolkits::MessageData {
public:
	CAlarmParam(const char * szAlarmFile) {
		strncpy_s(m_szAlarmFile, szAlarmFile, sizeof(m_szAlarmFile));
	}
	~CAlarmParam() {}
	char    m_szAlarmFile[MAX_ALARM_PATH_LENGTH];
};


/* 全局变量 */
extern ILog    * g_log;
extern IConfig * g_cfg;
//extern LmnToolkits::Thread *  g_thrd_db;
extern DWORD     g_dwLayoutColumns;
extern DWORD     g_dwLayoutRows;
extern DWORD     g_dwTimeUnitWidth;
extern DWORD     g_dwMaxPointsCount;
extern DWORD     g_dwMyImageLeftBlank;
extern DWORD     g_dwMyImageRightBlank;
extern DWORD     g_dwMyImageTimeTextOffsetX;
extern DWORD     g_dwMyImageTimeTextOffsetY;
extern DWORD     g_dwCollectInterval[MAX_GRID_COUNT];
extern DWORD     g_dwMyImageMinTemp[MAX_GRID_COUNT];
extern DWORD     g_dwLowTempAlarm[MAX_GRID_COUNT];
extern DWORD     g_dwHighTempAlarm[MAX_GRID_COUNT];
extern BOOL      g_bAlarmOff;   // 报警开关是否打开
extern char      g_szAlarmFilePath[MAX_ALARM_PATH_LENGTH];
extern char      g_szComPort[MAX_ALARM_PATH_LENGTH][MAX_COM_PORT_LENGTH];
extern CMySkin   g_skin;


/* 函数 */
extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern DuiLib::CControlUI* CALLBACK MY_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData);
extern char * GetDefaultAlarmFile(char * szDefaultFile, DWORD dwSize);

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