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

	// 硬件改动，检查接收器串口状态
	void   CheckLaunchAsyn();
	void   CheckLaunch();

	// 读取所有的串口数据
	void   ReadAllComPorts();
	// 处理手持读卡器数据
	BOOL   ProcHandeReader(CLmnSerialPort * pCom, const BYTE * pData, DWORD dwDataLen, TempItem & item);
	// 清除结尾可能存在的"dd aa"
	void   ProcTail(CDataBuf * pBuf);

private:
	static CBusiness *  pInstance;
	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	CMySqliteDatabase           m_db;
	std::vector<CMyComPort *>   m_vSerialPorts;
};





