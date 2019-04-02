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

	int  SetReceriverChannelAsyn(BYTE byChannelA, BYTE byChannelB, BYTE byChannelC, int nCom);
	int  SetReceriverChannel(const CSetReceiverChannelParam * pParam);

	int  SetSurgencyReaderAsyn(WORD wAddr, BYTE byChannel, int nCom);
	int  SetSurgencyReader(const CSetSurgencyReaderParam * pParam);

	int  SetReceriverAreaAsyn(BYTE byChannel,int nCom);
	int  SetReceriverArea(const CSetReceiverAreaParam * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }
};





