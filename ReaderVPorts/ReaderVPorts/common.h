#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#define   LOG_FILE_NAME           "blank.log"
#define   CONFIG_FILE_NAME        "blank.cfg"
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               "mainframe_blank.xml"
#define   SKIN_FOLDER             "res"

#define   MAX_VPORT_COUNT          4

#define   MSG_OPEN_VPORT           101
#define   MSG_HANDLE_VPORT         102

#define   UM_NOTIFY_OPEN_VPORT_RET            (WM_USER+1)

class COpenVPortParam : public LmnToolkits::MessageData {
public:
	COpenVPortParam(int nIndex) {
		m_nIndex = nIndex;
	}
	~COpenVPortParam() {}

	int     m_nIndex;
};

class CHandleVPortParam : public LmnToolkits::MessageData {
public:
	CHandleVPortParam(int nIndex) {
		m_nIndex = nIndex;
	}
	~CHandleVPortParam() {}

	int     m_nIndex;
};

extern HWND    g_hWnd;
extern ILog    * g_log;
extern IConfig * g_cfg;
extern LmnToolkits::Thread *  g_thrd_db;
extern LmnToolkits::Thread *  g_thrd_vport[MAX_VPORT_COUNT];
extern int     g_nInit[MAX_VPORT_COUNT];


//extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);

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