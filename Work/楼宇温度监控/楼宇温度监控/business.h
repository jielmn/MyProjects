#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "MyDb.h"
#include "sigslot.h"

class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();

	void  AddFloorAsyn(HWND hWnd, int nFloor);
	void  AddFloor(const CAddFloorParam * pParam);

	void  GetAllFloorsAsyn();
	void  GetAllFloors();

	void  GetDevicesByFloorAsyn(int nFloor);
	void  GetDevicesByFloor(const CGetDevicesParam * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	CMySqliteDatabase              m_db;
};





