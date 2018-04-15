#pragma once

#include "zsCommon.h"

class CBusiness;

class  CZsSyncReader {
public:
	CZsSyncReader(CBusiness * pBusiness);
	~CZsSyncReader();

	// 重连
	int Reconnect();
	// 检查心跳
	int CheckHeartBeat();
	// 同步数据
	int Synchronize(std::vector<SyncItem*> & v);
	// 清空数据
	int ClearReader();

	enum SYNC_READER_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	SYNC_READER_STATUS GetStatus();

private:
	void Clear();

	SYNC_READER_STATUS     m_eReaderStatus;	
	CBusiness  *           m_pBusiness;

	HANDLE                 m_hComm;                           // 串口句柄
	CDataBuf               m_received_data;                   // 从串口接收数据的缓冲区

private:
	// 串口连接，关闭处理
	// 打开串口
	BOOL      OpenUartPort(const char *UartPortName);
	// 关闭串口
	BOOL      CloseUartPort();

	BOOL      InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits);
	BOOL      WriteUartPort(HANDLE hComm, const void * WriteBuf, DWORD ToWriteDataLen, DWORD *WritedDataLen);
	BOOL      RcvDataFromUartPort(HANDLE hComm, void *RcvBuf, DWORD ToRcvDataLen, DWORD *RcvedDataLen, LPOVERLAPPED lpOverlapped);


	// Reader协议处理
	int   Prepare();
	int   ReadPrepareRet();
	int   SyncData();
	int   ReadSyncDataRet(std::vector<SyncItem*> & v);
	int   ClearData();
	int   ReadClearDataRet();


	int   Receive(DWORD & dwReceivedCnt);            // 接收数据
};

