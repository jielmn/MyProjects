#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#define   PROJ_NAME               "zhexian"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   MSG_OPEN_COM_PORT        1001
#define   MSG_CLOSE_COM            1002
#define   MSG_READ_COM             1004

#define   UM_OPEN_COM_RET         (WM_USER +2)
#define   UM_CLOSE_COM_RET        (WM_USER +3)
#define   UM_READ_COM_RET         (WM_USER +5)

#define   COM_PORT_RATE           19200

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

class COpenComParam : public LmnToolkits::MessageData {
public:
	COpenComParam(int nCom) : m_nCom(nCom) { }

	int    m_nCom;
};

extern CGlobalData  g_data;

extern BOOL EnumPortsWdm(std::vector<std::string> & v);


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