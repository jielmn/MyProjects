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

#define   PROJ_NAME               "wearable"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   MAX_GRID_COUNT 14

#define   MSG_OPEN_COM             1001
#define   MSG_READ_COM             1002

#define   UM_COM_STATUS           (WM_USER + 1)
#define   UM_COM_DATA             (WM_USER + 2)

#define   ABNORMAL_COLOR          0xFFFBE044
#define   NORMAL_COLOR            0xFF4AB20A

#define   TEST_FLAG                0

class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	HWND                      m_hWnd;
	int                       m_nComPort;
	BYTE                      m_byMinBeat;
	BYTE                      m_byMaxBeat;
	BYTE                      m_byMinOxy;
	int                       m_nMinTemp;
	int                       m_nMaxTemp;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
		m_hWnd = 0;
		m_nComPort = 0;

		m_byMinBeat = 0;
		m_byMaxBeat = 0;
		m_byMinOxy = 0;
		m_nMinTemp = 0;
		m_nMaxTemp = 0;
	}
};

extern CGlobalData  g_data;
extern BOOL  IsAbnormal(BYTE beat, BYTE beatV, BYTE byOxy, BYTE byOxyV, int nTemp);
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