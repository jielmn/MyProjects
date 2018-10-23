#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"
#include "skin.h"

#include "UIlib.h"
using namespace DuiLib;

#define   LOG_FILE_NAME           "surgery3.log"
#define   CONFIG_FILE_NAME        "surgery3.cfg"
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               "mainframe.xml"
#define   SKIN_FOLDER             "res\\surgery_res_3"
#define   MAX_GRID_COUNT          50
#define   MAX_READERS_PER_GRID    9
#define   READER_FILE_NAME        "reader.xml"
#define   SETTING_FRAME_NAME      "DUISettingFrame"
#define   SETTING_FILE            "Setting.xml"
#define   MAX_AREA_COUNT          20
#define   AREA_CFG_FILE_NAME      "area.cfg"
#define   MAX_AREA_ID             100
#define   MAX_BED_LENGTH          16
#define   MAX_NAME_LENGTH         16

// 控件相关
#define   LAYOUT_MAIN_NAME         "layMain"
#define   GRID_NAME                "MyGrid"
#define   MYCHART_XML_FILE         "MyChart.xml"
#define   LAYOUT_STATUS_NAME       "layStatus"
#define   MYIMAGE_CLASS_NAME       "MyImage"
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
#define   CFG_READER_SWITCH             "reader switch"
#define   CFG_LOW_TEMP_ALARM            "low temperature alarm"
#define   CFG_HIGH_TEMP_ALARM           "high temperature alarm"
#define   CFG_BED_NO                    "bed no"
#define   DEFAULT_MIN_TEMP_INDEX        2
#define   CFG_BED_NAME                  "bed name"
#define   CFG_PATIENT_NAME              "patient name"

// 其他

// 类
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
}ReaderCfg;

typedef struct tagGridCfg
{
	BOOL        m_bSwitch;
	DWORD       m_dwCollectInterval;
	DWORD       m_dwMinTemp;
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
	GridCfg   m_GridCfg[MAX_GRID_COUNT];
}CfgData;

class  CGlobalData {
public:
	HWND      m_hWnd;
	ILog    * m_log;
	IConfig * m_cfg;
	CMySkin   m_skin;

	CfgData   m_CfgData;
};

#define  MAX_AREA_NAME_LENGTH   64
typedef struct tagArea {
	char   szAreaName[MAX_AREA_NAME_LENGTH];
	DWORD  dwAreaNo;
}TArea;

extern CGlobalData  g_data;
extern std::vector<TArea *>  g_vArea;


extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern DuiLib::CControlUI* CALLBACK MY_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData);
extern DuiLib::CControlUI* CALLBACK CS_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData);

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