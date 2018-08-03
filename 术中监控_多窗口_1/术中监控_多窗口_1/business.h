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

	// 调整滑动条
	int    UpdateScrollAsyn(int nIndex,DWORD dwDelay = 0);
	int    UpdateScroll(const CUpdateScrollParam * pParam);

	// 报警
	int   AlarmAsyn(const char * szAlarmFile);
	int   Alarm(const CAlarmParam * pParam);

	// 重连Reader
	int   ReconnectReaderAsyn(int nIndex, DWORD dwDelayTime = 0);
	int   ReconnectReader(const CReconnectReaderParam * pParam);

	// 通知界面连接结果
	int   NotifyUiReaderStatus(int nIndex, CTelemedReader::READER_STATUS eStatus);
	// 获取状态
	CTelemedReader::READER_STATUS  GetReaderStatus(int nIndex);
	// 获取端口号
	const char * GetReaderComPort(int nIndex);

	// 读取Tag温度
	int   ReadTagTempAsyn(int nIndex, DWORD dwDelayTime = 0);
	int   ReadTagTemp(const CReadTempParam * pParam);
	int   NotifyUiReadTagTemp(int nIndex, int ret, DWORD dwTemp);

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	char    m_szAlarmFile[MAX_ALARM_PATH_LENGTH];
	CTelemedReader       m_reader[MAX_GRID_COUNT];
};





