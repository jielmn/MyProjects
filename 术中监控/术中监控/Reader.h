#pragma once

#include "exhCommon.h"

class CBusiness;

class  CReader {
public:
	CReader(CBusiness * pBusiness);
	~CReader();
	
	// 重连
	int  Reconnect();
	// 获取Tag温度数据
	int  GetTagTemperatureData();

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

	// 协议部分
	int   Prepare();
	int   ReadPrepareRet();
	int   Inventory();
	int   ReadInventoryRet();
	int   ReadTagBlock( int nStartBlock, int nReadLength );
	int   ReadTagBlockRet( std::vector<TagBlock*> &v );

	int   Receive(DWORD & dwReceivedCnt);            // 接收数据
	int   ReadToken(Token & t);

	void  Clear();
private:
	READER_STATUS      m_eStatus;
	HANDLE             m_hComm;                           // 串口句柄
	CDataBuf           m_received_data;                   // 从串口接收数据的缓冲区
	CBusiness  *       m_pBusiness;

	TagItem            m_tag;                             // 清点的单个tag
	BOOL               m_bFoundTag;                       // 有无清点出的tag
};

