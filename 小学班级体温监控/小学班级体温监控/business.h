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

	void   AddClassAsyn(HWND hWnd, DWORD dwGrade, DWORD  dwClass);
	void   AddClass(const CAddClassParam * pParam);

	void   GetAllClassesAsyn();
	void   GetAllClasses();

	void   GetRoomDataAsyn(DWORD dwNo);
	void   GetRoomData(const CGetRoomParam * pParam);

	void   ModifyDeskAsyn(DWORD dwClassNo, DWORD dwPos, const char * szName, int nSex);
	void   ModifyDesk(const CModifyDeskParam * pParam);

	void   EmptyDeskAsyn(DWORD dwClassNo, DWORD dwPos);
	void   EmptyDesk(const CEmptyDeskParam * pParam);

	void   DeleteClassAsyn(DWORD  dwClassNo);
	void   DeleteClass(const CDeleteClassParam * pParam);

	void   ExchangeDeskAsyn(DWORD  dwClassNo, DWORD  dwPos1, DWORD  dwPos2);
	void   ExchangeDesk(const CExchangeDeskParam * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	CMySqliteDatabase     m_db;
};





