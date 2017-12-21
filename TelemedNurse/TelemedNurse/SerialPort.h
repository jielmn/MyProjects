#pragma once

#include <afx.h>
#include <vector>
#include "Business.h"
#include "LmnThread.h"

enum  SerialPortStatus {
	SERIAL_PORT_STATUS_OPENING = 0,
	SERIAL_PORT_STATUS_OPEND,
	SERIAL_PORT_STATUS_CLOSED,
};

enum SerialPortSyncError {
	SERIAL_PORT_SYNC_ERROR_OK = 0,
	SERIAL_PORT_SYNC_ERROR_NOT_OPENED,
	SERIAL_PORT_SYNC_ERROR_FAILED_TO_SYNC,
	SERIAL_PORT_SYNC_ERROR_FAILED_TO_RECEIVE_RET,
};

class CSerialPort {
public:
	CSerialPort();
	~CSerialPort();

	int   Prepare();
	int   ReadPrepareRet();

	int   Sync();
	int   ReadSyncRet( std::vector<TagData*> & v );

	int   Clear();
	int   ReadClearRet();

	void  NotifySerialPortStatus( SerialPortStatus eStatus );
	void  NotifySerialPortSyncRet(SerialPortSyncError eError, std::vector<TagData*> * pVRet = 0 );

	BOOL  IfOpened();

	static CSerialPort *  GetInstance();

private:
	static CSerialPort *  pInstance;
	HANDLE     m_hComm;
	CDataBuf   m_received_data;

public:
	// 打开串口
	BOOL      OpenUartPort( const char *UartPortName);
	// 关闭串口
	BOOL      CloseUartPort();

	BOOL      InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits);	
	BOOL      WriteUartPort(HANDLE hComm, const void * WriteBuf, DWORD ToWriteDataLen, DWORD *WritedDataLen);
	BOOL      RcvDataFromUartPort(HANDLE hComm, void *RcvBuf, DWORD ToRcvDataLen, DWORD *RcvedDataLen, LPOVERLAPPED lpOverlapped);

	int        Receive(DWORD & dwReceivedCnt);            // 接收数据
	int        SkipInstantResp();                         // 跳过温度贴的及时反馈
};



extern LmnToolkits::Thread  * g_thrd_serial_port;


class MessageHandlerSerialPort : public LmnToolkits::MessageHandler {
public:
	MessageHandlerSerialPort();
	~MessageHandlerSerialPort();
	BOOL CanBeFreed() { return false; }
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
};

extern MessageHandlerSerialPort * g_handler_serial_port;

#define  MSG_RECONNECT_SERIAL_PORT      100
#define  DELAY_RECONNECT_SERIAL_PORT    10000

#define  MSG_SERIAL_PORT_HEART_BEAT     101
#define  DELAY_SERIAL_PORT_HEART_BEAT   10000

#define  MSG_SERIAL_PORT_SYNC           102
#define  MSG_SERIAL_PORT_CLEAR          103


void  SyncReader();
void  ClearReader();