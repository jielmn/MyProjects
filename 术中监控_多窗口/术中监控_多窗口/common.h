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

#define  DEFAULT_ALARM_FILE_PATH                     "\\res\\1.wav"

#define  COLOR_THREAD_1                 "0x666666"
#define  COLOR_THREAD_2                 "0x9EF8D1"
#define  COLOR_TEXT_1                   "0xFFFFFF"

#define  COLOR_LOW_TEMPERATURE          "0x02A5F1"
#define  COLOR_NORMAL_TEMPERATURE       "0xFFFFFF"
#define  COLOR_HIGH_TEMPERATURE         "0xFC235C"

#define  RGB_REVERSE(a)                 ( ( (a & 0xFF) << 16 ) | ( a & 0xFF00 ) | ( (a & 0xFF0000) >> 16  ) )


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

	char    m_szAlarmFile[256];
};




extern ILog    * g_log;
extern IConfig * g_cfg;
extern LmnToolkits::Thread *  g_thrd_db;

extern  DWORD   g_dwCollectInterval[MYCHART_COUNT];
extern  DWORD   g_dwLowTempAlarm[MYCHART_COUNT];
extern  DWORD   g_dwHighTempAlarm[MYCHART_COUNT];
extern  DWORD   g_dwMinTemp[MYCHART_COUNT];
extern  char    g_szComPort[MYCHART_COUNT][32];
#define MAX_ALARM_FILE_PATH                256
extern  char    g_szAlarmFilePath[MAX_ALARM_FILE_PATH];


extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern BOOL GetAllSerialPortName(std::vector<std::string> & vCom);
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