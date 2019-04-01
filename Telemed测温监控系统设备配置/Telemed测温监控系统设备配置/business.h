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

	int  SetHandReaderAsyn(BYTE byChannel, BYTE byAddr, int nCom);
	int  SetHandReader(const CSettingHandReaderParam * pParam);

	int  SetHandReaderSnAsyn(DWORD  dwSn, int nCom);
	int  SetHandReaderSn(const CSetHandReaderSnParam * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }
};





