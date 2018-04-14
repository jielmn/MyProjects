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

	// 重连Reader
	int   ReconnectReaderAsyn(DWORD dwDelayTime = 0);
	int   ReconnectReader();
	// 通知界面连接结果
	int   NotifyUiReaderStatus(CTelemedReader::READER_STATUS eStatus);
	// 获取状态
	CTelemedReader::READER_STATUS  GetReaderStatus();

	// 读取Tag温度
	int   ReadTagTempAsyn(DWORD dwDelayTime = 0);
	int   ReadTagTemp();
	int   NotifyUiReadTagTemp(int ret, DWORD dwTemp );

	// 报警
	int   AlarmAsyn(const char * szAlarmFile);
	int   Alarm(const CAlarmParam * pParam);

private:
	static CBusiness *  pInstance;

	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:

	CTelemedReader       m_reader;

	char                 m_szAlarmFile[256];
};




