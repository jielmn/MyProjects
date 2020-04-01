#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "LmnSerialPort.h"

#include "sigslot.h"

class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();

	void  CheckDevicesAsyn();
	void  CheckDevices();

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }


private:
	CLmnSerialPort           m_com;
};





