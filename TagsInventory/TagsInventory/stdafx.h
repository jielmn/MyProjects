
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类
#include <afxdb.h>


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持
#include <vector>
#include <algorithm>



#include "LmnContainer.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"


#include "inc/rfidlib.h"
#include "inc/rfidlib_reader.h"
#include "inc/rfidlib_AIP_ISO15693.h"
#include "inc/rfidlib_aip_iso14443A.h"
#include "inc/rfidlib_aip_iso18000p3m3.h"

typedef struct tagTagItem {
	BYTE     abyUid[32];
	DWORD    dwUidLen;
}TagItem;

class CSaveInvParam : public LmnToolkits::MessageData {
public:
	CSaveInvParam();
	~CSaveInvParam();

	CString                  m_strBatchId;
	std::vector<TagItem *>   m_items;
};

#include "MyDatabase.h"
#include "MyReader.h"

#include "UIlib.h"
using namespace DuiLib;

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

extern IConfig *  g_cfg;
extern IConfig *  g_cfg_change;              // 保存上一次用户名
extern ILog *     g_log;
extern CMyDatabase * g_mydb;
extern CMyReader   * g_myreader;
extern BOOL          g_bLogVerifyID;

extern LmnToolkits::Thread *  g_thrd_db;
extern LmnToolkits::Thread *  g_thrd_reader;

typedef struct tagLoginUser {
	char     user_id[32];
	char     user_name[32];
	TagItem  user_tag_id;
}LoginUser;


class CTagItem : public LmnToolkits::MessageData {
public:
	CTagItem(const TagItem * pItem);
	~CTagItem();

	TagItem   m_item;
};

class CQueryParam : public LmnToolkits::MessageData {
public:
	CQueryParam( const char * szStartTime, const char * szEndTime, const char * szOperatoe ) {
		if (szStartTime) {
			strncpy(m_szStartTime, szStartTime, sizeof(m_szStartTime) - 1);
		}
		if (szEndTime) {
			strncpy(m_szEndTime, szEndTime, sizeof(m_szEndTime) - 1);
		}
		if (szOperatoe) {
			strncpy(m_szOperator, szOperatoe, sizeof(m_szOperator) - 1);
		}
	}
	~CQueryParam() {}

	char   m_szStartTime[32];
	char   m_szEndTime[32];
	char   m_szOperator[32];
};

typedef struct tagDbErrMsg {
	DWORD    dwErrId;
	BOOL     bDisconnected;                 // 是否需要重新连接数据库
	char     szErrDescription[256];
	char     szBatchId[32];                 // 保存批号
	void *   pParam;
}DbErrMsg;

class MyMessageHandlerDb : public LmnToolkits::MessageHandler {
public:
	MyMessageHandlerDb();
	~MyMessageHandlerDb();
	BOOL CanBeFreed() { return false; }
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
};

class MyMessageHandlerReader : public LmnToolkits::MessageHandler {
public:
	MyMessageHandlerReader();
	~MyMessageHandlerReader();
	BOOL CanBeFreed() { return false; }
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
};

typedef struct tagQueryResult {
	DWORD     dwId;
	int       nProcType;
	char      szOperator[32];
	char      szProcTime[32];
	DWORD     dwTagsCnt;
	char      szBatchId[32];
}QueryResult;

extern LoginUser                g_login_user;
extern MyMessageHandlerDb *     g_handler_db;
extern MyMessageHandlerReader * g_handler_reader;

extern HWND                     g_hDlg;
extern HWND						g_hQueryDlg;

#define STATUS_OPEN            1
#define STATUS_CLOSE           0

#define STATUS_SAVING          2

#define STATUS_TYPE_DATABASE   0
#define STATUS_TYPE_READER     1

// thread message
#define MSG_RECONNECT_DB           1
#define MSG_RECONNECT_READER       2
#define MSG_READER_INVENTORY       3
#define MSG_VERIFY_USER            4
#define MSG_SAVE_INVENTORY         5
#define MSG_QUERY                  6

// windows message
#define UM_SHOW_STATUS         (WM_USER+1)
#define UM_TAG_ITEM            (WM_USER+2)
#define UM_VERIFY_USER_RESULT  (WM_USER+3)
#define UM_SAVE                (WM_USER+4)
#define UM_TWO_DIMENSION_CODE  (WM_USER+5)
#define UM_NEW                 (WM_USER+6)
#define UM_DB_ERR_MSG          (WM_USER+7)
#define UM_QUERY               (WM_USER+8)

#define  MSG_BOX_CAPTION        "盘点"
#define  RECONNECT_DB_TIME               10000


extern bool operator == (const TagItem & t1, const TagItem & t2);
extern char * GetUid(char * buf, DWORD dwBufLen, const BYTE uid[], DWORD uidlen, char chSplit = '\0');
extern BOOL GetBoolean(const char * szStr);
typedef void(*CB_TAG_REPORT_HANDLE)(void *cbParam, RFID_READER_HANDLE hreader, DWORD AIPtype, DWORD TagType, DWORD AntId, BYTE uid[], WORD uidlen);
extern err_t rfid_inventory(RFID_READER_HANDLE hreader, BOOLEAN newAI, BYTE Antennas[], BYTE AntCount,
							BOOLEAN enISO15693, BOOLEAN enAFI, BYTE afi, BOOLEAN enISO14443A, BOOLEAN enISO18000p3m3,
							CB_TAG_REPORT_HANDLE tag_report_handler, void *cbParam, DWORD *tagCount);
extern char *  GetOperateType(char * buf, DWORD dwBufSize, DWORD dwOperateType);

extern char *  GetNum(char * buf, DWORD dwBufSize, DWORD dwNum);

template <class T>
void ClearVector(vector<T> & v) {
	typedef vector<T>::iterator v_it;
	v_it it;
	for (it = v.begin(); it != v.end(); it++) {
		delete *it;
	}
	v.clear();
}

#define DEBUG_FLAG           0