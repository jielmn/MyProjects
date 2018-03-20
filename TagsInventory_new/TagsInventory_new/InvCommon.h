#pragma once

#include <vector>
#include "sigslot.h"
#include "LmnContainer.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"

#include "inc/rfidlib.h"
#include "inc/rfidlib_reader.h"
#include "inc/rfidlib_AIP_ISO15693.h"
#include "inc/rfidlib_aip_iso14443A.h"
#include "inc/rfidlib_aip_iso18000p3m3.h"

extern ILog    * g_log;
extern IConfig * g_cfg;
extern HWND    g_hWnd;
extern LmnToolkits::Thread *  g_thrd_db;
extern LmnToolkits::Thread *  g_thrd_reader;

// thread message
#define MSG_RECONNECT_DB           1
#define MSG_RECONNECT_READER       2
#define MSG_READER_INVENTORY       3
#define MSG_USER_LOGIN             4

// db reconnect 时间
#define  RECONNECT_DB_TIME         10000
#define  RECONNECT_READER_TIME     10000

// 错误码
#define INV_ERR_NO_MEMORY          10001

// windows 自定义消息
#define UM_SHOW_DB_STATUS          (WM_USER+1)
#define UM_SHOW_READER_STATUS      (WM_USER+2)
#define UM_INVENTORY_RESULT        (WM_USER+3)
#define UM_LOGIN_RESULT            (WM_USER+4)


// config配置字符串常量
#define ODBC_TYPE                  "odbc type"
#define ODBC_STRING_ORACLE         "odbc string oracle"
#define ODBC_STRING_MYSQL          "odbc string mysql"


char * MyEncrypt(const void * pSrc, DWORD dwSrcSize, char * dest, DWORD dwDestSize);
int    MyDecrypt(const char * szSrc, void * pDest, DWORD & dwDestSize);


#define  DB_STATUS_OK_TEXT             "数据库连接OK"
#define  DB_STATUS_CLOSE_TEXT          "数据库连接失败"
#define  READER_STATUS_OK_TEXT         "读卡器连接OK"
#define  READER_STATUS_CLOSE_TEXT      "读卡器连接失败"

#define  SET_CONTROL_TEXT(ctrl,text)    do { if ( ctrl ) { ctrl->SetText(text); } } while( 0 )

#define  MAX_TAG_ID_LENGTH              32
typedef struct tagTagItem {
	BYTE     abyUid[MAX_TAG_ID_LENGTH];
	DWORD    dwUidLen;
}TagItem;

class CTagItemParam : public LmnToolkits::MessageData {
public:
	CTagItemParam(const TagItem * pItem);
	~CTagItemParam();

	TagItem   m_item;
};

#define   MAX_USER_ID_LENGTH             32
#define   MAX_USER_NAME_LENGTH           32
typedef struct tagUser {
	char     szUserId[MAX_USER_ID_LENGTH];
	char     szUserName[MAX_USER_NAME_LENGTH];
	TagItem  tTagId;

	// 以下是权限
	// 权限
}User;


typedef void(*CB_TAG_REPORT_HANDLE)(void *cbParam, RFID_READER_HANDLE hreader, DWORD AIPtype, DWORD TagType, DWORD AntId, BYTE uid[], WORD uidlen);
extern err_t rfid_inventory(RFID_READER_HANDLE hreader, BOOLEAN newAI, BYTE Antennas[], BYTE AntCount,
	BOOLEAN enISO15693, BOOLEAN enAFI, BYTE afi, BOOLEAN enISO14443A, BOOLEAN enISO18000p3m3,
	CB_TAG_REPORT_HANDLE tag_report_handler, void *cbParam, DWORD *tagCount);

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