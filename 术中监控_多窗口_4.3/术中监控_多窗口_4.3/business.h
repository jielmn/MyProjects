#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "sigslot.h"

class CDuiFrameWnd;
class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();
	void InitSigslot(CDuiFrameWnd * pMainWnd);

private:
	static CBusiness *  pInstance;
	void Clear();
	void GetGridsOrderCfg();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }
};





