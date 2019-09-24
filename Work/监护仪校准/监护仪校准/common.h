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

#define   PROJ_NAME               "MonitorAdjust"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   MAX_COLUMNS_CNT          10
#define   MAX_TEMP_ITEMS_CNT       160            // 一共160个子项
#define   FIRST_TEMP               3000           // 从30℃开始

#define   MAX_MACHINE_CNT   4
enum MachineType {
	MachineType_MR = 0,
	MachineType_FLP,
	MachineType_GE,
	MachineType_GE2,
};

#define MSG_ADJUST                   1001
#define MSG_ADJUST_RET               1002

#define UM_WRONG_DATA                (WM_USER + 1)

typedef  struct  tagTempItem {
	int    m_nTemp;
	int    m_nDutyCycle;
}TempItem;

class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	TempItem                  m_standard_items[MAX_MACHINE_CNT][MAX_TEMP_ITEMS_CNT];
	HWND                      m_hWnd;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
		m_hWnd = 0;
		memset(m_standard_items, 0, sizeof(m_standard_items));
	}
};

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

class CAdjustParam : public LmnToolkits::MessageData {
public:
	CAdjustParam(int nComPort, int nTemp, int nDutyCycle) {
		m_nComPort = nComPort;
		m_nTemp = nTemp;
		m_nDutyCycle = nDutyCycle;
	}

	int m_nComPort;
	int m_nTemp;
	int m_nDutyCycle;
};


extern CGlobalData  g_data;

extern BOOL EnumPortsWdm(std::vector<std::string> & v);
extern const char * GetMachineTypeStr(MachineType e);
extern BOOL LoadStandardRes();
class CTempItemUI;
extern BOOL LoadFileData(CTempItemUI *  temp_items[MAX_TEMP_ITEMS_CNT], int nItemCnt, MachineType eType,
	const char * szFileName);

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