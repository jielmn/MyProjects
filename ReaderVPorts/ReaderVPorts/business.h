#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "VPort.h"

class CBusiness : public LmnToolkits::MessageHandler {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();

	int  OpenCloseVPortAsyn(int nIndex);
	int  OpenCloseVPort(const COpenVPortParam * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();
	void  HandleVPort(const CHandleVPortParam * pParam);

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	CVPort     m_port[MAX_VPORT_COUNT];
};





