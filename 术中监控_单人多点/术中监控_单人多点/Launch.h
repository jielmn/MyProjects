#pragma once

#include "common.h"
#include "LmnSerialPort.h"
#include "sigslot.h"

class CBusiness;

class  CLaunch : public CLmnSerialPort {
public:
	CLaunch(CBusiness * pBusiness);
	~CLaunch();

	// ����
	int  Reconnect();

	// Ӳ���Ķ������״̬
	int  CheckStatus();

	// ����
	int  HeartBeat(const CReaderHeartBeatParam * pParam);

	// ��ȡ�¶�
	int  QueryTemperature(const CGetTemperatureParam * pParam);

	// ��ȡ�������ݲ���������
	int  ReadComData();

private:
	DWORD      m_dwGridRetryTime[MAX_GRID_COUNT];
	CDataBuf   m_recv_buf;
	//DWORD      m_dwLastWriteTick;

public:
	void   CloseLaunch();

private:
	BOOL   WriteLaunch(const void * WriteBuf, DWORD & WriteDataLen);

private:
	sigslot::signal0<>                               sigWrongFormat;
	sigslot::signal1<DWORD>                          sigHeartBeatOk;
	sigslot::signal2<DWORD, DWORD>                   sigTempOk;
};