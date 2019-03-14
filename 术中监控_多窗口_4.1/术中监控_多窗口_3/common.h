#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"
#include "skin.h"
#include "mysql.h"

#include "UIlib.h"
using namespace DuiLib;

#ifdef   _DEBUG
#define  TEST_FLAG                0
#define  TEST_FLAG_1              0
#define  TEST_FLAG_2              0
#endif

#if TEST_FLAG
#define  TIMER_TEST_ID            1
#define  TIMER_TEST_INTERVAL      5000

#define  TIMER_TEST_ID_1          2
#define  TIMER_TEST_INTERVAL_1    7000
#endif

#define  TIMER_UPDATE_TIME_DESC             100
#define  TIMER_UPDATE_TIME_DESC_INTERVAL    10000

#define  TIMER_CHECK_CONFLICT               101
#define  TIMER_CHECK_CONFLICT_INTERVAL      30000

#define  TIMER_CHECK_AUTO_SAVE              102
#define  TIMER_CHECK_AUTO_SAVE_INTERVAL     60000

#define  TIMER_AUTO_PRUNE                   103
#define  TIMER_AUTO_PRUNE_INTERVAL          10000


#define  DB_FLAG  0
#define  DBG_FLAG 1

#define  MAX_POINTS_COUNT         60481

#define   GLOBAL_LOCK_NAME        "surgery_surveil_3"
#define   LOG_FILE_NAME           "surgery3.log"
#define   CONFIG_FILE_NAME        "surgery3.cfg"
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               "mainframe.xml"
#define   SKIN_FOLDER             "res\\surgery_res_3"
#define   MAX_GRID_COUNT          30
#define   MAX_READERS_PER_GRID    6
#define   READER_FILE_NAME        "reader.xml"
#define   SETTING_FRAME_NAME      "DUISettingFrame"
#define   SETTING_FILE            "Setting.xml"
#define   MAX_AREA_COUNT          20
#define   AREA_CFG_FILE_NAME      "area.cfg"
#define   MAX_AREA_ID             100
#define   MAX_BED_LENGTH          16
#define   MAX_NAME_LENGTH         16
#define   BINDING_FRAME_NAME      "DUIBindingFrame"
#define   BINDING_FILE            "Binding.xml"

// 控件相关
#define   LAYOUT_MAIN_NAME         "layMain"
#define   GRID_NAME                "MyGrid"
#define   MYCHART_XML_FILE         "MyChart.xml"
#define   LAYOUT_STATUS_NAME       "layStatus"
#define   LABEL_STATUS_NAME        "lblLaunchStatus"
#define   MYIMAGE_CLASS_NAME       "MyImage"
#define   MYIMAGE_CLASS_NAME1      "MyImage1"
#define   ALARM_IMAGE_CLASS_NAME   "AlarmImage"
#define   LAYOUT_GRID_NAME         "lay_1"
#define   LAYOUT_MAXIUM_NAME       "lay_2"
#define   LABEL_INDEX_NAME_GRID    "lblIndex_1"
#define   LABEL_INDEX_NAME_MAXIUM  "lblIndex_2"
#define   BUTTON_BED_NAME_GRID     "btnBed_1"
#define   EDIT_BED_NAME_GRID       "edtBed_1"
#define   BUTTON_NAME_NAME_GRID    "btnName_1"
#define   EDIT_NAME_NAME_GRID      "edtName_1"
#define   LABEL_CUR_TEMP_GRID      "lblCur_1"
#define   LABEL_CUR_TEMP_GRID1     "lblCur_2"
#define   LABEL_BED_TITLE_GRID     "lblBedTitle_1"
#define   LABEL_NAME_TITLE_GRID    "lblNameTitle_1"
#define   LABEL_CUR_TITLE_GRID     "lblCurTitle_1"
#define   MYIMAGE_NAME             "my_image_1"
#define   MYIMAGE_NAME_MAXIUM      "my_image_2"
#define   OPT_GRID_SWITCH_GRID     "opn_grid_switch_1"
#define   ALARM_NAME               "alarm_1"
#define   OPT_GRID_SWITCH_MAX      "opn_grid_switch_2"
#define   BUTTON_BED_NAME_MAX      "btnBed_2"
#define   EDIT_BED_NAME_MAX        "edtBed_2"
#define   BUTTON_NAME_NAME_MAX     "btnName_2"
#define   EDIT_NAME_NAME_MAX       "edtName_2"
#define   LAYOUT_READERS           "layReaders"
#define   CTL_INDICATOR_NAME       "indicator"
#define   LAY_READER_NAME          "layReader"
#define   READER_SWITCH_NAME       "opn_reader_switch"
#define   READER_TEMP_NAME         "lblReaderTemp"
#define   BTN_READER_NAME          "btnReaderName"
#define   EDT_READER_NAME          "edtReaderName"
#define   ALARM_IMAGE_NAME         "alarm_image"
#define   BTN_MENU_NAME            "menubtn"
#define   MYTREE_CONFIG_NAME       "CfgTree"
#define   MYTREE_CLASS_NAME        "MyTree"
#define   LAYOUT_WINDOW_HMARGIN    10
#define   LAYOUT_WINDOW_VMARGIN    1
#define   STATUS_PANE_HEIGHT       30
#define   WINDOW_TITLE_HEIGHT      32

