#pragma once

#include "common.h"
#include "LmnSerialPort.h"

class CBusiness;

class  CLaunch : public CLmnSerialPort {
public:
	CLaunch( );
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

private:
	void   CloseLaunch();
	BOOL   WriteLaunch(const void * WriteBuf, DWORD & WriteDataLen);
};