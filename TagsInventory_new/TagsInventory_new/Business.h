#pragma once

#include "InvCommon.h"
#include "InvDatabase.h"

class CBusiness : public sigslot::has_slots<>, public LmnToolkits::MessageHandler {

public:
	CBusiness();
	~CBusiness();

	// �������ݿ�(�첽��ֻ������Ϣ��db�߳�)
	int   ReconnectDatabaseAsyn( DWORD dwDelayTime = 0 );
	int   ReconnectDatabase();

	// ����Reader(�첽)
	int   ReconnectReaderAsyn();
	int   ReconnectReader();

	// �Ƿ��¼�ɹ�
	BOOL  IfLogined () const;

	sigslot::signal1< int *>                               sigInit;                
	sigslot::signal1< int *>                               sigDeinit;              
	
	static CBusiness *  GetInstance();

private:
	static CBusiness *  pInstance;

	void OnInit ( int * ret );
	void OnDeInit( int * ret );	

	void Clear();

	// ��Ϣ����
	void OnMessage( DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:

	BOOL           m_bLogin;                                // �Ƿ��¼
	CInvDatabase   m_InvDatabase;                           // ���ݿ�
};