// CONFIG
#define   CFG_MAIN_LAYOUT_COLUMNS       "main layout columns"
#define   CFG_MAIN_LAYOUT_ROWS          "main layout rows"
#define   DEFAULT_MAIN_LAYOUT_COLUMNS   2
#define   DEFAULT_MAIN_LAYOUT_ROWS      2
#define   CFG_AREA_ID_NAME              "area id"
#define   CFG_ALARM_VOICE_SWITCH        "alarm voice switch"
#define   DEFAULT_ALARM_VOICE_SWITCH    FALSE
#define   CFG_SKIN                      "skin"
#define   DEFAULT_SKIN                  0
#define   CFG_AREA_NAME                 "area name"
#define   CFG_AREA_NO                   "area no"
#define   CFG_GRID_SWITCH               "grid switch"
#define   DEFAULT_READER_SWITCH         FALSE
#define   CFG_COLLECT_INTERVAL          "collect interval"
#define   CFG_MIN_TEMP                  "min temperature"
#define   CFG_MAX_TEMP                  "max temperature"
#define   CFG_READER_SWITCH             "reader switch"
#define   CFG_LOW_TEMP_ALARM            "low temperature alarm"
#define   CFG_HIGH_TEMP_ALARM           "high temperature alarm"
#define   CFG_BED_NO                    "bed no"
#define   DEFAULT_MIN_TEMP_INDEX        2
#define   DEFAULT_MAX_TEMP_INDEX        1
#define   CFG_BED_NAME                  "bed name"
#define   CFG_PATIENT_NAME              "patient name"
#define   CFG_READER_NAME               "reader name"
#define   CFG_TEMP_FONT                 "temperature font"
#define   DEFAULT_TEMP_FONT             13

// 其他
#define   MAX_TEMPERATURE                42
#define   MIN_MYIMAGE_VMARGIN            40              // 图像的上、下至少留出的空白
#define   MYIMAGE_LEFT_BLANK             50
#define   MYIMAGE_RIGHT_BLANK            50
#define   MIN_TEMP_V_INTERVAL            30
#define   MAX_REMARK_LENGTH              28
#define   DEFAULT_POINT_RADIUS           6
#define   EDT_REMARK_WIDTH               200
#define   EDT_REMARK_HEIGHT              30
#define   EDT_REMARK_Y_OFFSET            -50
#define   DEFAULT_COLLECT_INTERVAL       10
#define   RADIUS_SIZE_IN_GRID            4
#define   RADIUS_SIZE_IN_MAXIUM          4
#define   DEFAULT_ALARM_FILE_PATH        "\\res\\surgery_res_3\\1.wav"
#define   RECONNECT_LAUNCH_TIME_INTERVAL 30000
#define   READER_STATUS_CLOSE            0
#define   READER_STATUS_OPEN             1
#define   WRITE_SLEEP_TIME               1100
#define   READ_INTERVAL_TIME             1000

// message
#define MSG_UPDATE_SCROLL                1001
#define MSG_ALARM                        1002
#define MSG_RECONNECT_LAUNCH             1003
#define MSG_PRINT_STATUS                 1004
#define MSG_GET_TEMPERATURE              1005
#define MSG_READ_LAUNCH                  1006
#define MSG_CHECK_LAUNCH_STATUS          1008
#define MSG_RECONNECT_DB                 1009
#define MSG_QUERY_BINDING                2000
#define MSG_QUERY_BINDING_MAX            2999
#define MSG_DB_HEARTBEAT                 3000
#define MSG_SAVE_TEMP                    3001
#define MSG_GET_ALL_PATIENTS             3002
#define MSG_BIND_TAGS                    3003
#define MSG_GET_GRID_TEMP                3004
#define MSG_GET_GRID_TEMP_MAX            3999
#define MSG_SAVE_TEMP_SQLITE             4000
#define MSG_QUERY_TEMP_SQLITE            4001
#define MSG_SAVE_REMARK_SQLITE           4002
#define MSG_QUERY_HAND_READER_TEMP_SQLITE   4003

