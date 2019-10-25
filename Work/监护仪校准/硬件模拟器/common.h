#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#define  NEW_VERSION_FLAG         1

#define   PROJ_NAME               "sim"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   MSG_START               1001
#define   MSG_STOP                1002
#define   MSG_READ                1003

#define   UM_START_RET            (WM_USER + 1)
#define   UM_STOP_RET             (WM_USER + 2)

class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	HWND                      m_hWnd;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
		m_hWnd = 0;
	}
};

class CStartParam : public LmnToolkits::MessageData {
public:
	CStartParam(int nComPort) {
		m_nComPort = nComPort;
	}

	int m_nComPort;
};

extern CGlobalData  g_data;

BOOL EnumPortsWdm(std::vector<std::string> & v);

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