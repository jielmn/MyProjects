#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#include "UIlib.h"
using namespace DuiLib;

// 是否多人多点的Reader配置
#define   MULTI_FLAG              1

#define   LOG_FILE_NAME           "surgery_setting.log"
#define   CONFIG_FILE_NAME        "surgery_setting.cfg"
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               "mainframe.xml"
#define   SKIN_FOLDER             "res"

#define   MSG_SETTING_READER       1001
#define   MSG_SETTING_GW           1002
#define   MSG_QUERY_GW             1003

#define   UM_SETTTING_READER_RET   (WM_USER + 1)
#define   UM_SETTTING_GW_RET       (WM_USER + 2)
#define   UM_QUERY_GW_RET          (WM_USER + 3)

#define   MAX_AREA_COUNT            20

class CSettingReaderParam : public LmnToolkits::MessageData {
public:
	CSettingReaderParam(int nAreaNo, int nBedNo, int nComPort) {
		m_nAreaNo = nAreaNo;
		m_nBedNo = nBedNo;
		m_nComPort = nComPort;
	}
	
	int   m_nAreaNo;
	int   m_nBedNo;
	int   m_nComPort;
};

class CSettingGwParam : public LmnToolkits::MessageData {
public:
	CSettingGwParam(int nAreaNo, int nComPort) {
		m_nAreaNo = nAreaNo;
		m_nComPort = nComPort;
	}

	int   m_nAreaNo;
	int   m_nComPort;
};

class CQueryGwParam : public LmnToolkits::MessageData {
public:
	CQueryGwParam(int nComPort) {
		m_nComPort = nComPort;
	}

	int   m_nComPort;
};

typedef struct tagArea {
	char   szAreaName[64];
	DWORD  dwAreaNo;
}TArea;

extern ILog    * g_log;
extern IConfig * g_cfg;
extern IConfig * g_cfg_area;
extern LmnToolkits::Thread *  g_thrd_worker;
extern HWND    g_hWnd;
extern std::vector<TArea *>  g_vArea;


//extern char * Time2String(char * szDest, DWORD dwDestSize, const time_t * t);
extern BOOL EnumPortsWdm(std::vector<std::string> & v);
extern void  SaveAreas();
extern DWORD  FindNewAreaId(const std::vector<TArea *> & v);

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