#pragma once

#include <vector>
#include <algorithm>

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


#define   COM_STRING               "Silicon Labs CP210x USB to UART Bridge"

#define   MSG_CHECK_DEVICES        100

#define   UM_COM_STATUS            (WM_USER + 1)


class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	LmnToolkits::Thread *     m_thrd_com;
	HWND                      m_hWnd;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
		m_thrd_com = 0;
		m_hWnd = 0;
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