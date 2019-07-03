#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"
#include "lua.hpp"
   
#define   PROJ_NAME               "lua"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")


class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
	}
};

extern CGlobalData  g_data;

extern lua_State* init_lua();


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