#pragma once

#include "InvCommon.h"
#include "InvDatabase.h"
#include "InvReader.h"

class CBusiness : public sigslot::has_slots<>, public LmnToolkits::MessageHandler {

public:
	CBusiness();
	~CBusiness();

	// �������ݿ�(�첽��ֻ������Ϣ��db�߳�)
	int   ReconnectDatabaseAsyn( DWORD dwDelayTime = 0 );
	int   ReconnectDatabase( );

	// ����Reader(�첽)
	int   ReconnectReaderAsyn(DWORD dwDelayTime = 0 );
	int   ReconnectReader();

	// �̵�
	int   InventoryAsyn();
	int   Inventory();

	// ֪ͨ����DB Status
	int   NotifyUiDbStatus(CInvDatabase::DATABASE_STATUS eStatus);
	int   NotifyUiReaderStatus(CInvReader::READER_STATUS eStatus);

	CInvDatabase::DATABASE_STATUS  GetDbStatus();
	CInvReader::READER_STATUS      GetReaderStatus();

	// ֪ͨ�����̵��item
	int   NotifyUiInventory(const TagItem * pItem);

	// ֪ͨ�����¼���
	int   NotifyUiLoginRet();

	// �Ƿ��¼�ɹ�
	BOOL  IfLogined () const;
	const char * GetUserName();
	BOOL  IfUserId( const TagItem * pItem ) const;

	// ���̵��Tag ID��Ϣ��¼
	int   LoginUserAsyn(const TagItem * pItem);
	int   LoginUser(const CTagItemParam * pItem);

	// ����С�̵�
	int   InvSmallSaveAsyn( const CString & strBatchId,  const std::vector<TagItem *> & v );
	int   InvSmallSave( const CInvSmallSaveParam * pParam);
	int   NotifyUiInvSmallSaveRet( int nError, const CString & strBatchId );

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
	User           m_user;                                  // ��¼���û�
	CInvDatabase   m_InvDatabase;                           // ���ݿ�
	CInvReader     m_InvReader;
};


