#pragma once

#include <vector>
#include <algorithm>
#include <assert.h>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#define   PROJ_NAME               "bluecheck"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   TEST_FLAG                1


#define   COM_STRING               "Silicon Labs CP210x USB to UART Bridge"

#define   MSG_CHECK_DEVICES        100
#define   MSG_AUTO_START_TEST      101
#define   MSG_DISCONNECT_BLE       102
#define   MSG_READ_COM_DATA        103

#define   UM_COM_STATUS            (WM_USER + 1)
#define   UM_INFO_MSG              (WM_USER + 2)
#define   UM_BLUETOOTH_CNN_RET     (WM_USER + 3)
#define   UM_STOP_AUTO_TEST_RET    (WM_USER + 4)
#define   UM_AUTO_TEST_FIN         (WM_USER + 5)
#define   UM_BLE_DISCONNECTED      (WM_USER + 6)

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


class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	LmnToolkits::Thread *     m_thrd_com;
	HWND                      m_hWnd;

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