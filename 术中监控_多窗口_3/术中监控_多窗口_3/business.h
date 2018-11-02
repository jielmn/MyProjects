#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "sigslot.h"
#include "Launch.h"

class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();

	// 调整滑动条
	int    UpdateScrollAsyn(int nIndex, DWORD dwDelay = 0);
	int    UpdateScroll(const CUpdateScrollParam * pParam);
	void   OnUpdateScroll(DWORD dwIndex);

	// 报警
	int   AlarmAsyn();
	int   Alarm();
	void  OnAlarm();

	// 重连发射器
	int   ReconnectLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReconnectLaunch();
	void  OnReconnect(DWORD dwDelay);
	void  OnStatus(CLmnSerialPort::PortStatus e);

	// 打印状态
	int   PrintStatusAsyn();
	int   PrintStatus();

	// 获取温度
	int   GetGridTemperatureAsyn(DWORD  dwIndex, DWORD dwDelay = 0);
	int   GetGridTemperature(const CGridTempParam * pParam);
	int   GetTemperatureAsyn(DWORD  dwIndex, DWORD dwSubIndex, DWORD dwDelay = 0);
	int   GetTemperature(CTemperatureParam * pParam);	

	// launch 读串口数据
	int   ReadLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReadLaunch();
	void  OnReaderTemp(DWORD dwIndex, DWORD dwSubIndex, DWORD dwTemp);
	void  OnCheckReader();

private:
	static CBusiness *  pInstance;
	void Clear();
	void  CheckGrid(DWORD dwIndex);

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	char              m_szAlarmFile[256];
	CLaunch           m_launch;
	ReaderStatus      m_reader_status[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
};





