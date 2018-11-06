#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#define   LOG_FILE_NAME           "setting.log"
#define   CONFIG_FILE_NAME        "setting.cfg"
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame_Setting"
#define   SKIN_FILE               "mainframe_setting.xml"
#define   SKIN_FOLDER             "proj_setting_res"
#define   MAX_AREA_COUNT          20

typedef struct tagArea {
	char   szAreaName[64];
	DWORD  dwAreaNo;
}TArea;

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
extern std::vector<TArea *>  g_vArea;
extern IConfig * g_cfg_area;

extern BOOL EnumPortsWdm(std::vector<std::string> & v);
extern int  GetCh340Count(char * szComPort, DWORD dwComPortLen);
extern DWORD  FindNewAreaId(const std::vector<TArea *> & v);
extern void  SaveAreas();

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