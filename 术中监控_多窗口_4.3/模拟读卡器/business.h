#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "sigslot.h"
#include "LmnSerialPort.h"

class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();

	void  ConnectComPortAsyn();
	void  ConnectComPort();

	void  ReadDataAsyn(DWORD  dwDelay = 0);
	void  ReadData();

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	CLmnSerialPort          m_serial_port;
	CDataBuf                m_buf;
	DWORD                   m_arrTemp[MAX_GRID * MAX_READERS_PER_GRID];
};





