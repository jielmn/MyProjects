#pragma once

#include "LmnThread.h"
#include "Business.h"

class  CBindingReader {
public:
	CBindingReader();
	~CBindingReader();

	int Init();
	int Inventory(TagId * pId);
	void NotifyInventory( int nResult, const TagId * pId);

	static CBindingReader *  GetInstance();

private:
	static CBindingReader *  pInstance;
};

extern LmnToolkits::Thread  * g_thrd_binding_reader;


class MessageHandlerBindingReader : public LmnToolkits::MessageHandler {
public:
	MessageHandlerBindingReader();
	~MessageHandlerBindingReader();
	BOOL CanBeFreed() { return false; }
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
};

extern MessageHandlerBindingReader * g_handler_binding_reader;

#define  MSG_INVENTORY              1
#define  DELAY_INVENTORY_TIME       1000