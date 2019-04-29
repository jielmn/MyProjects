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

	// ��ӡ״̬(������)
	void   PrintStatusAsyn();
	void   PrintStatus();

	// Ӳ���Ķ���������������״̬
	void   CheckLaunchAsyn();
	void   CheckLaunch();

	// �������ӽ�����(�����øĶ���grid count�仯)
	void   RestartLaunchAsyn();
	void   RestartLaunch();

	// ��ȡ�¶�
	void   GetGridTemperatureAsyn(DWORD  dwIndex, DWORD dwDelay = 0);
	void   GetGridTemperature(const CGetGridTempParam * pParam);
	void   GetGridTemperature(DWORD dwIndex, DWORD dwSubIndex, BYTE byArea);

	// ��������صĻص�
	void  OnStatus(CLmnSerialPort::PortStatus e);

private:
	static CBusiness *  pInstance;
	void Clear();
	// ����˳������
	void GetGridsOrderCfg();
	// ��������
	void GetAreaCfg();
	// ��λ������
	void GetGridsCfg();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	CMySqliteDatabase             m_sqlite;
	CLaunch                       m_launch;
	sigslot::signal1<CLmnSerialPort::PortStatus>    m_sigLanchStatus;
	sigslot::signal2<DWORD,BOOL>                    m_sigTrySurReader;         // param: bed no, is try read
	// ���ж������Ƿ�õ��¶�����
	BOOL                          m_bSurReaderTemp[MAX_GRID_COUNT][MAX_READERS_PER_GRID];
};





