#pragma once

#include "common.h"
#include "LmnSerialPort.h"

class CBusiness;

class  CLaunch : public CLmnSerialPort {
public:
	CLaunch( );
	~CLaunch();

	// 重连
	int  Reconnect();

	// 硬件改动，检查状态
	int  CheckStatus();

	// 心跳
	int  HeartBeat(const CReaderHeartBeatParam * pParam);

	// 获取温度
	int  QueryTemperature(const CGetTemperatureParam * pParam);

	// 读取串口数据并处理数据
	int  ReadComData();

private:
	DWORD      m_dwGridRetryTime[MAX_GRID_COUNT];
	CDataBuf   m_recv_buf;
	//DWORD      m_dwLastWriteTick;

private:
	void   CloseLaunch();
	BOOL   WriteLaunch(const void * WriteBuf, DWORD & WriteDataLen);
};