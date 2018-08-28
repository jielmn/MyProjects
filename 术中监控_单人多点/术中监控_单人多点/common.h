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

#ifdef _DEBUG
#define TEST_FLAG                1
#endif

#if TEST_FLAG
#define  TIMER_TEST_ID_1            1
#define  TIMER_TEST_ID_2            2
#define  TIMER_TEST_ID_3            3
#define  TIMER_TEST_INTERVAL_1      3000
#define  TIMER_TEST_INTERVAL_2      4000
#define  TIMER_TEST_INTERVAL_3      5000
#endif

#define   LOG_FILE_NAME           "single_patient_multi_points.log"
#define   CONFIG_FILE_NAME        "single_patient_multi_points.cfg"
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               "main.xml"
#define   SKIN_FOLDER             "res\\single_patient_multi_points_res"
#define   SETTING_FRAME_NAME      "DUISettingFrame"
#define   SETTING_FILE            "Setting.xml"
#define   AREA_CFG_FILE_NAME      "area.cfg"

#define   MAX_READERS_COUNT       9
#define   READER_FILE_NAME        "reader.xml"
#define   MAX_GRID_COUNT          MAX_READERS_COUNT

#define   LAYOUT_READERS          "layReaders"
#define   ALARM_IMAGE_CLASS_NAME   "AlarmImage"
#define   MYIMAGE_CLASS_NAME       "MyImage"
#define   ALARM_IMAGE_NAME         "alarm_image"
#define   READER_SWITCH_NAME       "opn_reader_switch"
#define   READER_TEMP_NAME         "lblReaderTemp"
#define   BTN_READER_NAME          "btnReaderName"
#define   EDT_READER_NAME          "edtReaderName"
#define   LAY_READER_NAME          "layReader"
#define   BTN_PATIENT_NAME         "btnName"
#define   EDT_PATIENT_NAME         "edtName"
#define   BTN_PATIENT_SEX          "btnSex"
#define   EDT_PATIENT_SEX          "edtSex"
#define   BTN_PATIENT_AGE          "btnAge"
#define   EDT_PATIENT_AGE          "edtAge"
#define   MYIMAGE_NAME             "image0"
#define   CTL_INDICATOR_NAME       "indicator"
#define   BUTTON_OK_NAME           "btnOK"
#define   MYTREE_CONFIG_NAME       "CfgTree"
#define   MYTREE_CLASS_NAME        "MyTree"
#define   BTN_MENU_NAME            "menubtn"
#define   EDIT_REMARK_NAME         "edRemark"
#define   LBL_LAUNCH_STATUS        "lblReaderStatus"

#define   CFG_PATIENT_NAME         "patient name"
#define   CFG_PATIENT_SEX          "patient sex"
#define   CFG_PATIENT_AGE          "patient age"
#define   CFG_READER_NAME          "reader name"
#define   CFG_SHOWING_LOWEST_TEMP  "showing lowest temp"
#define   CFG_MYIMAGE_LEFT_BLANK   "myimage left blank"
#define   CFG_MYIMAGE_RIGHT_BLANK  "myimage right blank"
#define   CFG_COLLECT_INTERVAL     "collect interval"
#define   CFG_ONCE_COLLECT_WIDTH   "once collect width"
#define   CFG_LOW_TEMP_ALARM       "low temp alarm"
#define   CFG_HIGH_TEMP_ALARM      "high temp alarm"
#define   CFG_ARGB                 "argb"
#define   CFG_AREA_NAME            "area name"
#define   CFG_AREA_NO              "area no"
#define   CFG_BED_NO               "bed no"
#define   CFG_AREA_ID_NAME         "area id"
#define   CFG_ALARM_VOICE_SWITCH   "alarm voice switch"
#define   CFG_READER_SWITCH        "reader switch"
#define   CFG_ALARM_FILE           "alarm file"
#define   CFG_TELNET_PORT          "telnet port"

#define   DEFAULT_LOWEST_TEMP            28
#define   DEFAULT_MYIMAGE_LEFT_BLANK     50
#define   DEFAULT_MYIMAGE_RIGHT_BLANK    100
#define   DEFAULT_COLLECT_INTERVAL       60
#define   DEFAULT_COLLECT_INTERVAL_WIDTH 100
#define   DEFAULT_LOW_TEMP_ALARM         3500
#define   DEFAULT_HIGH_TEMP_ALARM        4000
#define   DEFAULT_ALARM_VOICE_SWITCH     FALSE
#define   DEFAULT_ALARM_FILE_PATH        "\\res\\single_patient_multi_points_res\\1.wav"

#define   MAX_TEMPERATURE                42
#define   MIN_MYIMAGE_VMARGIN            30              // 图像的上、下至少留出的空白
#define   MIN_TEMP_V_INTERVAL            30
#define   DEFAULT_POINT_RADIUS           6

#define   EDT_REMARK_WIDTH               200
#define   EDT_REMARK_HEIGHT              30
#define   EDT_REMARK_Y_OFFSET            -50

#define   MAX_REMARK_LENGTH              28
#define   MAX_AREA_NAME_LENGTH           64

