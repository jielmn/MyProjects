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

extern ILog    * g_log;
extern IConfig * g_cfg;
extern LmnToolkits::Thread *  g_thrd_db;


extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);

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