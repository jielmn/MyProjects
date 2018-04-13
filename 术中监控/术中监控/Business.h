#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"

#include "Reader.h"

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
	int   NotifyUiReaderStatus(CReader::READER_STATUS eStatus);
	// ��ȡ״̬
	CReader::READER_STATUS  GetReaderStatus();

	// ��ȡTag�ϵ��¶�����
	int   GetTagTempAsyn();
	int   GetTagTemp();
	int   NotifyUiTagTemp();

private:
	static CBusiness *  pInstance;

	void Clear();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:

	CReader       m_reader;
};




