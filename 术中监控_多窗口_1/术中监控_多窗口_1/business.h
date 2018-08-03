#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "TelemedReader.h"

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

	// ����Reader
	int   ReconnectReaderAsyn(int nIndex, DWORD dwDelayTime = 0);
	int   ReconnectReader(const CReconnectReaderParam * pParam);

	// ֪ͨ�������ӽ��
	int   NotifyUiReaderStatus(int nIndex, CTelemedReader::READER_STATUS eStatus);
	// ��ȡ״̬
	CTelemedReader::READER_STATUS  GetReaderStatus(int nIndex);
	// ��ȡ�˿ں�
	const char * GetReaderComPort(int nIndex);

	// ��ȡTag�¶�
	int   ReadTagTempAsyn(int nIndex, DWORD dwDelayTime = 0);
	int   ReadTagTemp(const CReadTempParam * pParam);
	int   NotifyUiReadTagTemp(int nIndex, int ret, DWORD dwTemp);

private:
	static CBusiness *  pInstance;
	void Clear();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	char    m_szAlarmFile[MAX_ALARM_PATH_LENGTH];
	CTelemedReader       m_reader[MAX_GRID_COUNT];
};





