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
	int    UpdateScrollAsyn(DWORD dwDelay = 0);
	int    UpdateScroll();

	// 报警
	int   AlarmAsyn(const char * szAlarmFile);
	int   Alarm(const CAlarmParam * pParam);

	// 重连发射器
	int   ReconnectLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReconnectLaunch();

	// 通知界面Launch状态
	int   NotifyUiLaunchStatus(CLmnSerialPort::PortStatus eStatus);

	// launch 读串口数据
	int   ReadLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReadLaunch();

	// 通知界面格子相关的Reader在线状态
	int   NotifyUiReaderStatus(DWORD dwGridIndex, int nStatus);

	// 通知界面温度数据
	int   NotifyUiTempData(DWORD dwGridIndex, DWORD  dwTemp);
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





