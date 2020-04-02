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

	void  StartAutoTestAsyn(const char * szMac);
	void  StartAutoTest(const CStartAutoTestParam * pParam);

	void  StopAutoTestAsyn();
	void  StopAutoTest();

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }


private:
	CLmnSerialPort           m_com;
	BOOL                     m_bBluetoothCnn;                    // 蓝牙是否连接上
};





