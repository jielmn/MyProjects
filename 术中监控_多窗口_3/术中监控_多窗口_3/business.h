#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "sigslot.h"
#include "Launch.h"

class  CMyDb {
public:
	CMyDb();
	~CMyDb();

	int Reconnect();

	sigslot::signal1<int>     m_sigStatus;

private:
	MYSQL   m_mysql;
	int     m_nStatus;    //0: �ر�;    1: ��
};

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

	// ��ӡ״̬
	int   PrintStatusAsyn();
	int   PrintStatus();

	// ��ȡ�¶�
	int   GetGridTemperatureAsyn(DWORD  dwIndex, DWORD dwDelay = 0);
	int   GetGridTemperature(const CGridTempParam * pParam);
	int   GetTemperatureAsyn(DWORD  dwIndex, DWORD dwSubIndex, DWORD dwDelay = 0);
	int   GetTemperature(CTemperatureParam * pParam);	

	// launch ����������
	int   ReadLaunchAsyn(DWORD dwDelayTime = 0);
	int   ReadLaunch();
	void  OnReaderTemp(DWORD dwIndex, DWORD dwSubIndex, const LastTemp & t);
	void  OnCheckReader();

	// Ӳ���Ķ������״̬
	int   CheckLaunchStatusAsyn();
	int   CheckLaunchStatus();

	// �������ݿ�
	int   ReconnectDbAsyn(DWORD dwDelay = 0);
	int   ReconnectDb();
	// 0: �ر�;  1: ��
	void  OnDbStatus(int nDbStatus);

private:
	static CBusiness *  pInstance;
	void Clear();
	void  CheckGrid(DWORD dwIndex);

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	char              m_szAlarmFile[256];
	CLaunch           m_launch;
	ReaderStatus      m_reader_status[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
	CMyDb             m_db;
};





