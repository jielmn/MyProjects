#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "sigslot.h"
#include "LmnSerialPort.h"
#include "MyDatabase.h"

class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();

	void  ReconnectAsyn(BOOL bCloseFirst = FALSE);
	void  Reconnect();

	void  ReadDataAsyn(DWORD  dwDelay = 0);
	void  ReadData();

	void  GetDeviceUserNameAsyn(const char * szDeviceId);
	void  GetDeviceUserName(const CGetNameParam * pParam);

	void  SaveDeviceUserNameAsyn(const char * szDeviceId, const char * szName);
	void  SaveDeviceUserName(const CSaveNameParam * pParam);

	void  SaveItemAsyn(CWearItem * pItem);
	void  SaveItem(const CSaveItemParam * pParam);

	void  GetDataAsyn(const char *szDeviceId);
	void  GetData(const CGetDataParam * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	CLmnSerialPort     m_com;
	CDataBuf           m_buf;
	CMySqliteDatabase  m_sqlite;
};





