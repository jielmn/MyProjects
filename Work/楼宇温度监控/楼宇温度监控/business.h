#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "common.h"
#include "MyDb.h"
#include "sigslot.h"

class CMyComPort {
public:
	CMyComPort() {
		m_last_cmd_time = 0;
	}
	CLmnSerialPort     m_com;
	CDataBuf           m_buf;
	time_t             m_last_cmd_time;
};

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

	void  AddDeviceAsyn(HWND hWnd, int nFloor, int nDeviceId, const char * szDeviceAddr);
	void  AddDevice(const CAddDeviceParam * pParam);

	void  DeleteDeviceAsyn(int nDeviceId);
	void  DeleteDevice(const CDelDeviceParam * pParam);

	void  DeleteFloorAsyn(int nFloor);
	void  DeleteFloor(const CDelFloorParam * pParam);

	// 硬件改动，检查接收器串口状态
	void   CheckLaunchAsyn();
	void   CheckLaunch();

	// 读取所有的串口数据
	void   ReadAllComPorts();
	BOOL   ProcHandeReader(CLmnSerialPort * pCom, const BYTE * pData, DWORD dwDataLen, TempItem & item);
	void   ProcTail(CDataBuf * pBuf);

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	CMySqliteDatabase              m_db;
	std::vector<CMyComPort *>      m_vSerialPorts;
};





