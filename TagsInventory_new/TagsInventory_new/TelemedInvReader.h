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

	// ����
	int  Reconnect();
	// �̵�
	int  Inventory();

	enum READER_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	READER_STATUS GetStatus() const;

public:
	sigslot::signal1<int>                              m_sigStatusChange;         // ״̬�ı�
	sigslot::signal1<DWORD >                           m_sigReconnect;            // ���� 
	sigslot::signal2<const BYTE *, DWORD>              m_sigInvTagIetm;           // �̵㵽һ��Tag
	sigslot::signal0<>                                 m_sigInventoryAsyn;        // �̵�

private:
	void  Clear();
	// �����ܵĶ����ݣ�ֱ��dwIntervalTimeʱ��󣬲����յ����ݡ����ߵ���һ������������
	void  ReceiveAsPossible(DWORD  dwIntervalTime, DWORD dwMaxDataLength);

	// Э�鲿��
	int   RequestPrepare();
	int   ReadPrepareRet();
	int   ReadToken(TokenTelemed & t);

	int   RequestInventory();
	int   ReadInventoryRet();
	// END Э�鲿��

private:
	READER_STATUS      m_eStatus;
	CDataBuf           m_received_data;                // �Ӵ��ڽ������ݵĻ�����	
};




