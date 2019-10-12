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

#define   PROJ_NAME               "UpperHost"
#define   LOG_FILE_NAME           (PROJ_NAME ".log")
#define   CONFIG_FILE_NAME        (PROJ_NAME ".cfg")
#define   MAIN_CLASS_WINDOW_NAME  "DUIMainFrame"
#define   SKIN_FILE               ("mainframe_" PROJ_NAME ".xml")
#define   SKIN_FOLDER             ("res\\" PROJ_NAME "_res")

#define   UM_WRONG_LUA            (WM_USER +1)
#define   UM_OPEN_COM_RET         (WM_USER +2)
#define   UM_CLOSE_COM_RET        (WM_USER +3)
#define   UM_WRITE_COM_RET        (WM_USER +4)
#define   UM_READ_COM_RET         (WM_USER +5)

#define   MSG_OPEN_COM             1001
#define   MSG_CLOSE_COM            1002
#define   MSG_WRITE_COM            1003
#define   MSG_READ_COM             1004

#define  MYDATA_TYPE_INT           0
#define  MYDATA_TYPE_STRING        1

class  CMyData {
public:
	CMyData() {
		m_nDataype = MYDATA_TYPE_INT;
		m_nData = 0;
	}

	int          m_nDataype;
	CDuiString   m_strData;
	int          m_nData;
};

class COpenComParam : public LmnToolkits::MessageData {
public:
	COpenComParam(int nCom) : m_nCom(nCom) { }
	
	int    m_nCom;
};

class CWriteComParam : public LmnToolkits::MessageData {
public:
	CWriteComParam(const BYTE * pData, DWORD  dwDataLen) { 
		m_pData = new BYTE[dwDataLen];
		memcpy(m_pData, pData, dwDataLen);
		m_dwDataLen = dwDataLen;
	}

	~CWriteComParam() {
		if (m_pData)
			delete[] m_pData;
	}

	BYTE *     m_pData;
	DWORD      m_dwDataLen;
};


class  CGlobalData {
public:
	ILog    *                 m_log;
	IConfig *                 m_cfg;
	LmnToolkits::Thread *     m_thrd_db;
	HWND                      m_hWnd;

public:
	CGlobalData() {
		m_log = 0;
		m_cfg = 0;
		m_thrd_db = 0;
		m_hWnd = 0;
	}
};

extern CGlobalData  g_data;

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