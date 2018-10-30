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

private:
	void   CloseLaunch();

private:
	CDataBuf   m_recv_buf;

public:
	sigslot::signal1<PortStatus>        m_sigStatus;
	sigslot::signal1<DWORD>             m_sigReconnect;
};
