#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"
#include "LuaConfig.h"
#include "UIlib.h"
using namespace DuiLib;

#define   PROJ_NAME               "wearable_monitor"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   VERSION                     "1.0"
#define   COMPILE_TIME                "2019-12-18 16:00"

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

typedef struct tagWearableData {
	int    nBeat;
	int    nOxy;
	int    nTemp;
}WearableData;


class  CGlobalData {
public:
	ILog    *                 m_log;
	CLuaCfg *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	HWND                      m_hWnd;
	int                       m_nComPort;
	BOOL                      m_bShowHistory;
	BOOL                      m_bWarningPrepose;
	BOOL                      m_bRecycle;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
		m_hWnd = 0;
		m_nComPort = 0;
		m_bShowHistory = FALSE;
		m_bWarningPrepose = TRUE;
		m_bRecycle = FALSE;
	}
};

extern CGlobalData  g_data;


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