#define UM_UPDATE_SCROLL                 (WM_USER+1)
#define UM_LAUNCH_STATUS                 (WM_USER+2)
#define UM_READER_TEMP                   (WM_USER+3)
#define UM_READER_DISCONNECTED           (WM_USER+4)
#define UM_READER_PROCESSING             (WM_USER+5)
#define UM_DB_STATUS                     (WM_USER+6)
#define UM_QUERY_TAG_BINDING_RET         (WM_USER+7)
#define UM_GET_ALL_PATIENTS_RET          (WM_USER+8)
#define UM_BIND_TAGS_RET                 (WM_USER+9)
#define UM_QUERY_TEMP_SQLITE_RET         (WM_USER+10)
#define UM_QUERY_HAND_READER_TEMP_SQLITE_RET         (WM_USER+11)
#define UM_HAND_READER_TEMP                          (WM_USER+12)

#define PRINT_EXCEL_MAX_POINTS_COUNT      100
#define GET_TEMPERATURE_TIMEOUT           2000

// 类
typedef struct tagTempData {
	DWORD    dwIndex;
	DWORD    dwTemperature;
	time_t   tTime;
	char     szRemark[MAX_REMARK_LENGTH];
}TempData;

class CDuiFrameWnd;
class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CDialogBuilderCallbackEx(DuiLib::CPaintManagerUI *pManager, CDuiFrameWnd * pMainWnd) {
		m_pManager = pManager;
		m_pMainWnd = pMainWnd;
	}
	CControlUI* CreateControl(LPCTSTR pstrClass);
private:
	DuiLib::CPaintManagerUI *  m_pManager;
	CDuiFrameWnd *             m_pMainWnd;
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

typedef struct tagReaderCfg
{
	BOOL      m_bSwitch;
	DWORD     m_dwLowTempAlarm;
	DWORD     m_dwHighTempAlarm;
	DWORD     m_dwBed;
	char      m_szName[32];
}ReaderCfg;

typedef struct tagGridCfg
{
	BOOL        m_bSwitch;
	DWORD       m_dwCollectInterval;
	DWORD       m_dwMinTemp;
	DWORD       m_dwMaxTemp;
	char        m_szBed[MAX_BED_LENGTH];
	char        m_szName[MAX_NAME_LENGTH];
	ReaderCfg   m_ReaderCfg[MAX_READERS_PER_GRID];
}GridCfg;

typedef struct tagCfgData {
	DWORD     m_dwAreaNo;
	DWORD     m_dwLayoutColumns;
	DWORD     m_dwLayoutRows;
	DWORD     m_dwSkinIndex;
	BOOL      m_bAlarmVoiceOff;
	DWORD     m_dwTempFont;
	BOOL      m_bAutoSaveExcel;
	BOOL      m_bCrossAnchor;         // 十字锚
	DWORD     m_dwHandReaderMinTemp;
	DWORD     m_dwHandReaderMaxTemp;
	DWORD     m_dwHandReaderLowTempAlarm;
	DWORD     m_dwHandReaderHighTempAlarm;
	GridCfg   m_GridCfg[MAX_GRID_COUNT];
}CfgData;

class  CGlobalData {
public:
	HWND      m_hWnd;
	ILog    * m_log;
	IConfig * m_cfg;
	CMySkin   m_skin;

	CfgData   m_CfgData;
	DWORD     m_dwCollectIntervalWidth;
	ARGB      m_argb[MAX_READERS_PER_GRID];
	char      m_szLaunchPort[64];
	BOOL      m_bAutoScroll;                           // 自动更新滑动条
	char      m_szDbHost[64];                          // mysql host
	char      m_szDbUser[64];
	char      m_szDbPwd[64];
	BOOL      m_bUtf8;                                 // 是否utf8编码	
	BOOL      m_bCurve;                                // 线条是否曲线
};

#define  MAX_AREA_NAME_LENGTH   64
typedef struct tagArea {
	char   szAreaName[MAX_AREA_NAME_LENGTH];
	DWORD  dwAreaNo;
}TArea;

