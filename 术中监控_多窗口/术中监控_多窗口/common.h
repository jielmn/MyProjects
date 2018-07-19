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

//#define  MAIN_FRAME_TYPE   0       // 3 * 2格式
#define  MAIN_FRAME_TYPE   1       // 2 * 2格式

#if MAIN_FRAME_TYPE == 0
#define   LAY_LAYER_COUNT        2
#define   MYCHART_PER_LAYER      3
#else
#define   LAY_LAYER_COUNT        2
#define   MYCHART_PER_LAYER      2
#endif



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

#define  MSG_RECONNECT_READER             1
#define  MSG_GET_TAG_TEMP                 2
#define  MSG_ALARM                        3

#define UM_SHOW_READER_STATUS            (WM_USER+1)
#define UM_SHOW_READ_TAG_TEMP_RET        (WM_USER+2)
#define UM_UPDATE_SCROLL                 (WM_USER+3)

#define  RECONNECT_READER_DELAY          5000

#define    EXH_ERR_READER_CLOSE                          20001
#define    EXH_ERR_READER_FAILED_TO_WRITE                20002
#define    EXH_ERR_READER_TIMEOUT_OR_WRONG_FORMAT        20003
#define    EXH_ERR_NOT_FOUND_TAG                         20004

#define  COLOR_LOW_TEMPERATURE          "0x02A5F1"
#define  COLOR_NORMAL_TEMPERATURE       "0xFFFFFF"
#define  COLOR_HIGH_TEMPERATURE         "0xFC235C"

// 张维国的新的读卡器
#define TELEMED_READER_TYPE_1  

#define TIME_LEFT_TIMER_ID           2
#define TIME_LEFT_TIMER_INTEVAL      1000


#define  MAX_READER_COMMAND_LENGTH              256
// Reader通信协议命令
typedef struct tagReaderCmd {
	BYTE      abyCommand[MAX_READER_COMMAND_LENGTH];
	DWORD     dwCommandLength;
}ReaderCmd;

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

class CReconnectReaderParam : public LmnToolkits::MessageData {
public:
	CReconnectReaderParam(int nIndex) {
		m_nIndex = nIndex;
	}
	~CReconnectReaderParam() {}

	int     m_nIndex;
};

class CReadTempParam : public LmnToolkits::MessageData {
public:
	CReadTempParam(int nIndex) {
		m_nIndex = nIndex;
	}
	~CReadTempParam() {}

	int     m_nIndex;
};

extern ILog    * g_log;
extern IConfig * g_cfg;
extern HWND    g_hWnd;
//extern LmnToolkits::Thread *  g_thrd_db;
extern LmnToolkits::Thread *  g_thrd_reader[MYCHART_COUNT];
extern LmnToolkits::Thread *  g_thrd_timer;
extern LmnToolkits::Thread *  g_thrd_background;

extern ReaderCmd  PREPARE_COMMAND;
extern ReaderCmd  READ_TAG_DATA_COMMAND;

extern  DWORD   g_dwCollectInterval[MYCHART_COUNT];
extern  DWORD   g_dwLowTempAlarm[MYCHART_COUNT];
extern  DWORD   g_dwHighTempAlarm[MYCHART_COUNT];
extern  DWORD   g_dwMinTemp[MYCHART_COUNT];
extern  char    g_szComPort[MYCHART_COUNT][32];
#define MAX_ALARM_FILE_PATH                256
extern  char    g_szAlarmFilePath[MAX_ALARM_FILE_PATH];

extern DWORD SERIAL_PORT_SLEEP_TIME;


extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern BOOL GetAllSerialPortName(std::vector<std::string> & vCom);
extern char * GetDefaultAlarmFile(char * szDefaultFile, DWORD dwSize);
extern int TransferReaderCmd(ReaderCmd & cmd, const char * szCmd);

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