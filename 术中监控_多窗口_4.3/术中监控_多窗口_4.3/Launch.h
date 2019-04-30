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
	int  QueryTemperature(BYTE byArea, WORD wBedID);

	// 读取串口数据并处理数据
	int  ReadComData();

	BOOL WriteLaunch(const void * WriteBuf, DWORD & WriteDataLen);

	// 硬件改动，检查状态
	int  CheckStatus();

private:
	void   CloseLaunch();
	void   ProcSurReader(const BYTE * pData, DWORD dwDataLen);

private:
	CDataBuf   m_recv_buf;

public:
	sigslot::signal1<PortStatus>                      m_sigStatus;
	sigslot::signal3<DWORD, DWORD, ReaderStatus>      m_sigReaderStatus;          // params: index, subindex, status
	sigslot::signal2<WORD, const TempItem &>          m_sigReaderTemp;            // 术中温度
	sigslot::signal1<const TempItem &>                m_sigHandReaderTemp;        // 手持温度
};
