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

#ifdef   _DEBUG
#define  TEST_FLAG                1
#endif

#if TEST_FLAG
#define  TIMER_TEST_ID            1
#define  TIMER_TEST_INTERVAL      5000
#endif


/* 宏 */
#define   LOG_FILE_NAME           "SurgerySurveil.log"
#define   CONFIG_FILE_NAME        "SurgerySurveil.cfg"
#define   MAIN_FRAME_NAME         "DUIMainFrame_surgery"
#define   SETTING_FRAME_NAME      "DUISettingFrame"
#define   GLOBAL_LOCK_NAME        "surgery_surveil"
#define   MAIN_XML_FILE           "main.xml"
#define   SETTING_FILE            "Setting.xml"
#define   SKIN_FOLDER             "res"
#define   MYCHART_XML_FILE        "MyChart.xml"
#define   MAX_GRID_COUNT           50
#define   SKIN_BLACK               0
#define   SKIN_WHITE               1

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
#define   CFG_MYIMAGE_TEMP_TEXT_OFFSET_X   "temperature text offset x"
#define   CFG_MYIMAGE_TEMP_TEXT_OFFSET_Y   "temperature text offset y"
#define   CFG_ALARM_VOICE_SWITCH           "alarm voice switch"
#define   CFG_SKIN                         "skin"

#define   DEFAULT_MAIN_LAYOUT_COLUMNS   4
#define   DEFAULT_MAIN_LAYOUT_ROWS      4
#define   DEFAULT_TIME_UNIT_WIDTH       80
#define   DEFAULT_MAX_POINTS_COUNT      1500
#define   DEFALUT_MYIMAGE_LEFT_BLANK    40
#define   DEFALUT_MYIMAGE_RIGHT_BLANK   40
#define   DEFAULT_ALARM_FILE_PATH       "\\res\\1.wav"
#define   DEFAULT_LOW_ALARM             3500
#define   DEFAULT_HI_ALARM              4000
#define   DEFAULT_COLLECT_INTERVAL      10          // 采集间隔时间，单位秒
#define   DEFAULT_MIN_TEMP              28
#define   DEFAULT_MYIMAGE_TIME_TEXT_OFFSET_X   1
#define   DEFAULT_MYIMAGE_TIME_TEXT_OFFSET_Y   1
#define   DEFAULT_MYIMAGE_TEMP_TEXT_OFFSET_X   -35
#define   DEFAULT_MYIMAGE_TEMP_TEXT_OFFSET_Y   -8
#define   DEFAULT_ALARM_VOICE_SWITCH           FALSE

#if 0
#define   DEFAULT_SKIN                         SKIN_BLACK
#else
#define   DEFAULT_SKIN                         SKIN_WHITE
#endif

#define   LAYOUT_WINDOW_NAME       "layWindow"
#define   LAYOUT_MAIN_NAME         "layMain"
#define   LAYOUT_STATUS_NAME       "layStatus"
#define   LABEL_INDEX_SMALL_NAME   "lblIndex_1"
#define   LABEL_BED_SMALL_NAME     "lblBed_1"
#define   LABEL_NAME_SMALL_NAME    "lblName_1"
#define   LABEL_CUR_TEMP_SMALL_NAME    "lblCur_1"
#define   LABEL_BED_TITLE_SMALL_NAME     "lblBedTitle_1"
#define   LABEL_NAME_TITLE_SMALL_NAME    "lblNameTitle_1"
#define   LABEL_CUR_TEMP_TITLE_SMALL_NAME    "lblCurTitle_1"
#define   MYIMAGE_NAME                       "my_image"
#define   MYIMAGE_CLASS_NAME                 "MyImage"
#define   BTN_MENU_NAME                      "menubtn"
#define   MYTREE_CONFIG_NAME                 "CfgTree"
#define   MYTREE_CLASS_NAME                  "MyTree"
#define   BUTTON_OK_NAME                     "btnOK"



#define   LAYOUT_MAIN_BK_COLOR_INDEX    0
#define   LABEL_STATUS_BK_COLOR_INDEX   1
#define   GRID_BORDER_COLOR_INDEX       2
#define   COMMON_TEXT_COLOR_INDEX       3
#define   MYIMAGE_BK_COLOR_INDEX        4
#define   MYIMAGE_TEMP_THREAD_COLOR_INDEX  5
#define   MYIMAGE_TEMP_DOT_COLOR_INDEX     6

#define   COMMON_PEN_INDEX              0
#define   COMMON_BRUSH_INDEX            1
#define   BRIGHTER_PEN_INDEX            2
#define   LOW_TEMP_PEN_INDEX            3
#define   HIGH_TEMP_PEN_INDEX           4

#define   STATE_GRIDS                    0
#define   STATE_MAXIUM                   1

#define   MAX_ALARM_PATH_LENGTH          256
#define   MAX_COM_PORT_LENGTH            8

#define   MAX_TEMPERATURE                42
#define   MIN_MYIMAGE_VMARGIN            15              // 图像的上、下至少留出的空白

#define   DWORD2RGB(a)                   ( ( ((a) & 0xFF) << 16 ) | ( (a) & 0xFF00 ) | ( ((a) & 0xFF0000) >> 16  ) )

#define   MIN_TEXT_INTERVAL              120
#define   DEFAULT_POINT_RADIUS           6

