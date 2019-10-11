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

	void  OpenComPortAsyn(int nComPort);
	void  OpenComPort(const COpenComParam * pParam);

	void  CloseComPortAsyn();
	void  CloseComPort();

	void  WriteComDataAsyn(const BYTE  * pData, DWORD  dwDataLen);
	void  WriteComData(const CWriteComParam * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	CLmnSerialPort                   m_serial_port;
};





