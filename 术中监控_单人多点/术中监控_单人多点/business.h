#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "Launch.h"
#include "sigslot.h"

class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

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

	// Reader����
	int   ReaderHeartBeatAsyn(DWORD dwGridIndex, DWORD dwDelayTime = 0);
	int   ReaderHeartBeat(const CReaderHeartBeatParam * pParam);

	// ��ȡ�¶�
	int   QueryTemperatureAsyn(DWORD dwGridIndex, DWORD dwDelayTime = 0);
	int   QueryTemperature(const CGetTemperatureParam * pParam);

	// ��ȡһ���е��¶�
	int   QueryRoundTemperatureAsyn(DWORD dwDelayTime = 0);
	int   QueryRoundTemperature();

	// Ӳ���Ķ������״̬
	int   CheckLaunchStatusAsyn();
	int   CheckLaunchStatus();

	void  OnLaunchError();
	void  OnHeartBeatRet(DWORD  dwIndex, int nRet);
	void  OnTempRet(DWORD dwIndex, DWORD dwTemp);
private:
	static CBusiness *  pInstance;
	void Clear();
	BOOL  IsGetAllQueryRet();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	char     m_szAlarmFile[MAX_ALARM_PATH_LENGTH];
	CLaunch  m_launch;

	BOOL     m_bFirstHeartBeats;              // ��һ��������������ѯ��������¶�
	BOOL     m_bQueryRet[MAX_READERS_COUNT];  // �����Ĳ�ѯ������
};





