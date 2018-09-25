#pragma once

#include <vector>
#include "LmnCommon.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"

#define  MSG_RECONNECT_READER              1
#define  MSG_READER_HEARTBEAT              2
#define  MSG_SET_READER_ID                 3
#define  MSG_SET_READER_TIME               4
#define  MSG_SET_READER_MODE               5
#define  MSG_CLEAR_READER                  6
#define  MSG_GET_READER_DATA               7
#define  MSG_SET_READER_BLUE_TOOTH         8
#define  MSG_SET_READER_BLUE_TOOTH_NAME    9

#define  UM_READER_STATUS                  (WM_USER+1)
#define  UM_READER_ID_RET                  (WM_USER+2)
#define  UM_READER_TIME_RET                (WM_USER+3)
#define  UM_READER_MODE_RET                (WM_USER+4)
#define  UM_CLEAR_READER_RET               (WM_USER+5)
#define  UM_GET_READER_DATA_RET            (WM_USER+6)
#define  UM_READER_BLUE_TOOTH_RET          (WM_USER+7)
#define  UM_READER_BLUE_TOOTH_NAME_RET     (WM_USER+8)

class CReaderIdParam : public LmnToolkits::MessageData {
public:
	CReaderIdParam(int nId, int nVersion) {
		m_nId = nId;
		m_nVersion = nVersion;
	}
	int     m_nId;
	int     m_nVersion;
};

class CReaderTimeParam : public LmnToolkits::MessageData {
public:
	CReaderTimeParam(time_t tTime) {
		m_tTime = tTime;
	}
	time_t     m_tTime;
};

class CReaderModeParam : public LmnToolkits::MessageData {
public:
	CReaderModeParam(int nMode) {
		m_nMode = nMode;
	}
	int     m_nMode;
};

class CReaderBlueToothParam : public LmnToolkits::MessageData {
public:
	CReaderBlueToothParam(BOOL b) {
		m_bEnable = b;
	}
	BOOL   m_bEnable;
};

class CReaderBlueToothNameParam : public LmnToolkits::MessageData {
public:
	CReaderBlueToothNameParam(const char * szName) {
		STRNCPY(m_szName, szName, sizeof(m_szName));
	}
	char   m_szName[256];
};

typedef struct tagTagId {
	BYTE    abyUid[8];
	DWORD   dwLen;
}TagId;

typedef struct tagTempItem {
	time_t     tTime;
	TagId      tTagId;
	DWORD      dwTemp;
	TagId      tCardId;
}TempItem;

extern ILog    * g_log;
extern IConfig * g_cfg;
extern DWORD     g_dwReaderVersion;

const char * GetErrorDescription( int e );
char * DateTime2Str(char * szDest, DWORD dwDestSize, const time_t * t);
extern char * GetUid(char * buf, DWORD dwBufLen, const BYTE uid[], DWORD uidlen, char chSplit = '\0');

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