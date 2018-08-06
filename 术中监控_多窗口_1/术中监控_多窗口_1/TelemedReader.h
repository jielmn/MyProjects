#pragma once

#include "common.h"

class CBusiness;

class  CTelemedReader {
public:
	CTelemedReader(CBusiness * pBusiness = 0);
	~CTelemedReader();

	// 重连
	int  Reconnect();
	// 读取温度
	int ReadTagTemp(DWORD & dwTemp);
	
	enum READER_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	READER_STATUS GetStatus();

private:
	// 串口连接，关闭处理
	// 打开串口
	BOOL      OpenUartPort(const char *UartPortName);
	// 关闭串口
	BOOL      CloseUartPort();

	BOOL      InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits);
	BOOL      WriteUartPort(HANDLE hComm, const void * WriteBuf, DWORD ToWriteDataLen, DWORD *WritedDataLen);
	BOOL      RcvDataFromUartPort(HANDLE hComm, void *RcvBuf, DWORD ToRcvDataLen, DWORD *RcvedDataLen, LPOVERLAPPED lpOverlapped);

	// 数据接收
	int       Receive(DWORD & dwReceivedCnt);
	// 数据清除
	void      Clear();


	// 协议部分
	int   Prepare();
	int   ReadPrepareRet();

	void  ReceiveAsPossible(DWORD dwMaxTime, DWORD dwMaxDataLength);
private:
	READER_STATUS      m_eStatus;
	HANDLE             m_hComm;                           // 串口句柄
	CDataBuf           m_received_data;                   // 从串口接收数据的缓冲区
public:
	BOOL               m_bStop;

public:
	CBusiness  *       m_pBusiness;

public:
	int                m_nIndex;
	char               m_szComPort[32];
};


