#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "Launch.h"

class CBusiness : public LmnToolkits::MessageHandler {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();

	// 调整滑动条
	int    UpdateScrollAsyn(int nIndex,DWORD dwDelay = 0);
	int    UpdateScroll(const CUpdateScrollParam * pParam);

	// 报警
	int   AlarmAsyn(const char * szAlarmFile);
	int   Alarm(const CAlarmParam * pParam);

	// 重连发射器
	int   ReconnectLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReconnectLaunch();
	// 硬件改动，检查状态
	int   CheckLaunchStatusAsyn();
	int   CheckLaunchStatus();

	// 通知界面Launch状态
	int   NotifyUiLaunchStatus(CLmnSerialPort::PortStatus eStatus);
	// 通知状态栏其他信息
	int   NotifyUiBarTips(int nIndex);

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	char     m_szAlarmFile[MAX_ALARM_PATH_LENGTH];
	CLaunch  m_launch;
};





