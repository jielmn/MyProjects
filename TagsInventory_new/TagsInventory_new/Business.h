#pragma once

#include "InvCommon.h"
#include "InvDatabase.h"
//#include "InvReader.h"
#include "610InvReader.h"

class CBusiness : public sigslot::has_slots<>, public LmnToolkits::MessageHandler {

public:
	CBusiness();
	~CBusiness();

	// 重连数据库(异步，只发送消息到db线程)
	int   ReconnectDatabaseAsyn( DWORD dwDelayTime = 0 );
	int   ReconnectDatabase( );

	// 重连Reader(异步)
	int   ReconnectReaderAsyn(DWORD dwDelayTime = 0 );
	int   ReconnectReader();
	void  OnReaderStatusChange(C601InvReader::READER_STATUS);
	void  OnReaderReconnectAsyn( DWORD dwRelayTime );
	void  OnInvTagItem(const BYTE *, DWORD);
	void  OnInventoryAsyn();

	// 盘点
	int   InventoryAsyn();
	int   Inventory();

	// 通知界面DB Status
	int   NotifyUiDbStatus(CInvDatabase::DATABASE_STATUS eStatus);
	int   NotifyUiReaderStatus(C601InvReader::READER_STATUS eStatus);

	CInvDatabase::DATABASE_STATUS  GetDbStatus();
	C601InvReader::READER_STATUS      GetReaderStatus();

	// 通知界面盘点的item
	int   NotifyUiInventory(const TagItem * pItem);

	// 通知界面登录结果
	int   NotifyUiLoginRet();

	// 是否登录成功
	BOOL  IfLogined () const;
	const char * GetUserName();
	BOOL  IfUserId( const TagItem * pItem ) const;

	// 用盘点的Tag ID信息登录
	int   LoginUserAsyn(const TagItem * pItem);
	int   LoginUser(const CTagItemParam * pItem);

	// 保存小盘点
	int   InvSmallSaveAsyn( const CString & strBatchId,  const std::vector<TagItem *> & v );
	int   InvSmallSave( const CInvSmallSaveParam * pParam);
	int   NotifyUiInvSmallSaveRet( int nError, const CString & strBatchId );

	// check
	int   CheckTagAsyn(const TagItem * pItem);
	int   CheckTag(const CTagItemParam * pItem );
	int   NotifyUiCheckTagRet( int nRet, const CTagItemParam * pItem );

	// 设置定时器
	int   SetTimer( DWORD dwTimerId, DWORD dwDelayTime );
	int   NotifyUiTimer(DWORD dwTimerId);

	// 保存大盘点
	int   InvBigSaveAsyn(const CString & strBatchId, const std::vector<CString *> & v);
	int   InvBigSave(const CInvBigSaveParam * pParam);
	int   NotifyUiInvBigSaveRet(int nError, const CString & strBatchId, const CString & strWrongSmallPkgId );

	// 查询
	int   QueryAsyn(time_t tStart, time_t tEnd, const char * szBatchId, const char * szOperator, int nQueryType );
	int   Query( const CQueryParam * pParam );
	int   NotifyUiQueryRet(int nError,int nQueryType, std::vector<QueryResultItem *> * pvRet);

	// 查询小包装的Tags
	int   QuerySmallAsyn(DWORD dwId);
	int   QuerySmall(const CQuerySmallParam * pParam);
	int   NotifyUiQuerySmallRet(int nError, std::vector<QuerySmallResultItem *> * pvRet);

	// 查询大包装的Tags
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

	// 消息处理
	void OnMessage( DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:

	BOOL           m_bLogin;                                // 是否登录
	User           m_user;                                  // 登录的用户
	CInvDatabase   m_InvDatabase;                           // 数据库
	C601InvReader    m_InvReader;
};


