#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "LmnContainer.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#define  RECONNECT_READER_DELAY          10000

#define  MSG_RECONNECT_READER             1
#define  MSG_GET_TAG_TEMP                 2
#define  MSG_ALARM                        3

//境噪音，读取完成，场内无标签，读取失败
#define  STATUS_NOISE                     0x58
#define  STATUS_COMPLETE                  0x59
#define  STATUS_NO_TAG                    0x94
#define  STATUS_FAIL                      0x95

#define UM_SHOW_READER_STATUS            (WM_USER+1)
#define UM_SHOW_READ_TAG_TEMP_RET        (WM_USER+2)

#define  MAX_READER_COMMAND_LENGTH              256
// Reader通信协议命令
typedef struct tagReaderCmd {
	BYTE      abyCommand[MAX_READER_COMMAND_LENGTH];
	DWORD     dwCommandLength;
}ReaderCmd;

#define  MAX_TOKEN_LENGTH              64  
typedef struct tagToken {
	BYTE   byChannel;
	BYTE   byOperation;	
	BYTE   byDataLen;
	BYTE   reserved[1];
	BYTE   abyData[MAX_TOKEN_LENGTH];
}Token;

#define   MAX_TAG_ID_LENGTH           16
typedef  struct  tagTagItem {
	BYTE     abyUid[MAX_TAG_ID_LENGTH];
	BYTE     byUidLen;
	BYTE     reserved[3];
}TagItem;

typedef  struct tagTagBlock {
	BYTE     abyData[4];
}TagBlock;

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

#define    EXH_ERR_READER_CLOSE                          20001
#define    EXH_ERR_READER_FAILED_TO_WRITE                20002
#define    EXH_ERR_READER_TIMEOUT_OR_WRONG_FORMAT        20003
#define    EXH_ERR_NOT_FOUND_TAG                         20004

#define   READER_TAIL               "\x0d\x0a"

extern ILog    * g_log;
extern IConfig * g_cfg;
extern HWND    g_hWnd;
extern DWORD SERIAL_PORT_SLEEP_TIME;

extern LmnToolkits::Thread *  g_thrd_reader;
extern LmnToolkits::Thread *  g_thrd_timer;
extern LmnToolkits::Thread *  g_thrd_background;

extern  DWORD   g_dwCollectInterval;
extern  DWORD   g_dwLowTempAlarm;
extern  DWORD   g_dwHighTempAlarm;
#define MAX_ALARM_FILE_PATH                256
extern  char    g_szAlarmFilePath[MAX_ALARM_FILE_PATH];

#define  DEFAULT_ALARM_FILE_PATH                     "\\res\\1.wav"

extern ReaderCmd  PREPARE_COMMAND;
extern ReaderCmd  READ_TAG_DATA_COMMAND;

extern BOOL GetAllSerialPortName(std::vector<std::string> & vCom);
extern int TransferReaderCmd(ReaderCmd & cmd, const char * szCmd);
extern int AdTemperature(int AD_Value);


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