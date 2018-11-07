#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "sigslot.h"

class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();

	int  SettingGwAsyn(int nAreaNo, int nComPort);
	int  SettingGw(const CSettingGwParam * pParam);

	int  SettingReaderAsyn(int nAreaNo, int nBedNo, int nComPort);
	int  SettingReader(const CSettingReaderParam * pParam);

	int  SettingSnAsyn(int nSn, int nComPort);
	int  SettingSn(const CSettingSnParam * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }
};





