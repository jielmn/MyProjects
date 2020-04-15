#pragma once

#include <vector>
#include <algorithm>
#include <iterator> 

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"

#define   PROJ_NAME               "primaryschool"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#ifdef _DEBUG
#define  TEST_FLAG     1
#endif

#define   MAX_CLASS_NUM             10
#define   MAX_ROOM_COLUMN           10

#define   MSG_ADD_CLASS             100
#define   MSG_GET_ALL_CLASSES       101
#define   MSG_GET_ROOM              102
#define   MSG_MODIFY_DESK           103
#define   MSG_EMPTY_DESK            104
#define   MSG_DELETE_CLASS          105
#define   MSG_EXCHANGE_DESK         106

#define   UM_ADD_CLASS_RET          (WM_USER + 1)
#define   UM_GET_ALL_CLASSES_RET    (WM_USER + 2)
#define   UM_GET_ROOM_RET           (WM_USER + 3)
#define   UM_MODIFY_DESK_RET        (WM_USER + 4)
#define   UM_EMPTY_DESK_RET         (WM_USER + 5)
#define   UM_DELETE_CLASS_RET       (WM_USER + 6)
#define   UM_EXCHANGE_DESK_RET      (WM_USER + 7)

typedef  struct  tagDeskItem{
	int     nGrade;
	int     nClass;
	int     nRow;
	int     nCol;
	BOOL    bValid;
	char    szName[12];
	int     nSex;
	char    szTagId[20];
	int     nTemp;
	time_t  time;
}DeskItem;


class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	DWORD                     m_dwRoomRows;
	DWORD                     m_dwRoomCols;
	SIZE                      m_DeskSize;
	HWND                      m_hWnd;
	char                      m_aszChNo[10][6];

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
		m_dwRoomRows = 0;
		m_dwRoomCols = 0;
		memset(&m_DeskSize, 0, sizeof(m_DeskSize));
		memset(m_aszChNo, 0, sizeof(m_aszChNo));
		m_hWnd = 0;
	}
};

class CAddClassParam : public LmnToolkits::MessageData {
public:
	CAddClassParam(HWND hWnd, DWORD dwGrade, DWORD  dwClass) {
		m_hWnd = hWnd;
		m_dwGrade = dwGrade;
		m_dwClass = dwClass;
	}

	HWND    m_hWnd;
	DWORD   m_dwGrade;
	DWORD   m_dwClass;
};

class CGetRoomParam : public LmnToolkits::MessageData {
public:
	CGetRoomParam(DWORD  dwNo) {
		m_dwNo = dwNo;
	}
	DWORD   m_dwNo;
};

class CModifyDeskParam : public LmnToolkits::MessageData {
public:
	CModifyDeskParam(DWORD dwClassNo, DWORD dwPos, const char * szName, int nSex) {
		m_dwNo = dwClassNo;
		m_dwPos = dwPos;
		STRNCPY(m_szName, szName, sizeof(m_szName));
		m_nSex = nSex;
	}
	DWORD   m_dwNo;
	DWORD   m_dwPos;
	char    m_szName[12];
	int     m_nSex;
};

class CEmptyDeskParam : public LmnToolkits::MessageData {
public:
	CEmptyDeskParam(DWORD dwClassNo, DWORD dwPos) {
		m_dwNo = dwClassNo;
		m_dwPos = dwPos;
	}
	DWORD   m_dwNo;
	DWORD   m_dwPos;
};

class CDeleteClassParam : public LmnToolkits::MessageData {
public:
	CDeleteClassParam(DWORD dwClassNo) {
		m_dwNo = dwClassNo;
	}
	DWORD   m_dwNo;
};

class CExchangeDeskParam : public LmnToolkits::MessageData {
public:
	CExchangeDeskParam(DWORD  dwClassNo, DWORD  dwPos1, DWORD  dwPos2) {
		m_dwNo = dwClassNo;
		m_dwPos1 = dwPos1;
		m_dwPos2 = dwPos2;
	}
	DWORD   m_dwNo;
	DWORD   m_dwPos1;
	DWORD   m_dwPos2;
};




extern CGlobalData  g_data;

extern DWORD GetIntFromDb(const char * szValue, int nDefault = 0);
extern char * GetStrFromdDb(char * buf, DWORD dwSize, const char * szValue);
extern bool sortDWord(DWORD d1, DWORD d2);

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