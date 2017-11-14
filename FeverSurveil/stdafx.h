
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

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

// �¶�������
typedef struct tagTagData {
	time_t     tTime;
	BYTE       abyTagId[8];             // tag id
	DWORD      dwTemperatur;           // �¶����ݣ���3500��ʾ35.00
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

#define PROJ_CAPTION                                  "���������ϴ�"

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
extern void ReceiveSerialPortData();                      // ���մ�������
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