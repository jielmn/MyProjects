#pragma once

#include <vector>
#include "LmnCommon.h"
#include "LmnSerialPort.h"
#include "sigslot.h"
#include "ReaderToolCommon.h"

class CTelemedReader : public CLmnSerialPort {
public:
	enum  ReaderError {
		ERROR_OK = 0,
		ERROR_DISCONNECTED,
		ERROR_FAILED_TO_RECEIVE_OR_WRONG_FORMAT,
		ERROR_FAIL = -1,
	};

	enum TELEMED_READER_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	typedef struct tagReaderCmd {
		BYTE      abyCommand[256];
		DWORD     dwCommandLength;
	}ReaderCmd;

	CTelemedReader();
	TELEMED_READER_STATUS GetStatus() const;

	sigslot::signal1<TELEMED_READER_STATUS>            m_sigStatusChange;         // 状态改变

public:
	// 重连
	int  Reconnect();
	// 心跳检查
	int  Heartbeat();
	// 设置Id
	int  SetReaderId(const CReaderIdParam * pParam);
	// 设置Time
	int  SetReaderTime(const CReaderTimeParam * pParam);
	// 设置Mode
	int  SetReaderMode(const CReaderModeParam * pParam);
	// 清空Reader
	int ClearReader();
	// 获取Reader数据 
	int GetReaderData( std::vector<TempItem* > & vRet );
	//
	int SetReaderBluetooth(const CReaderBlueToothParam * pParam);

private:
	TELEMED_READER_STATUS     m_eStatus;
	CDataBuf                  m_received_data;                // 从串口接收数据的缓冲区	

	ReaderCmd                 PREPARE_COMMAND;
	ReaderCmd                 HEARTBEAT_COMMAND;

private:
	void  Clear();

	// 协议部分
	int   RequestPrepare();
	int   ReadPrepareRet();
	int   RequestHeartbeat();
	int   ReadHeartbeatRet();

	void  ReceiveAsPossible(DWORD  dwWaitTime, DWORD dwMaxDataLength = 0);

	void  FillTempItem(TempItem * pItem, const BYTE * pData, DWORD dwDataLen);
};