
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
#include "LmnString.h"
#include "LmnConfig.h"
#include "LmnLog.h"
#include "LmnThread.h"



enum ComStatus
{
	// main status
	COM_STATUS_CLOSED = 0,
	COM_STATUS_OPEN,

	// open status, sub status
	COM_STATUS_IDLE,
	COM_STATUS_PREPARING,
	COM_STATUS_BINDIND,
	COM_STATUS_UPLOADING,
	COM_STATUS_CLEARING,
};

enum  DbStatus {
	DB_STATUS_CLOSED = 0,
	DB_STATUS_OPEN,
};

// 温度贴数据
typedef struct tagTagData {
	time_t     tTime;
	BYTE       abyTagId[8];             // tag id
	DWORD      dwTemperatur;           // 温度数据，如3500表示35.00
}TagData;

#include "SerialPort.h"
#include "mysql.h"
#include "MyDatabase.h"

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#define  MSG_RECONNECT_SERIAL_PORT                   1
#define  MSG_SYNC                                    2
#define  MSG_PREPARE                                 3
#define  MSG_RECEIVE_SERIAL_PORT_DATA                4
#define  MSG_RECONNECT_DB                            5
#define  MSG_UPDATE_FEVER_DATA                       6
#define  MSG_CLEAR_READER                            7

#define  UM_SERIAL_PORT_STATE                        (WM_USER+1)
#define  UM_SYNC_RESULT                              (WM_USER+2)
#define  UM_SYNC                                     (WM_USER+3)
#define  UM_DB_STATE                                 (WM_USER+4)

#define PROJ_CAPTION                                  "发热疫情上传"

typedef struct tagReaderCmd {
	BYTE      abyCommand[256];
	DWORD     dwCommandLength;
}ReaderCmd;

class MyMessageHandler : public LmnToolkits::MessageHandler {
public:
	MyMessageHandler();
	~MyMessageHandler();
	BOOL CanBeFreed() { return false; }
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
};

class CUpdateFeverData : public LmnToolkits::MessageData {
public:
	CUpdateFeverData(DWORD dwIndex, DWORD dwCount) {
		m_dwHospitalIndex = dwIndex;
		m_dwFeverCount = dwCount;
	}
	~CUpdateFeverData() {

	}

	DWORD   m_dwHospitalIndex;
	DWORD   m_dwFeverCount;
};


extern IConfig *  g_cfg;
extern ILog *     g_log;
extern LmnToolkits::Thread *  g_thrd_db;
extern LmnToolkits::Thread *  g_thrd_serial_port;
extern MyMessageHandler *     g_handler;
extern CSerialPort *          g_serial_port;
extern CMyDatabase *          g_db;
extern HWND                   g_hwndMain;
extern ReaderCmd              SYNC_COMMAND;
extern ReaderCmd              PREPARE_COMMAND;
extern ReaderCmd              CLEAR_COMMAND;

extern char * GetUid(char * buf, DWORD dwBufLen, const BYTE uid[], DWORD uidlen, char chSplit = '\0');
extern BOOL GetBoolean(const char * szStr);

extern int TransferReaderCmd(ReaderCmd & cmd, const char * szCmd);

extern void ReconnectDb(BOOL bRightNow = FALSE);
extern void ReconnectSerialPort(BOOL bRightNow = FALSE);
extern void Sync();
extern void Prepare();
extern void ClearReader();
extern void ReceiveSerialPortData();                      // 接收串口数据
extern void UpdateFeverData(DWORD dwHospitalIndex, DWORD dwFeverCnt);

extern void NotifyUiSerialPortStatus();
extern void NotifyUiSyncRet(std::vector<TagData *> * pVec);
extern void NotifyUiDbStatus();


template <class T>
void ClearVector(std::vector<T> & v) {
	typedef std::vector<T>::iterator v_it;
	v_it it;
	for (it = v.begin(); it != v.end(); it++) {
		delete *it;
	}
	v.clear();
}