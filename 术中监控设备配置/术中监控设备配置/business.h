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

	int  SettingReaderAsyn( int nAreaNo, int nBedNo, int nComPort );
	int  SettingReader(const CSettingReaderParam * pParam);

	int  SettingGwAsyn(int nAreaNo, int nComPort);
	int  SettingGw(const CSettingGwParam * pParam);

	int  QueryGwAsyn(int nComPort);
	int  QueryGw(const CQueryGwParam * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }
};





