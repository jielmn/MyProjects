#pragma once

#include <vector>
#include <algorithm>
#include <assert.h>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#include "UIlib.h"
using namespace DuiLib;

#define   PROJ_NAME               "bluecheck"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#ifdef _DEBUG
#define   TEST_FLAG                1
#endif



#define   COM_STRING               "Silicon Labs CP210x USB to UART Bridge"

#define   MSG_CHECK_DEVICES        100
#define   MSG_AUTO_START_TEST      101
#define   MSG_DISCONNECT_BLE       102
#define   MSG_READ_COM_DATA        103
#define   MSG_STOP_AUTO_TEST       104
#define   MSG_MEASURE_TEMP         105
#define   MSG_SAVE_RESULT          106
#define   MSG_QUERY_DATA           107

#define   UM_COM_STATUS            (WM_USER + 1)
#define   UM_INFO_MSG              (WM_USER + 2)
#define   UM_BLUETOOTH_CNN_RET     (WM_USER + 3)
// #define   UM_STOP_AUTO_TEST_RET    (WM_USER + 4)
#define   UM_AUTO_TEST_FIN         (WM_USER + 5)
#define   UM_BLE_DISCONNECTED      (WM_USER + 6)
#define   UM_STOP_AUTO_TEST_RET    (WM_USER + 7)
#define   UM_MEASURE_TEMP_FIN      (WM_USER + 8)
#define   UM_SAVE_RESULT_FIN       (WM_USER + 9)
#define   UM_QUERY_DATA_RET        (WM_USER + 10)

enum InfoType {
	CONNECTING  = 0,
	CONNECTING_1,
	CNN_FAILED,
	CNN_OK,
	TEMPING,
	TEMP_RET,
	SETTING_NOTIFYID,
	SETTING_NOTIFYID_RET,
	ATING,
	ATING_RET,
};

enum AutoTestRet {
	RET_OK   = 0,
	RET_BLE_FAILED,
	RET_TEMP_FAILED,
	RET_AT_FAILED,
	RET_NOTIFYID_FAILED,
};


class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	LmnToolkits::Thread *     m_thrd_com;
	HWND                      m_hWnd;
	BOOL                      m_bQuiting;

	BYTE                      m_byMacType;
	char                      m_szWriteCharId[5];
	char                      m_szNotifyCharId[5];	
	BYTE                      m_adwTempCmd[2];
	char                      m_reserverd0[3];

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
		m_thrd_com = 0;
		m_hWnd = 0;
		m_bQuiting = FALSE;

		m_byMacType = 0;
		memset(m_szWriteCharId, 0, sizeof(m_szWriteCharId));
		memset(m_szNotifyCharId, 0, sizeof(m_szNotifyCharId));
		memset(m_adwTempCmd, 0, sizeof(m_adwTempCmd));
	}
};

extern CGlobalData  g_data;

class CStartAutoTestParam : public LmnToolkits::MessageData {
public:
	CStartAutoTestParam(const char * szMac) {
		STRNCPY(m_szMac, szMac, sizeof(m_szMac));
	}

	char   m_szMac[20];
};

typedef  struct  tagTempItem {
	DWORD      dwTemp;
	char       szTagId[20];
}TempItem;

class CSaveResultParam : public LmnToolkits::MessageData {
public:
	CSaveResultParam(const char * szReaderId, BOOL bPass, DWORD  dwFact) {
		STRNCPY(m_szMac, szReaderId, sizeof(m_szMac));
		m_bPass = bPass;
		m_dwFact = dwFact;
	}

	char   m_szMac[20];
	BOOL   m_bPass;
	DWORD  m_dwFact;
};

class CQueryDataParam : public LmnToolkits::MessageData {
public:
	CQueryDataParam(time_t t1, time_t t2, const char * szMac, HWND hWnd) {
		STRNCPY(m_szMac, szMac, sizeof(m_szMac));
		m_t1 = t1;
		m_t2 = t2;
		m_hWnd = hWnd;
	}

	char     m_szMac[20];
	time_t   m_t1;
	time_t   m_t2;
	HWND     m_hWnd;
};

typedef  struct tagReaderItem {
	char     m_szMac[20];
	BOOL     m_bPass;
	DWORD    m_dwFact;
	time_t   m_time;
}ReaderItem;

extern DWORD GetIntFromDb(const char * szValue, int nDefault = 0);
extern char * GetStrFromdDb(char * buf, DWORD dwSize, const char * szValue);


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