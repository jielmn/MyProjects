#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "sigslot.h"
#include "MyDatabase.h"
#include "Launch.h"

class CDuiFrameWnd;
class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();
	void InitSigslot(CDuiFrameWnd * pMainWnd);

	// 打印状态(调试用)
	void   PrintStatusAsyn();
	void   PrintStatus();

	// 硬件改动，检查接收器串口状态
	void   CheckLaunchAsyn();
	void   CheckLaunch();

	// 重新连接接收器(因配置改动，grid count变化)
	void   RestartLaunchAsyn();
	void   RestartLaunch();

	// 获取温度
	void   GetGridTemperatureAsyn(DWORD  dwIndex, DWORD dwDelay = 0);
	void   GetGridTemperature(const CGetGridTempParam * pParam);
	void   GetGridTemperature(DWORD dwIndex, DWORD dwSubIndex, BYTE byArea);

	// 发射器相关的回调
	void  OnStatus(CLmnSerialPort::PortStatus e);

private:
	static CBusiness *  pInstance;
	void Clear();
	// 窗格顺序配置
	void GetGridsOrderCfg();
	// 病区配置
	void GetAreaCfg();
	// 床位的配置
	void GetGridsCfg();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	CMySqliteDatabase             m_sqlite;
	CLaunch                       m_launch;
	sigslot::signal1<CLmnSerialPort::PortStatus>    m_sigLanchStatus;
	sigslot::signal2<DWORD,BOOL>                    m_sigTrySurReader;         // param: bed no, is try read
	// 术中读卡器是否得到温度数据
	BOOL                          m_bSurReaderTemp[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
};





