#pragma once

#include "LmnCommon.h"
#include "LmnThread.h"
#include "sigslot.h"
#include "ReaderToolCommon.h"
#include "TelemedReader.h"

class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();

	int Init();
	int DeInit();

	// ����Reader
	int  ReconnectReaderAsyn(DWORD  dwRelayTime = 0);
	int  ReconnectReader();
	void OnReaderStatusChange(CTelemedReader::TELEMED_READER_STATUS eNewStatus);

	// �������
	int  ReaderHeatbeatAsyn(DWORD  dwRelayTime = 0);
	int  ReaderHeatbeat();

	// ����Reader Id
	int  SetReaderIdAsyn(int nId, int nVersion);
	int  SetReaderId( const CReaderIdParam * pParam );

	// ����Reader time
	int  SetReaderTimeAsyn(time_t tTime);
	int  SetReaderTime(const CReaderTimeParam * pParam);

	// ����Reader mode
	int  SetReaderModeAsyn(int nMode);
	int  SetReaderMode(const CReaderModeParam * pParam);

	// ���Reader
	int  ClearReaderAsyn();
	int  ClearReader();

	// ��ȡReader����
	int  GetReaderDataAsyn();
	int  GetReaderData();

	// ��������
	int SetReaderBluetoothAsyn( BOOL bEnable );
	int SetReaderBluetooth(const CReaderBlueToothParam * pParam);

	// ������������
	int SetReaderBluetoothNameAsyn(const char * szBlueToothName);
	int SetReaderBluetoothName(const CReaderBlueToothNameParam * pParam);

public:
	sigslot::signal1<CTelemedReader::TELEMED_READER_STATUS>            m_sigReaderStatusChange;
	sigslot::signal1<int>                                              m_sigSetReaderIdRet;
	sigslot::signal1<int>                                              m_sigSetReaderTimeRet;
	sigslot::signal1<int>                                              m_sigSetReaderModeRet;
	sigslot::signal1<int>                                              m_sigClearReaderRet;
	sigslot::signal2<int, const std::vector<TempItem* > &>             m_sigGetReaderDataRet;
	sigslot::signal1<int>                                              m_sigSetReaderBlueToothRet;
	sigslot::signal1<int>                                              m_sigSetReaderBlueToothNameRet;

private:
	static CBusiness *  pInstance;

	void Clear();

	// ��Ϣ����
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	LmnToolkits::Thread   m_thrd_reader;

	CTelemedReader        m_Reader;
};

