#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"
#include "LuaConfig.h"
#include "UIlib.h"
using namespace DuiLib;
#include <time.h>

#define   PROJ_NAME               "wearable_monitor"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   VERSION                     "1.0"
#define   COMPILE_TIME                "2019-12-18 16:00"

#define   MSG_RECONNECT               1
#define   MSG_READ_DATA               2
#define   MSG_GET_NAME                3
#define   MSG_SAVE_NAME               4
#define   MSG_SAVE_ITEM               5

#define   UM_RECONNECT_RET            (WM_USER+1)
#define   UM_DATA_ITEM                (WM_USER+2)
#define   UM_GET_NAME_RET             (WM_USER+3)

#define   ABNORMAL_COLOR          0xFFFBE044
#define   NORMAL_COLOR            0xFF4AB20A
#define   ABNORMAL_TEXT_COLOR     0xFFFF0000

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

#define  MAX_DEVICE_ID_LEN  20
#define  MAX_NAME_LEN       20

typedef  struct  tagDataItem {
	int     nData;
	time_t  tTime;
}DataItem;

class CWearItem {
public:
	char  m_szDeviceId[MAX_DEVICE_ID_LEN];
	char  m_szName[MAX_NAME_LEN];
	int   m_nHearbeat;    // 心率
	int   m_nOxy;         // 血氧
	int   m_nTemp;        // 体温
	int   m_nPose;	

public:
	CWearItem();
	~CWearItem();
	CWearItem & operator =( const CWearItem & item );
};

typedef struct tagSort {
	BOOL     bSorted;
	BOOL     bAscend;
}Sort;

class CGetNameParam : public LmnToolkits::MessageData {
public:
	CGetNameParam(const char * szDeviceId) { 
		STRNCPY( m_szDeviceId, szDeviceId, MAX_DEVICE_ID_LEN );
	}
	
	char m_szDeviceId[MAX_DEVICE_ID_LEN];
};

typedef  struct  tagQueryNameRet {
	char    szDeviceId[MAX_DEVICE_ID_LEN];
	char    szName[MAX_NAME_LEN];
}QueryNameRet;

class CSaveNameParam : public LmnToolkits::MessageData {
public:
	CSaveNameParam(const char * szDeviceId, const char * szName) {
		STRNCPY(m_szDeviceId, szDeviceId, MAX_DEVICE_ID_LEN);
		STRNCPY(m_szName, szName, MAX_NAME_LEN);
	}

	char m_szDeviceId[MAX_DEVICE_ID_LEN];
	char m_szName[MAX_NAME_LEN];
};

class CSaveItemParam : public LmnToolkits::MessageData {
public:
	CSaveItemParam(CWearItem * pItem) {
		m_item = *pItem;
		m_time = time(0);
	}

	CWearItem   m_item;
	time_t      m_time;
};

class  CGlobalData {
public:
	ILog    *                 m_log;
	CLuaCfg *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_com;
	LmnToolkits::Thread *     m_thrd_db;
	HWND                      m_hWnd;
	int                       m_nComPort;
	BOOL                      m_bShowHistory;
	BOOL                      m_bWarningPrepose;
	BOOL                      m_bRecycle;
	int                       m_nMaxHeartBeat;
	int                       m_nMinHeartBeat;
	int                       m_nMinOxy;
	int                       m_nMaxTemp;
	int                       m_nMinTemp;
	time_t                    m_tStartTime;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_com = 0;
		m_thrd_db = 0;
		m_hWnd = 0;
		m_nComPort = 0;
		m_bShowHistory = FALSE;
		m_bWarningPrepose = FALSE;
		m_bRecycle = FALSE;
		m_nMaxHeartBeat = 0;
		m_nMinHeartBeat = 0;
		m_nMinOxy = 0;
		m_nMaxTemp = 0;
		m_nMinTemp = 0;
		m_tStartTime = GetTdZeroTime();
	}
};

extern CGlobalData  g_data;

extern int     CharacterCompare(const char * s1, const char * s2);
extern BOOL    IsWarningItem(CWearItem * pItem);
extern time_t  GetTodayZeroTime();

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