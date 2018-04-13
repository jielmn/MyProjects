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

	// 重连Reader
	int   ReconnectReaderAsyn(DWORD dwDelayTime = 0);
	int   ReconnectReader();
	// 通知界面连接结果
	int   NotifyUiReaderStatus(CReader::READER_STATUS eStatus);
	// 获取状态
	CReader::READER_STATUS  GetReaderStatus();

	// 获取Tag上的温度数据
	int   GetTagTempAsyn();
	int   GetTagTemp();
	int   NotifyUiTagTemp();

private:
	static CBusiness *  pInstance;

	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:

	CReader       m_reader;
};




