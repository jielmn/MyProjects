#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"

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

private:
	static CBusiness *  pInstance;
	void Clear();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	char    m_szAlarmFile[MAX_ALARM_PATH_LENGTH];
};





