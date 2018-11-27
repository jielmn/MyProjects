#pragma once

#include "common.h"
#include "LmnSerialPort.h"
#include "sigslot.h"

class  CLaunch : public CLmnSerialPort {
public:
	CLaunch();
	~CLaunch();

	// ����
	int  Reconnect();

	// ��ȡ�¶�
	int  QueryTemperature(DWORD dwArea, DWORD dwBedID);

	// ��ȡ�������ݲ���������
	int  ReadComData();

	BOOL WriteLaunch(const void * WriteBuf, DWORD & WriteDataLen);

	// Ӳ���Ķ������״̬
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