class CGraphicsRoundRectPath : public GraphicsPath
{

public:
	CGraphicsRoundRectPath();

public:
	void AddRoundRect(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY);
};

class CUpdateScrollParam : public LmnToolkits::MessageData {
public:
	CUpdateScrollParam(int nIndex) : m_nIndex(nIndex) { }
	int     m_nIndex;
};

class CTemperatureParam : public LmnToolkits::MessageData {
public:
	CTemperatureParam(DWORD dwIndex, DWORD dwSubIndex) : m_dwIndex(dwIndex), m_dwSubIndex(dwSubIndex) { }
	DWORD     m_dwIndex;
	DWORD     m_dwSubIndex;
};

class CGridTempParam : public LmnToolkits::MessageData {
public:
	CGridTempParam(DWORD dwIndex) : m_dwIndex(dwIndex) { }
	DWORD     m_dwIndex;
};

class CQueryBindingParam : public LmnToolkits::MessageData {
public:
	CQueryBindingParam(DWORD dwIndex, DWORD dwSubIndex, 
		const char * szTagId) : m_dwIndex(dwIndex), m_dwSubIndex(dwSubIndex) {
		STRNCPY(m_szTagId, szTagId, sizeof(m_szTagId) );
	}
	DWORD     m_dwIndex;
	DWORD     m_dwSubIndex;
	char      m_szTagId[20];
};

typedef struct tagReaderStatus {
	BOOL     m_bConnected;
	DWORD    m_dwTryCnt;
	DWORD    m_dwLastQueryTick;
	DWORD    m_dwLastTemp;
	BOOL     m_bChecked;
}ReaderStatus;

// 上一次读取的温度
typedef struct  tagLastTemp {
	DWORD   m_dwTemp;                      // 温度
	time_t  m_Time;                        // 时间
	char    m_szTagId[20];                 // tag id
	char    m_szReaderId[20];              // Reader id
}LastTemp;

typedef struct tagTagBinding {
	BOOL     m_bGetBindingRet;             // 有无查询到绑定结果
	DWORD    m_dwPatientId;                // 0表示还没有绑定病人
	char     m_szPatientName[32];
	char     m_szTagName[32];
	char     m_szTagId[20];
}TagBinding;

typedef struct tagHandReaderTemp {
	DWORD   m_dwTemp;                      // 温度
	char    m_szTagId[20];                 // tag id
	char    m_szReaderId[20];              // Reader id
	char    m_szCardId[20];                // card id
}HandReaderTemp;

typedef struct tagTagControlItem {
	CControlUI *    m_Control;
	string *        m_pTagId;
}TagControlItem;

class CSaveTempParam : public LmnToolkits::MessageData {
public:
	CSaveTempParam(DWORD dwIndex, DWORD dwSubIndex, const LastTemp * pTemp)
		: m_dwIndex(dwIndex), m_dwSubIndex(dwSubIndex) {
		memcpy(&m_tTemp, pTemp, sizeof(LastTemp));
	}
	DWORD     m_dwIndex;
	DWORD     m_dwSubIndex;
	LastTemp  m_tTemp;
};

typedef  struct  tagBodyPart {
	char      m_szName[20];
}BodyPart;

class CGetPatientsParam : public LmnToolkits::MessageData {
public:
	CGetPatientsParam(HWND hWnd, DWORD dwPatientId)
		: m_hWnd(hWnd), m_dwPatientId(dwPatientId) {}
	HWND   m_hWnd;
	DWORD  m_dwPatientId;
};

typedef struct  tagPatient {
	char    m_szId[20];
	DWORD   m_dwPatientId;
	char    m_szName[20];
	BOOL    m_bMale;
	DWORD   m_dwAge;
}Patient;

typedef struct tagTagBinding_1 {
	char     m_szTagName[32];
	char     m_szTagId[20];
}TagBinding_1;

class CBindTagsParam : public LmnToolkits::MessageData {
public:
	CBindTagsParam( HWND hWnd, DWORD dwPatientId, const TagBinding_1 * items, 
		DWORD dwTagsCnt) : m_hWnd(hWnd),  m_dwPatientId(dwPatientId), 
		m_dwItemsCnt(dwTagsCnt) {
		assert(dwTagsCnt <= MAX_READERS_PER_GRID);
		memcpy(m_items, items, sizeof(TagBinding_1) * dwTagsCnt);
	}

