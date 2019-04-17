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

#define   PROJ_NAME               "temp_monitor"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#define   MAX_GRID_COUNT           60             // ��༸������
#define   MAX_READERS_PER_GRID     6              // ÿ��������༸�����µ�

// �ؼ�id
#define   TABS_ID                  "switch"
#define   TAB_INDEX_MONITOR        0
#define   TAB_INDEX_READER         1


// CONFIG
#define   CFG_MAIN_LAYOUT_COLUMNS           "main layout columns"
#define   CFG_MAIN_LAYOUT_ROWS              "main layout rows"
#define   DEFAULT_MAIN_LAYOUT_COLUMNS       2
#define   DEFAULT_MAIN_LAYOUT_ROWS          2
#define   CFG_MAIN_LAYOUT_GRIDS_COUNT       "main layout grids count"
#define   DEFAULT_MAIN_LAYOUT_GRIDS_COUNT   4

// ���ж�����
typedef struct tagReaderCfg
{
	BOOL      m_bSwitch;                             // ����������
	DWORD     m_dwLowTempAlarm;                      // ���±���(��λ�����϶�)
	DWORD     m_dwHighTempAlarm;                     // ���±���(��λ�����϶�)
}ReaderCfg;

// ���д���
typedef struct tagGridCfg
{
	BOOL        m_bSwitch;                              // �ܿ���
	DWORD       m_dwCollectInterval;                    // �ɼ����(��λ:��)
	DWORD       m_dwMinTemp;                            // ��ʾ������¶�(��λ:��)
	DWORD       m_dwMaxTemp;                            // ��ʾ������¶�(��λ:��)
	ReaderCfg   m_ReaderCfg[MAX_READERS_PER_GRID];
}GridCfg;

typedef struct tagCfgData {
	DWORD     m_dwAreaNo;                               // ���в�����
	DWORD     m_dwLayoutColumns;                        // ����
	DWORD     m_dwLayoutRows;                           // ����
	DWORD     m_dwLayoutGridsCnt;                       // ��λ����  (���ܳ���MAX_GRID_COUNT)
	BOOL      m_bAlarmVoiceOff;                         // ������������
	BOOL      m_bAutoSaveExcel;                         // �Զ�����Excel
	BOOL      m_bCrossAnchor;                           // ʮ��ê
	GridCfg   m_GridCfg[MAX_GRID_COUNT];                // ���и�������

	DWORD     m_dwHandReaderMinTemp;                    // �ֳֶ�������ʾ����¶�
	DWORD     m_dwHandReaderMaxTemp;                    // �ֳֶ�������ʾ����¶�
	DWORD     m_dwHandReaderLowTempAlarm;               // �ֳֶ��������±���
	DWORD     m_dwHandReaderHighTempAlarm;              // �ֳֶ��������±���	
}CfgData;

class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	CfgData                   m_CfgData;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
	}
};

extern CGlobalData  g_data;

extern LONG WINAPI pfnUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);

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