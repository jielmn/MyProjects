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
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame_STATION"
#define   SKIN_FILE               "mainframe_luhe.xml"
#define   SKIN_FOLDER             "proj_luhe_res"

#define   UM_TRAY                 (WM_USER+1)
#define   UM_ONCLOSE              (WM_USER+2)
#define   UM_OTHER                (WM_USER+3)

#define   MSG_RECONNECT_LAUNCH    1
#define   MSG_PRINT_STATUS        2
#define   MSG_GET_STATION_DATA    3

#define   RECONNECT_LAUNCH_TIME_INTERVAL       10000


class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_com;                // 读串口的线程

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_com = 0;
	}
};

extern CGlobalData  g_data;

extern int  GetCh340Count(char * szComPort, DWORD dwComPortLen);


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