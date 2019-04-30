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
	int  QueryTemperature(BYTE byArea, WORD wBedID);

	// ��ȡ�������ݲ���������
	int  ReadComData();

	BOOL WriteLaunch(const void * WriteBuf, DWORD & WriteDataLen);

	// Ӳ���Ķ������״̬
	int  CheckStatus();

private:
	void   CloseLaunch();
	void   ProcSurReader(const BYTE * pData, DWORD dwDataLen);

private:
	CDataBuf   m_recv_buf;

public:
	sigslot::signal1<PortStatus>                      m_sigStatus;
	sigslot::signal3<DWORD, DWORD, ReaderStatus>      m_sigReaderStatus;          // params: index, subindex, status
	sigslot::signal2<WORD, const TempItem &>          m_sigReaderTemp;            // �����¶�
	sigslot::signal1<const TempItem &>                m_sigHandReaderTemp;        // �ֳ��¶�
};
