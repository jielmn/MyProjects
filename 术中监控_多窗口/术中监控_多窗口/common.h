#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#define   LOG_FILE_NAME           "surgery_temp.log"
#define   CONFIG_FILE_NAME        "surgery_temp.cfg"

#define   LAY_LAYER_COUNT        2
#define   MYCHART_PER_LAYER      3
#define   MYCHART_COUNT          (LAY_LAYER_COUNT*MYCHART_PER_LAYER)

#define   CHART_STATE_NORMAL     0
#define   CHART_STATE_EXPAND     1

extern ILog    * g_log;
extern IConfig * g_cfg;
extern LmnToolkits::Thread *  g_thrd_db;

extern  DWORD   g_dwCollectInterval[MYCHART_COUNT];
extern  DWORD   g_dwLowTempAlarm[MYCHART_COUNT];
extern  DWORD   g_dwHighTempAlarm[MYCHART_COUNT];
extern  DWORD   g_dwMinTemp[MYCHART_COUNT];
extern  char    g_szComPort[MYCHART_COUNT][32];

extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
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