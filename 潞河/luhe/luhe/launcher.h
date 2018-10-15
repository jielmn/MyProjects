#pragma once

#include "common.h"
#include "LmnSerialPort.h"
#include "sigslot.h"

class  CLaunch : public CLmnSerialPort {
public:
	CLaunch();
	~CLaunch();

	// ÖØÁ¬
	int  Reconnect();

public:
	sigslot::signal1<CLmnSerialPort::PortStatus>             m_sigStatus;
	sigslot::signal1<DWORD>                                  m_sigReconnect;

private:
	void  CloseLaunch();

private:
	CDataBuf   m_recv_buf;
};