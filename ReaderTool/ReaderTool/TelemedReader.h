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

	sigslot::signal1<TELEMED_READER_STATUS>            m_sigStatusChange;         // ״̬�ı�

public:
	// ����
	int  Reconnect();
	// �������
	int  Heartbeat();
	// ����Id
	int  SetReaderId(const CReaderIdParam * pParam);
	// ����Time
	int  SetReaderTime(const CReaderTimeParam * pParam);
	// ����Mode
	int  SetReaderMode(const CReaderModeParam * pParam);
	// ���Reader
	int ClearReader();
	// ��ȡReader���� 
	int GetReaderData( std::vector<TempItem* > & vRet );
	//
	int SetReaderBluetooth(const CReaderBlueToothParam * pParam);

private:
	TELEMED_READER_STATUS     m_eStatus;
	CDataBuf                  m_received_data;                // �Ӵ��ڽ������ݵĻ�����	

	ReaderCmd                 PREPARE_COMMAND;
	ReaderCmd                 HEARTBEAT_COMMAND;

private:
	void  Clear();

	// Э�鲿��
	int   RequestPrepare();
	int   ReadPrepareRet();
	int   RequestHeartbeat();
	int   ReadHeartbeatRet();

	void  ReceiveAsPossible(DWORD  dwWaitTime, DWORD dwMaxDataLength = 0);

	void  FillTempItem(TempItem * pItem, const BYTE * pData, DWORD dwDataLen);
};