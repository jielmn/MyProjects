#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "sigslot.h"
#include "launcher.h"

class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();

	int  ReconnectLauncherAsyn(DWORD  dwDelay = 0);
	int  ReconnectLauncher();

	int  PrintStatusAsyn(DWORD  dwDelay = 0);
	int  PrintStatus();

	int  GetStationDataAsyn(DWORD  dwDelay = 0);
	int  GetStationData();

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	void  OnLaunchStatus(CLmnSerialPort::PortStatus s);
	void  OnReconnectLaunch(DWORD dwDelay);

private:
	CLaunch     m_launch;
};