#define   COMMON_SETTING_TEXT            "通用设置"
#define   AREA_NO_TEXT                   "病区号"
#define   SWITCH_ON_TEXT                 "开"
#define   SWITCH_OFF_TEXT                "关"
#define   ALARM_VOICE_SWITCH_TEXT        "报警声音开关"
#define   LOW_ALARM_TEXT                 "低温报警"
#define   HIGH_ALARM_TEXT                "高温报警"
#define   READER_ID_TEXT                 "Reader相关床号"
#define   MAX_AREA_ID                    100
#define   MAX_BED_ID                     200
#define   MAX_AREA_COUNT                 99
#define   MAX_ALARM_PATH_LENGTH          256
#define   MAX_COM_PORT_LENGTH            8
#define  RECONNECT_LAUNCH_TIME_INTERVAL  10000
#define  READER_STATUS_CLOSE             0
#define  READER_STATUS_OPEN              1

#define MSG_UPDATE_SCROLL                 1001
#define MSG_ALARM                         1002
#define MSG_RECONNECT_LAUNCH              1003
#define MSG_GET_TEMPERATURE               2000
#define MSG_READER_HEART_BEAT             3000

#define UM_UPDATE_SCROLL                 (WM_USER+1)
#define UM_LAUNCH_STATUS                 (WM_USER+2)
#define UM_GRID_READER_STATUS            (WM_USER+3)

typedef struct tagTempData {
	DWORD    dwIndex;
	DWORD    dwTemperature;
	time_t   tTime;
	char     szRemark[MAX_REMARK_LENGTH];
}TempData;

typedef struct tagArea {
	char   szAreaName[MAX_AREA_NAME_LENGTH];
	DWORD  dwAreaNo;
}TArea;

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CDialogBuilderCallbackEx(DuiLib::CPaintManagerUI *pManager ) {
		m_pManager = pManager;
	}
	CControlUI* CreateControl(LPCTSTR pstrClass);

private:
	DuiLib::CPaintManagerUI *  m_pManager;
};

class CGraphicsRoundRectPath : public GraphicsPath
{

public:
	CGraphicsRoundRectPath();

public:
	void AddRoundRect(INT x, INT y, INT width, INT height, INT cornerX, INT cornerY);
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

class CAlarmParam : public LmnToolkits::MessageData {
public:
	CAlarmParam(const char * szAlarmFile) {
		strncpy_s(m_szAlarmFile, szAlarmFile, sizeof(m_szAlarmFile));
	}
	~CAlarmParam() {}
	char    m_szAlarmFile[MAX_ALARM_PATH_LENGTH];
};

class CReaderHeartBeatParam : public LmnToolkits::MessageData {
public:
	CReaderHeartBeatParam(DWORD dwIndex) : m_dwGridIndex(dwIndex) { }
	DWORD     m_dwGridIndex;
};

class CGetTemperatureParam : public LmnToolkits::MessageData {
public:
	CGetTemperatureParam(DWORD dwIndex) : m_dwGridIndex(dwIndex) { }
	DWORD     m_dwGridIndex;
};


class  CGlobalData {
public:
	char           m_szPatietName[32];
	char           m_szPatietSex[32];
	char           m_szPatietAge[32];
	char           m_szAlarmFilePath[MAX_ALARM_PATH_LENGTH];

	char           m_szReaderName[MAX_READERS_COUNT][32];
	DWORD          m_dwMyImageMinTemp;
	DWORD          m_dwMyImageLeftBlank;
	DWORD          m_dwMyImageRightBlank;
	DWORD          m_dwCollectInterval;
	DWORD          m_dwCollectIntervalWidth;
	DWORD          m_dwLowTempAlarm[MAX_READERS_COUNT];
	DWORD          m_dwHighTempAlarm[MAX_READERS_COUNT];
	ARGB           m_argb[MAX_READERS_COUNT];
	DWORD          m_dwAreaNo;
	DWORD          m_bAlarmVoiceOff;
	BOOL           m_bReaderSwitch[MAX_READERS_COUNT];
	DWORD          m_dwBedNo[MAX_READERS_COUNT];

	BOOL           m_bAutoScroll;                // 自动更新滑动条
	char           m_szLaunchComPort[MAX_COM_PORT_LENGTH];
};

extern ILog    * g_log;
extern IConfig * g_cfg;
extern IConfig * g_cfg_area;
//extern LmnToolkits::Thread *  g_thrd_db;
extern LmnToolkits::Thread *  g_thrd_work;
extern LmnToolkits::Thread *  g_thrd_launch;
extern HWND    g_hWnd;
extern CGlobalData  g_data;
extern ARGB g_default_argb[MAX_READERS_COUNT];
extern std::vector<TArea *>  g_vArea;
extern DuiLib::CEditUI * g_edRemark;

//extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern DuiLib::CControlUI* CALLBACK MY_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData);
extern time_t  DateTime2String(const char * szDatetime);
extern char * GetDefaultAlarmFile(char * szDefaultFile, DWORD dwSize);
extern void  OnEdtRemarkKillFocus(CControlUI * pUiImage);
extern char * Date2String_1(char * szDest, DWORD dwDestSize, const time_t * t);
extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern BOOL EnumPortsWdm(std::vector<std::string> & v);
extern int  GetCh340Count(char * szComPort, DWORD dwComPortLen);
extern BOOL  CheckComPortExist(int nCheckComPort);
extern DWORD  FindGridIndexByBed(DWORD dwBedNo);

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