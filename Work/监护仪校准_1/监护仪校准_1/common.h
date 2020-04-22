#pragma once

#include <vector>
#include <algorithm>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"
#include "LmnTelSvr.h"

#include "UIlib.h"
using namespace DuiLib;

#define  NEW_VERSION_FLAG         1

#define   PROJ_NAME               "MonitorAdjust"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   MAX_COLUMNS_CNT          10
#define   MAX_TEMP_ITEMS_CNT       71             // 一共160个子项
#define   FIRST_TEMP               3500           // 从30℃开始

#if !NEW_VERSION_FLAG
#define   MAX_MACHINE_CNT   4
enum MachineType {
	MachineType_MR = 0,
	MachineType_FLP,
	MachineType_GE,
	MachineType_GE2,
};
#else
#define   MAX_MACHINE_CNT   8
enum MachineType {
	MachineType_MR = 0,
	MachineType_FLP,
	MachineType_GE,
	MachineType_GE2,
	MachineType_5,
	MachineType_6,
	MachineType_7,
	MachineType_max,
};
#endif

#define MSG_ADJUST                   1001
#define MSG_ADJUST_ALL               1002
#define MSG_SET_WORK_MODE            1003

#define UM_WRONG_DATA                (WM_USER + 1)
#define UM_ADJUST_RET                (WM_USER + 2)
#define UM_ADJUST_ALL_RET            (WM_USER + 3)
#define UM_ADJUST_ALL_PROGRESS       (WM_USER + 4)
#define UM_SET_WORK_MODE_RET         (WM_USER + 5)

typedef  struct  tagTempItem {
	int    m_nTemp;
	int    m_nDutyCycle;
}TempItem;

typedef  struct  tagTempAdjust {
	int    m_nTemp;
	int    m_nOffset;
}TempAdjust;

enum  WorkMode {
	WorkMode_Adjust = 0,
	WorkMode_Temp,
};

class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	TempItem                  m_standard_items[MAX_MACHINE_CNT][MAX_TEMP_ITEMS_CNT];
	HWND                      m_hWnd;
	DWORD                     m_dwSleepTime;
	BOOL                      m_bQuit;
	DWORD                     m_dwBaud;

#if NEW_VERSION_FLAG
	std::vector<std::string>  m_vOtherMachineType;   // 除去4种基本类型之外的机器类型
#endif

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
		m_hWnd = 0;
		m_dwSleepTime = 1000;
		m_bQuit = FALSE;
		m_dwBaud = 9600;
#if !NEW_VERSION_FLAG
		memset(m_standard_items, 0, sizeof(m_standard_items));
#else
		for (int i = 0; i < MAX_MACHINE_CNT; i++) {
			for (int j = 0; j < MAX_TEMP_ITEMS_CNT; j++) {
				m_standard_items[i][j].m_nDutyCycle = 1000;
			}
		}
#endif
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

class CAdjustAllParam : public LmnToolkits::MessageData {
public:
	CAdjustAllParam(int nComPort, MachineType eType, TempAdjust * items, DWORD dwSize) {
		m_nComPort = nComPort;
		m_eType = eType;
		assert(dwSize == MAX_TEMP_ITEMS_CNT);
		memcpy(m_items, items, sizeof(TempAdjust) * MAX_TEMP_ITEMS_CNT);
		m_dwSize = dwSize;
	}

	int          m_nComPort;
	MachineType  m_eType;
	TempAdjust   m_items[MAX_TEMP_ITEMS_CNT];
	DWORD        m_dwSize;
};

class CWorkModeParam : public LmnToolkits::MessageData {
public:
	CWorkModeParam(int nComPort, WORD wReaderAddr, WorkMode  eMode) {
		m_nComPort = nComPort;
		m_wAddr = wReaderAddr;
		m_eMode = eMode;
	}

	int          m_nComPort;
	WORD         m_wAddr;
	WorkMode     m_eMode;
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