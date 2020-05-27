#pragma once

#include <vector>
#include <algorithm>
#include <iterator>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"
#include "LmnSerialPort.h"

#include "UIlib.h"
using namespace DuiLib;

#define   PROJ_NAME               "building_temp_monitor"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")


#define   MSG_ADD_FLOOR                   100
#define   MSG_GET_ALL_CLASSES             101
#define   MSG_GET_DEVICES_BY_FLOOR        102
#define   MSG_ADD_DEVICE                  103
#define   MSG_DEL_DEVICE                  104
#define   MSG_DEL_FLOOR                   105
#define   MSG_CHECK_LAUNCH_STATUS         106
#define   MSG_READ_COM_PORTS              107
#define   MSG_TEMPERATURE_ITEM            108

#define   UM_ADD_FLOOR_RET                (WM_USER + 1)
#define   UM_GET_ALL_FLOORS_RET           (WM_USER + 2)
#define   UM_GET_DEVICES_BY_FLOOR_RET     (WM_USER + 3)
#define   UM_ADD_DEVICE_RET               (WM_USER + 4)
#define   UM_DEL_DEVICE_RET               (WM_USER + 5)
#define   UM_DEL_FLOOR_RET                (WM_USER + 6)
#define   UM_CHECK_COM_PORTS_RET          (WM_USER + 7)
#define   UM_TEMPERATURE                  (WM_USER + 8)

#define   CFG_LAUNCH_COM_PORT               "launch com ports"
#define   MAX_COM_PORTS_CNT            4

class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	HWND                      m_hWnd;
	LmnToolkits::Thread *     m_thrd_db;
	LmnToolkits::Thread *     m_thrd_launch;

	BOOL                      m_bMultipleComport;
	BOOL                      m_bSpecifiedComports;
	int                       m_nComports[MAX_COM_PORTS_CNT];     // 使用哪些指定串口
	int                       m_nComportsCnt;

	int                       m_nHighTemp;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
		m_thrd_launch = 0;
		m_hWnd = 0;

		m_bMultipleComport = FALSE;
		m_bSpecifiedComports = FALSE;
		m_nComportsCnt = 0;
		m_nHighTemp = 0;
	}
};

class CAddFloorParam : public LmnToolkits::MessageData {
public:
	CAddFloorParam(HWND hWnd, int nFloor) {
		m_hWnd   = hWnd;
		m_nFloor = nFloor;
	}

	HWND    m_hWnd;
	int     m_nFloor;
};

class CGetDevicesParam : public LmnToolkits::MessageData {
public:
	CGetDevicesParam(int nFloor) {
		m_nFloor = nFloor;
	}
	int     m_nFloor;
};

typedef  struct  tagDeviceItem {
	int     nFloor;
	int     nDeviceNo;
	char    szAddr[32];
	int     nTemp;
	time_t  time;
}DeviceItem;

class CAddDeviceParam : public LmnToolkits::MessageData {
public:
	CAddDeviceParam(HWND hWnd, int nFloor, int nDeviceId, const char * szDeviceAddr) {
		m_hWnd = hWnd;
		m_nFloor = nFloor;
		m_nDeviceId = nDeviceId;
		STRNCPY(m_szDeviceAddr, szDeviceAddr, sizeof(m_szDeviceAddr));
	}

	HWND    m_hWnd;
	int     m_nFloor;
	int     m_nDeviceId;
	char    m_szDeviceAddr[32];
};

class CDelDeviceParam : public LmnToolkits::MessageData {
public:
	CDelDeviceParam(int nDeviceId) {
		m_nDeviceId = nDeviceId;
	}
	int     m_nDeviceId;
};

class CDelFloorParam : public LmnToolkits::MessageData {
public:
	CDelFloorParam(int nFloor) {
		m_nFloor = nFloor;
	}
	int     m_nFloor;
};

// 创建duilib控件的回调
class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CDialogBuilderCallbackEx(DuiLib::CPaintManagerUI *pManager) {
		m_pManager = pManager;
	}
	CControlUI* CreateControl(LPCTSTR pstrClass);

public:
	DuiLib::CPaintManagerUI *  m_pManager;
};

// 温度数据
typedef struct  tagTempItem {
	DWORD   m_dwTemp;                                        // 温度
	time_t  m_time;                                          // 时间
	int     m_nDeviceId;
}TempItem;

class CTempParam : public LmnToolkits::MessageData {
public:
	CTempParam(TempItem * pItem) {
		memcpy(&m_item, pItem, sizeof(TempItem));
	}
	TempItem   m_item;
};


extern CGlobalData  g_data;

extern bool sortInt(int d1, int d2);
extern CControlUI* CALLBACK FindControlByName(CControlUI* pSubControl, LPVOID pParam);
extern DWORD GetIntFromDb(const char * szValue, int nDefault = 0);
extern char * GetStrFromdDb(char * buf, DWORD dwSize, const char * szValue);
extern int  GetCh340Count(char * szComPort, DWORD dwComPortLen);
extern void InitComPort(CLmnSerialPort * pPort);

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