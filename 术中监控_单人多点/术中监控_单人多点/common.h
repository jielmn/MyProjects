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

#define   LOG_FILE_NAME           "single_patient_multi_points.log"
#define   CONFIG_FILE_NAME        "single_patient_multi_points.cfg"
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               "main.xml"
#define   SKIN_FOLDER             "res\\single_patient_multi_points_res"

#define   MAX_READERS_COUNT       9
#define   READER_FILE_NAME        "reader.xml"

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

#define   CFG_PATIENT_NAME         "patient name"
#define   CFG_PATIENT_SEX          "patient sex"
#define   CFG_PATIENT_AGE          "patient age"
#define   CFG_READER_NAME          "reader name"
#define   CFG_SHOWING_LOWEST_TEMP  "showing lowest temp"
#define   CFG_MYIMAGE_LEFT_BLANK   "myimage left blank"
#define   CFG_COLLECT_INTERVAL     "collect interval"
#define   CFG_ONCE_COLLECT_WIDTH   "once collect width"

#define   DEFAULT_LOWEST_TEMP            28
#define   DEFAULT_MYIMAGE_LEFT_BLANK     100
#define   DEFAULT_COLLECT_INTERVAL       10
#define   DEFAULT_COLLECT_INTERVAL_WIDTH 100

#define   MAX_TEMPERATURE                42
#define   MIN_MYIMAGE_VMARGIN            20              // 图像的上、下至少留出的空白
#define   MIN_TEMP_V_INTERVAL            30

#define   MAX_REMARK_LENGTH              28

typedef struct tagTempData {
	DWORD    dwIndex;
	DWORD    dwTemperature;
	time_t   tTime;
	char     szRemark[MAX_REMARK_LENGTH];
}TempData;

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


class  CGlobalData {
public:
	char           m_szPatietName[32];
	char           m_szPatietSex[32];
	char           m_szPatietAge[32];

	char           m_szReaderName[MAX_READERS_COUNT][32];
	DWORD          m_dwMyImageMinTemp;
	DWORD          m_dwMyImageLeftBlank;
	DWORD          m_dwCollectInterval;
	DWORD          m_dwCollectIntervalWidth;
};

extern ILog    * g_log;
extern IConfig * g_cfg;
extern LmnToolkits::Thread *  g_thrd_db;
extern HWND    g_hWnd;
extern CGlobalData  g_data;

//extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern DuiLib::CControlUI* CALLBACK MY_FINDCONTROLPROC(DuiLib::CControlUI* pSubControl, LPVOID lpData);

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