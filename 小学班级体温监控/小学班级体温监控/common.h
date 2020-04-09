#pragma once

#include <vector>
#include <algorithm>

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

typedef  struct  tagDeskItem{
	int     nRow;
	int     nCol;
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