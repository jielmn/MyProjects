#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#define   GLOBAL_LOCK_NAME        "UPDATE_LUHE"
#define   LOG_FILE_NAME           "update.log"
#define   CONFIG_FILE_NAME        "update.cfg"
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               "mainframe_update.xml"
#define   SKIN_FOLDER             "proj_update_res"

#define   UM_TRAY                 (WM_USER+1)
#define   UM_ONCLOSE              (WM_USER+2)
#define   UM_OTHER                (WM_USER+3)
#define   UM_UPDATE_RET           (WM_USER+4)
#define   UM_VERSION_RET          (WM_USER+5)

#define  TIMER_UPDATE_CHECK_ID        100
#define  TIMER_UPDATE_CHECK_INTERVAL  5000

#define  FETCH_STATION               1
#define  FETCH_VERSION               2

#define  STATE_OK                    0
#define  STATE_FETCH                 1
#define  STATE_GET_VERSION           2

#define  STATION_EXE_NAME           "TelemedStation.exe"
#define  STATION_CLASS_WINDOW_NAME  "DUIMainFrame_STATION"

class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	HWND                      m_hWnd;

	char                      m_szServerAddr[64];

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
	}
};

extern CGlobalData  g_data;


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