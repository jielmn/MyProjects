#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "LmnSerialPort.h"
#include "MyDatabase.h"

#include "sigslot.h"

class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();
	int Init();
	int DeInit();

	void  CheckDevicesAsyn();
	void  CheckDevices();

	void  StartAutoTestAsyn(const char * szMac);
	void  StartAutoTest(const CStartAutoTestParam * pParam);

	void  StopAutoTestAsyn();
	void  StopAutoTest();

	void  DisconnectBleAsyn();
	BOOL  DisconnectBle();

	void  ReadComDataAsyn();
	void  ReadComData();

	void  MeasureTempAsyn();
	void  MeasureTemp();

	void  SaveResultAsyn(const char * szReaderId, BOOL bPass, DWORD  dwFact);
	void  SaveResult(const CSaveResultParam * pParam);

	void  QueryDataAsyn(time_t t1, time_t t2, const char * szMac);
	void  QueryData(const CQueryDataParam * pParam);

private:
	static CBusiness *  pInstance;
	void Clear();
	void  PrintComData(void * pData, DWORD  dwDataLen);

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }


private:
	CLmnSerialPort           m_com;
	BOOL                     m_bBluetoothCnn;                    // 蓝牙是否连接上
	CMySqliteDatabase        m_db;
};





