#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"

#include "TelemedReader.h"

class CBusiness : public LmnToolkits::MessageHandler {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();

	int Init();
	int DeInit();

	// ����Reader
	int   ReconnectReaderAsyn(DWORD dwDelayTime = 0);
	int   ReconnectReader();
	// ֪ͨ�������ӽ��
	int   NotifyUiReaderStatus(CTelemedReader::READER_STATUS eStatus);
	// ��ȡ״̬
	CTelemedReader::READER_STATUS  GetReaderStatus();

	// ��ȡTag�¶�
	int   ReadTagTempAsyn(DWORD dwDelayTime = 0);
	int   ReadTagTemp();
	int   NotifyUiReadTagTemp(int ret, DWORD dwTemp );

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

	CTelemedReader       m_reader;

	char                 m_szAlarmFile[256];
};




