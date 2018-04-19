#pragma once

#include "InvCommon.h"
#include "InvDatabase.h"
//#include "InvReader.h"
#include "610InvReader.h"

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
	void  OnReaderStatusChange(C601InvReader::READER_STATUS);
	void  OnReaderReconnectAsyn( DWORD dwRelayTime );
	void  OnInvTagItem(const BYTE *, DWORD);
	void  OnInventoryAsyn();

	// �̵�
	int   InventoryAsyn();
	int   Inventory();

	// ֪ͨ����DB Status
	int   NotifyUiDbStatus(CInvDatabase::DATABASE_STATUS eStatus);
	int   NotifyUiReaderStatus(C601InvReader::READER_STATUS eStatus);

	CInvDatabase::DATABASE_STATUS  GetDbStatus();
	C601InvReader::READER_STATUS      GetReaderStatus();

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

	// check
	int   CheckTagAsyn(const TagItem * pItem);
	int   CheckTag(const CTagItemParam * pItem );
	int   NotifyUiCheckTagRet( int nRet, const CTagItemParam * pItem );

	// ���ö�ʱ��
	int   SetTimer( DWORD dwTimerId, DWORD dwDelayTime );
	int   NotifyUiTimer(DWORD dwTimerId);

	// ������̵�
	int   InvBigSaveAsyn(const CString & strBatchId, const std::vector<CString *> & v);
	int   InvBigSave(const CInvBigSaveParam * pParam);
	int   NotifyUiInvBigSaveRet(int nError, const CString & strBatchId, const CString & strWrongSmallPkgId );

	// ��ѯ
	int   QueryAsyn(time_t tStart, time_t tEnd, const char * szBatchId, const char * szOperator, int nQueryType );
	int   Query( const CQueryParam * pParam );
	int   NotifyUiQueryRet(int nError,int nQueryType, std::vector<QueryResultItem *> * pvRet);

	// ��ѯС��װ��Tags
	int   QuerySmallAsyn(DWORD dwId);
	int   QuerySmall(const CQuerySmallParam * pParam);
	int   NotifyUiQuerySmallRet(int nError, std::vector<QuerySmallResultItem *> * pvRet);

	// ��ѯ���װ��Tags
	int   QueryBigAsyn(DWORD dwId);
	int   QueryBig(const CQueryBigParam * pParam);
	int   NotifyUiQueryBigRet(int nError, std::vector<QueryResultItem *> * pvRet);

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
	C601InvReader    m_InvReader;
};