#define   RADIUS_SIZE_IN_GRID            3  
#define   RADIUS_SIZE_IN_MAXIUM          6

#define   MIN_TEMP_V_INTERVAL            30

#define   LOW_TEMP_ALARM_TEXT            "低温报警"
#define   HIGH_TEMP_ALARM_TEXT           "高温报警"
#define   LOW_TEMP_ALARM_TEXT_OFFSET_X   5
#define   LOW_TEMP_ALARM_TEXT_OFFSET_Y   5
#define   HIGH_TEMP_ALARM_TEXT_OFFSET_X   5
#define   HIGH_TEMP_ALARM_TEXT_OFFSET_Y   -20

#define   GRID_NAME                       "MyGrid"

#define   LAYOUT_WINDOW_HMARGIN           10
#define   LAYOUT_WINDOW_VMARGIN           1
#define   STATUS_PANE_HEIGHT              30
#define   WINDOW_TITLE_HEIGHT             32

#define   COMMON_SETTING_TEXT             "通用设置"
#define   GRIDS_COLUMNS_TEXT              "窗格列数"
#define   GRIDS_ROWS_TEXT                 "窗格行数"
#define   ALARM_VOICE_SWITCH_TEXT         "报警声音开关"
#define   SWITCH_ON_TEXT                  "开"
#define   SWITCH_OFF_TEXT                 "关"
#define   GRIDS_SETTING_TEXT              "窗格"
#define   COLLECT_INTERVAL_TEXT           "采样间隔"
#define   MIN_TEMPERATURE_TEXT            "显示的最低温度"
#define   LOW_ALARM_TEXT                  "低温报警"
#define   HIGH_ALARM_TEXT                 "高温报警"
#define   ANY_COM_PORT_TEXT               "任意端口"
#define   RW_COM_PORT_TEXT                "读写串口"
#define   SKIN_BLACK_TEXT                 "黑曜石"
#define   SKIN_WHITE_TEXT                 "白宣纸"

#define UM_UPDATE_SCROLL                 (WM_USER+1)

#define MSG_UPDATE_SCROLL                 1001

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

class CUpdateScrollParam : public LmnToolkits::MessageData {
public:
	CUpdateScrollParam(int nIndex) : m_nIndex ( nIndex ) { }
	int     m_nIndex;
};

class CDuiMenu : public DuiLib::WindowImplBase
{
protected:
	virtual ~CDuiMenu() {};        // 私有化析构函数，这样此对象只能通过new来生成，而不能直接定义变量。就保证了delete this不会出错
	DuiLib::CDuiString  m_strXMLPath;
	DuiLib::CControlUI * m_pOwner;

public:
	explicit CDuiMenu(LPCTSTR pszXMLPath, DuiLib::CControlUI * pOwner) : m_strXMLPath(pszXMLPath), m_pOwner(pOwner) {}
	virtual LPCTSTR    GetWindowClassName()const { return _T("CDuiMenu "); }
	virtual DuiLib::CDuiString GetSkinFolder() { return _T(""); }
	virtual DuiLib::CDuiString GetSkinFile() { return m_strXMLPath; }
	virtual void       OnFinalMessage(HWND hWnd) { delete this; }

	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void Init(HWND hWndParent, POINT ptPos);
	virtual void  Notify(DuiLib::TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

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


/* 全局变量 */
extern ILog    * g_log;
extern IConfig * g_cfg;
extern HWND    g_hWnd;
//extern LmnToolkits::Thread *  g_thrd_db;
extern LmnToolkits::Thread *  g_thrd_work;
extern LmnToolkits::Thread *  g_thrd_reader[MAX_GRID_COUNT];
extern DWORD     g_dwLayoutColumns;
extern DWORD     g_dwLayoutRows;
extern DWORD     g_dwTimeUnitWidth;
extern DWORD     g_dwMaxPointsCount;
extern DWORD     g_dwMyImageLeftBlank;
extern DWORD     g_dwMyImageRightBlank;
extern DWORD     g_dwMyImageTimeTextOffsetX;
extern DWORD     g_dwMyImageTimeTextOffsetY;
extern DWORD     g_dwMyImageTempTextOffsetX;
extern DWORD     g_dwMyImageTempTextOffsetY;
extern DWORD     g_dwCollectInterval[MAX_GRID_COUNT];
extern DWORD     g_dwMyImageMinTemp[MAX_GRID_COUNT];
extern DWORD     g_dwLowTempAlarm[MAX_GRID_COUNT];
extern DWORD     g_dwHighTempAlarm[MAX_GRID_COUNT];
extern BOOL      g_bAlarmOff;   // 报警开关是否打开
extern char      g_szAlarmFilePath[MAX_ALARM_PATH_LENGTH];
extern char      g_szComPort[MAX_ALARM_PATH_LENGTH][MAX_COM_PORT_LENGTH];
extern CMySkin   g_skin;
extern BOOL      g_bAlarmVoiceOff;
extern DWORD     g_dwSkinIndex;


/* 函数 */
extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern DuiLib::CControlUI* CALLBACK MY_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData);
extern char * GetDefaultAlarmFile(char * szDefaultFile, DWORD dwSize);
extern BOOL GetAllSerialPortName(std::vector<std::string> & vCom);

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