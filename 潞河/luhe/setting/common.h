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

#define   MSG_SETTING_GW          1001
#define   MSG_SETTING_READER      1002
#define   MSG_SETTING_SN          1003

#define   UM_SETTTING_GW_RET      (WM_USER + 1)
#define   UM_SETTTING_READER_RET  (WM_USER + 2)
#define   UM_SETTTING_SN_RET      (WM_USER + 3)

typedef struct tagArea {
	char   szAreaName[64];
	DWORD  dwAreaNo;
}TArea;

class CSettingGwParam : public LmnToolkits::MessageData {
public:
	CSettingGwParam(int nAreaNo, int nComPort) {
		m_nAreaNo = nAreaNo;
		m_nComPort = nComPort;
	}

	int   m_nAreaNo;
	int   m_nComPort;
};

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

class CSettingSnParam : public LmnToolkits::MessageData {
public:
	CSettingSnParam(int nSn, int nComPort) {
		m_nSn = nSn;
		m_nComPort = nComPort;
	}

	int   m_nSn;
	int   m_nComPort;
};

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
extern HWND    g_hWnd;

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