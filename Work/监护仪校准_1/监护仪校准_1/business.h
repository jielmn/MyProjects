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

	void  SetReaderModeAsyn(int nComPort, WORD wReaderAddr, WorkMode  eMode);
	void  SetReaderMode(const CWorkModeParam * pParam);

	void  SetReaderAsyn(int nComPort, WORD wReaderAddr, BYTE  byChannel);
	void  SetReader(const CSetReaderParam * pParam);

	void  SetReaderSnAsyn(int nComPort, DWORD dwSn);
	void  SetReaderSn(const CSetReaderSnParam * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }
};