	HWND           m_hWnd;
	DWORD          m_dwPatientId;
	TagBinding_1   m_items[MAX_READERS_PER_GRID];
	DWORD          m_dwItemsCnt;
};

class CSaveTempSqliteParam : public LmnToolkits::MessageData {
public:
	CSaveTempSqliteParam(const char * szTagId, DWORD dwTemp, time_t t){
		STRNCPY(m_szTagId, szTagId, sizeof(m_szTagId));
		m_dwTemp = dwTemp;
		m_time = t;
	}

	char           m_szTagId[20];
	DWORD          m_dwTemp;
	time_t         m_time;
};

class CQueryTempSqliteParam : public LmnToolkits::MessageData {
public:
	CQueryTempSqliteParam(const char * szTagId, DWORD dwIndex, DWORD  dwSubIndex) {
		STRNCPY(m_szTagId, szTagId, sizeof(m_szTagId));
		m_dwIndex = dwIndex;
		m_dwSubIndex = dwSubIndex;
	}

	char           m_szTagId[20];
	DWORD          m_dwIndex;
	DWORD          m_dwSubIndex;
};

class CSetRemarkSqliteParam : public LmnToolkits::MessageData {
public:
	CSetRemarkSqliteParam(const char * szTagId, time_t tTime, const char * szRemark) {
		STRNCPY(m_szTagId, szTagId, sizeof(m_szTagId));
		m_tTime = tTime;
		if (szRemark) {
			STRNCPY(m_szRemark, szRemark, sizeof(m_szRemark));
		}
		else {
			m_szRemark[0] = '\0';
		}
	}

	char           m_szTagId[20];
	time_t         m_tTime;
	char           m_szRemark[MAX_REMARK_LENGTH];
};

extern CGlobalData  g_data;
extern std::vector<TArea *>  g_vArea;
extern ARGB g_default_argb[MAX_READERS_PER_GRID];
// extern LmnToolkits::Thread *  g_thrd_excel;
extern LmnToolkits::Thread *  g_thrd_work;
extern LmnToolkits::Thread *  g_thrd_launch;
extern LmnToolkits::Thread *  g_thrd_db;
extern LmnToolkits::Thread *  g_thrd_sqlite;
extern DuiLib::CEditUI * g_edRemark;
extern DWORD g_dwPrintExcelMaxPointsCnt;
extern std::vector<BodyPart *>    g_vBodyParts;

extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern DuiLib::CControlUI* CALLBACK MY_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData);
extern DuiLib::CControlUI* CALLBACK CS_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData);
extern DWORD   GetMinTemp(DWORD  dwIndex);
extern DWORD   GetMaxTemp(DWORD  dwIndex);
extern DWORD   GetCollectInterval(DWORD dwIndex);
extern time_t  DateTime2String(const char * szDatetime);
extern char *  GetDefaultAlarmFile(char * szDefaultFile, DWORD dwSize);
extern int  GetCh340Count(char * szComPort, DWORD dwComPortLen);
extern BOOL EnumPortsWdm(std::vector<std::string> & v);
extern DWORD  FindReaderIndexByBed(DWORD dwBedNo);
extern BOOL  CheckComPortExist(int nCheckComPort);
extern void  OnEdtRemarkKillFocus_g(CControlUI * pUiImage);
extern char * Date2String_1(char * szDest, DWORD dwDestSize, const time_t * t);
extern char * Date2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern char * Date2String_2(char * szDest, DWORD dwDestSize, const time_t * t);
extern char * GetElapsedTimeDesc(char * buf, DWORD dwBufSize, time_t  tTimeDiff);
extern char * MyEncrypt(const void * pSrc, DWORD dwSrcSize, char * dest, DWORD dwDestSize);
extern int MyDecrypt(const char * szSrc, void * pDest, DWORD & dwDestSize);
extern char *  GetTagId(char * szTagId, DWORD dwTagIdLen, BYTE * pData, DWORD dwDataLen);
extern char *  GetReaderId(char * szReaderId, DWORD dwReaderIdLen, BYTE * pData, DWORD dwDataLen);
extern char *  GetReaderId_1(char * szReaderId, DWORD dwReaderIdLen, BYTE * pData);

#if DBG_FLAG
extern LONG WINAPI pfnUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);
#endif
extern time_t  GetTodayZeroTime();
extern int  GetWeekDay(time_t t);
extern const char * GetWeekDayName(int nWeekIndex);


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