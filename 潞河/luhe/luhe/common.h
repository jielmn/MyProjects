#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#define   GLOBAL_LOCK_NAME        "LUHE"
#define   LOG_FILE_NAME           "station.log"
#define   CONFIG_FILE_NAME        "station.cfg"
#define   UPDATE_FILE_NAME        "update.cfg"
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame_STATION"
#define   SKIN_FILE               "mainframe_luhe.xml"
#define   SKIN_FOLDER             "proj_luhe_res"

#define   UM_TRAY                 (WM_USER+1)
#define   UM_ONCLOSE              (WM_USER+2)
#define   UM_OTHER                (WM_USER+3)
#define   UM_MODIFY_NAME          (WM_USER+4)

#define   MSG_RECONNECT_LAUNCH    1
#define   MSG_PRINT_STATUS        2
#define   MSG_GET_STATION_DATA    3

#define   RECONNECT_LAUNCH_TIME_INTERVAL       10000
#define   READER_ID_LENGTH                     8
#define   TAG_ID_LENGTH                        8
#define   NURSE_ID_LENGTH                      8

#define   TIMER_HEART_BEAT                     90
#define   TIMER_HEART_BEAT_INTERVAL            55000

#define   TYPE_UPLOAD_TEMP                     1
#define   TYPE_HEART_BEAT                      2

typedef struct tagTempItem {
	BYTE    pbReaderId[READER_ID_LENGTH];
	BYTE    pbTagId[TAG_ID_LENGTH];
	BYTE    pbNurseId[NURSE_ID_LENGTH];
	DWORD   dwTemp;
	time_t  tTime;
}TempItem;

class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	IConfig *                 m_update_cfg;
	LmnToolkits::Thread *     m_thrd_com;                // 读串口的线程


	char                      m_szServerAddr[64];
	BOOL                      m_bBindingReader;
	char                      m_szName[64];
	BOOL                      m_bUtf8;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_com = 0;
	}
};

extern CGlobalData  g_data;

extern int  GetCh340Count(char * szComPort, DWORD dwComPortLen);
extern std::string UrlEncode(const std::string& str);


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