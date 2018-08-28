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

	// ����������
	int    UpdateScrollAsyn(DWORD dwDelay = 0);
	int    UpdateScroll();

	// ����
	int   AlarmAsyn(const char * szAlarmFile);
	int   Alarm(const CAlarmParam * pParam);

	// ����������
	int   ReconnectLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReconnectLaunch();

	// ֪ͨ����Launch״̬
	int   NotifyUiLaunchStatus(CLmnSerialPort::PortStatus eStatus);

	// launch ����������
	int   ReadLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReadLaunch();

	// ֪ͨ���������ص�Reader����״̬
	int   NotifyUiReaderStatus(DWORD dwGridIndex, int nStatus);

	// ֪ͨ�����¶�����
	int   NotifyUiTempData(DWORD dwGridIndex, DWORD  dwTemp);
private:
	static CBusiness *  pInstance;
	void Clear();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	char     m_szAlarmFile[MAX_ALARM_PATH_LENGTH];
	CLaunch  m_launch;
};





