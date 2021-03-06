#pragma once

#include "LmnSerialPort.h"
#include "sigslot.h"
#include "InvCommon.h"

typedef struct tagTokenTelemed {
	BYTE   abyUid[8];
}TokenTelemed;

class  CTelemedInvReader : public CLmnSerialPort {
public:
	CTelemedInvReader();
	~CTelemedInvReader();

	enum  ReaderError {
		ERROR_OK = 0,
		ERROR_DISCONNECTED,
		ERROR_FAIL = -1,
	};

	// 重连
	int  Reconnect();
	// 盘点
	int  Inventory();

	enum READER_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	READER_STATUS GetStatus() const;

public:
	sigslot::signal1<int>                              m_sigStatusChange;         // 状态改变
	sigslot::signal1<DWORD >                           m_sigReconnect;            // 重连 
	sigslot::signal2<const BYTE *, DWORD>              m_sigInvTagIetm;           // 盘点到一个Tag
	sigslot::signal0<>                                 m_sigInventoryAsyn;        // 盘点

private:
	void  Clear();
	// 尽可能的读数据，直到dwIntervalTime时间后，不再收到数据。或者到达一定数量的数据
	void  ReceiveAsPossible(DWORD  dwIntervalTime, DWORD dwMaxDataLength);

	// 协议部分
	int   RequestPrepare();
	int   ReadPrepareRet();
	int   ReadToken(TokenTelemed & t);

	int   RequestInventory();
	int   ReadInventoryRet();
	// END 协议部分

private:
	READER_STATUS      m_eStatus;
	CDataBuf           m_received_data;                // 从串口接收数据的缓冲区	
};




