#pragma once

#include <vector>
#include <algorithm>
#include <iterator> 
#include <map>

#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"
#include "LmnString.h"
#include "LmnSerialPort.h"

#define   PROJ_NAME               "primaryschool"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\proj_" PROJ_NAME "_res")

#ifdef _DEBUG
#define  TEST_FLAG     1
#define  SHOW_TAG_ID   1
#endif

#ifdef _DEBUG
#define  WARNING_TIME_ELAPSED          10                             // 单位秒
#else
#define  WARNING_TIME_ELAPSED          86400                          // 单位秒
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
#define   MSG_CHECK_LAUNCH_STATUS   107
#define   MSG_READ_COM_PORTS        108
#define   MSG_TEMPERATURE_ITEM      109
#define   MSG_BINDING_TAG           110
#define   MSG_DISABLE_BINDING_TAG   111
#define   MSG_UPGRADE               112

#define   UM_ADD_CLASS_RET          (WM_USER + 1)
#define   UM_GET_ALL_CLASSES_RET    (WM_USER + 2)
#define   UM_GET_ROOM_RET           (WM_USER + 3)
#define   UM_MODIFY_DESK_RET        (WM_USER + 4)
#define   UM_EMPTY_DESK_RET         (WM_USER + 5)
#define   UM_DELETE_CLASS_RET       (WM_USER + 6)
#define   UM_EXCHANGE_DESK_RET      (WM_USER + 7)
#define   UM_CHECK_COM_PORTS_RET    (WM_USER + 8)
#define   UM_NEW_TAG_TEMPERATURE    (WM_USER + 9)
#define   UM_BINDING_TAG_RET        (WM_USER + 10)
#define   UM_DESK_TEMPERATURE       (WM_USER + 11)
#define   UM_DISABLE_BINDING_RET    (WM_USER + 12)
#define   UM_UPGRADE_RET            (WM_USER + 13)


#define   CFG_LAUNCH_COM_PORT               "launch com ports"

// 如果3分钟没有收到数据，重新初始化串口(写发送命令)
#define  REINIT_COM_PORT_TIME         180

#define  HIGH_TEMP_COLOR            0xFFFF0000
#define  LOW_TEMP_COLOR             0xFF0000FF
#define  NORMAL_TEMP_COLOR          0xFF447AA1

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


#define   MAX_COM_PORTS_CNT            4
#define   DEFAULT_READER_NO_START      1
#define   DEFAULT_READER_NO_END        10

class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	LmnToolkits::Thread *     m_thrd_launch_cube;
	DWORD                     m_dwRoomRows;
	DWORD                     m_dwRoomCols;
	SIZE                      m_DeskSize;
	HWND                      m_hWnd;
	char                      m_aszChNo[10][6];

	BOOL                      m_bMultipleComport;
	BOOL                      m_bSpecifiedComports;
	int                       m_nComports[MAX_COM_PORTS_CNT];     // 使用哪些指定串口
	int                       m_nComportsCnt;
	int                       m_nReaderNoStart;
	int                       m_nReaderNoEnd;
	DWORD                     m_dwLowTemp;
	DWORD                     m_dwHighTemp;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
		m_thrd_launch_cube = 0;
		m_dwRoomRows = 0;
		m_dwRoomCols = 0;
		memset(&m_DeskSize, 0, sizeof(m_DeskSize));
		memset(m_aszChNo, 0, sizeof(m_aszChNo));
		m_hWnd = 0;
		m_bMultipleComport = FALSE;
		m_bSpecifiedComports = FALSE;
		m_nReaderNoStart = 0;
		m_nReaderNoEnd = 0;
		m_dwLowTemp = 0;
		m_dwHighTemp = 0;
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

#define  MAX_TAG_ID_LENGTH          20
#define  MAX_READER_ID_LENGTH       20

// 温度数据
typedef struct  tagTempItem {
	DWORD   m_dwTemp;                                        // 温度
	time_t  m_time;                                          // 时间
	char    m_szTagId[MAX_TAG_ID_LENGTH];                    // tag id
	char    m_szReaderId[MAX_READER_ID_LENGTH];              // reader id
}TempItem;


class CTempParam : public LmnToolkits::MessageData {
public:
	CTempParam(TempItem * pItem) {
		memcpy(&m_item, pItem, sizeof(TempItem));
	}
	TempItem   m_item;
};

class CBindingTagParam : public LmnToolkits::MessageData {
public:
	CBindingTagParam(const char * szTagId, DWORD  dwClassNo, DWORD  dwPos) {
		m_dwNo = dwClassNo;
		m_dwPos = dwPos;
		STRNCPY(m_szTagId, szTagId, sizeof(m_szTagId));
	}
	DWORD   m_dwNo;
	DWORD   m_dwPos;
	char    m_szTagId[20];
};

class CDisableBindingTagParam : public LmnToolkits::MessageData {
public:
	CDisableBindingTagParam(WORD wClassNo, WORD  wPos) {
		m_wNo  = wClassNo;
		m_wPos = wPos;
	}
	WORD   m_wNo;
	WORD   m_wPos;
};


extern CGlobalData  g_data;

extern DWORD GetIntFromDb(const char * szValue, int nDefault = 0);
extern char * GetStrFromdDb(char * buf, DWORD dwSize, const char * szValue);
extern bool sortDWord(DWORD d1, DWORD d2);
extern int  GetCh340Count(char * szComPort, DWORD dwComPortLen);
extern void InitComPort(CLmnSerialPort * pPort);
extern char *  GetTagId(char * szTagId, DWORD dwTagIdLen, const BYTE * pData, DWORD dwDataLen);
extern char *  GetHandReaderId(char * szReaderId, DWORD dwReaderIdLen, const BYTE * pData);

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