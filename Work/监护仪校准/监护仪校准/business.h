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

	void  AdjustAsyn(int nComPort, int nTemp, int nDutyCycle);
	void  Adjust(const CAdjustParam * pParam);

	void  AdjustAllAsyn(int nComPort, MachineType eType, TempAdjust * items, DWORD dwSize);
	void  AdjustAll(const CAdjustAllParam * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }
};





