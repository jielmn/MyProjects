#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#define   PROJ_NAME               "simulation"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   MSG_CONNECT_COM          1
#define   MSG_READ_DATA            2

#define   UM_STATUS                (WM_USER+1)
#define   UM_READ_DATA             (WM_USER+2)
#define   UM_SEND_DATA             (WM_USER+3)

#define   MAX_READERS_PER_GRID      6
#define   MAX_GRID                  64

typedef  struct  tagMyListItem {
	BYTE        m_data[128];
	DWORD       m_dwDataLen;
	time_t      m_time;
}MyListItem;


class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_launch;
	int                       m_nComPort;
	BOOL                      m_bConnected;
	HWND                      m_hWnd;
	BOOL                      m_bSurgery;               // 术中监控开关
	BOOL                      m_bHand;                  // 手持开关

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_launch = 0;
		m_nComPort = 0;
		m_bConnected = FALSE;
		m_hWnd = 0;
		m_bSurgery = FALSE;
		m_bHand = FALSE;
	}
};

extern CGlobalData  g_data;

extern char *  Data2String(char * buf, DWORD dwBufSize, const BYTE * pData, DWORD dwDataLen);
extern char * DateTime2String(char * szDest, DWORD dwDestSize, const time_t * t);

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