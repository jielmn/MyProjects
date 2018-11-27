#pragma once

#include "common.h"
#include "LmnSerialPort.h"
#include "sigslot.h"

class  CLaunch : public CLmnSerialPort {
public:
	CLaunch();
	~CLaunch();

	// 重连
	int  Reconnect();

	// 获取温度
	int  QueryTemperature(DWORD dwArea, DWORD dwBedID);

	// 读取串口数据并处理数据
	int  ReadComData();

	BOOL WriteLaunch(const void * WriteBuf, DWORD & WriteDataLen);

	// 硬件改动，检查状态
	int  CheckStatus();

private:
	void   CloseLaunch();

private:
	CDataBuf   m_recv_buf;

public:
	sigslot::signal1<PortStatus>          m_sigStatus;
	sigslot::signal1<DWORD>               m_sigReconnect;
	sigslot::signal3<DWORD, DWORD, DWORD> m_sigReaderStatus;
	sigslot::signal3<DWORD, DWORD, DWORD> m_sigReaderTemp;
	sigslot::signal0<>                    m_sigCheck;
};
