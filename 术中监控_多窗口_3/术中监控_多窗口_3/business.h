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

	// ����������
	int    UpdateScrollAsyn(int nIndex, DWORD dwDelay = 0);
	int    UpdateScroll(const CUpdateScrollParam * pParam);
	void   OnUpdateScroll(DWORD dwIndex);

	// ����
	int   AlarmAsyn();
	int   Alarm();
	void  OnAlarm();

	// ����������
	int   ReconnectLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReconnectLaunch();
	void  OnReconnect(DWORD dwDelay);
	void  OnStatus(CLmnSerialPort::PortStatus e);

private:
	static CBusiness *  pInstance;
	void Clear();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	char     m_szAlarmFile[256];
	CLaunch  m_launch;
};





