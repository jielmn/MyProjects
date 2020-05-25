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

#define   PROJ_NAME               "building_temp_monitor"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   MSG_ADD_FLOOR             100

#define   UM_ADD_FLOOR_RET          (WM_USER + 1)


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

// ����duilib�ؼ��Ļص�
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