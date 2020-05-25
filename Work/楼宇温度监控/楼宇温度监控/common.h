#pragma once

#include <vector>
#include <algorithm>
#include <iterator>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#include "UIlib.h"
using namespace DuiLib;

#define   PROJ_NAME               "building_temp_monitor"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")


#define   MSG_ADD_FLOOR                   100
#define   MSG_GET_ALL_CLASSES             101
#define   MSG_GET_DEVICES_BY_FLOOR        102

#define   UM_ADD_FLOOR_RET                (WM_USER + 1)
#define   UM_GET_ALL_FLOORS_RET           (WM_USER + 2)
#define   UM_GET_DEVICES_BY_FLOOR_RET     (WM_USER + 3)

class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	HWND                      m_hWnd;
	LmnToolkits::Thread *     m_thrd_db;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
		m_hWnd = 0;
	}
};

class CAddFloorParam : public LmnToolkits::MessageData {
public:
	CAddFloorParam(HWND hWnd, int nFloor) {
		m_hWnd   = hWnd;
		m_nFloor = nFloor;
	}

	HWND    m_hWnd;
	int     m_nFloor;
};

class CGetDevicesParam : public LmnToolkits::MessageData {
public:
	CGetDevicesParam(int nFloor) {
		m_nFloor = nFloor;
	}
	int     m_nFloor;
};

typedef  struct  tagDeviceItem {
	int     nFloor;
	int     nDeviceNo;
	char    szAddr[32];
	int     nTemp;
	time_t  time;
}DeviceItem;

// 创建duilib控件的回调
class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CDialogBuilderCallbackEx(DuiLib::CPaintManagerUI *pManager) {
		m_pManager = pManager;
	}
	CControlUI* CreateControl(LPCTSTR pstrClass);

public:
	DuiLib::CPaintManagerUI *  m_pManager;
};



extern CGlobalData  g_data;

extern bool sortInt(int d1, int d2);
extern CControlUI* CALLBACK FindControlByName(CControlUI* pSubControl, LPVOID pParam);
extern DWORD GetIntFromDb(const char * szValue, int nDefault = 0);
extern char * GetStrFromdDb(char * buf, DWORD dwSize, const char * szValue);

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