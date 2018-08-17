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
	int    UpdateScrollAsyn(int nIndex,DWORD dwDelay = 0);
	int    UpdateScroll(const CUpdateScrollParam * pParam);

	// ����
	int   AlarmAsyn(const char * szAlarmFile);
	int   Alarm(const CAlarmParam * pParam);

	// ����������
	int   ReconnectLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReconnectLaunch();
	// Ӳ���Ķ������״̬
	int   CheckLaunchStatusAsyn();
	int   CheckLaunchStatus();

	// ֪ͨ����Launch״̬
	int   NotifyUiLaunchStatus(CLmnSerialPort::PortStatus eStatus);
	// ֪ͨ״̬��������Ϣ
	int   NotifyUiBarTips(int nIndex);

	// Reader����
	int   ReaderHeartBeatAsyn(DWORD dwGridIndex, DWORD dwDelayTime = 0);
	int   ReaderHeartBeat(const CReaderHeartBeatParam * pParam);

	// ��ȡ�¶�
	int   QueryTemperatureAsyn(DWORD dwGridIndex, DWORD dwDelayTime = 0);
	int   QueryTemperature(const CGetTemperatureParam * pParam);

	// launch ����������
	int   ReadLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReadLaunch();

	// ֪ͨ�����¶�����
	int   NotifyUiTempData(DWORD dwGridIndex, DWORD  dwTemp);

	// ֪ͨ���������ص�Reader����״̬
	int   NotifyUiGridReaderStatus(DWORD dwGridIndex, int nStatus);

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





