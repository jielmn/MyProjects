#pragma once

#include "InvCommon.h"
#include "InvDatabase.h"
#include "InvReader.h"

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

	// 盘点
	int   InventoryAsyn();
	int   Inventory();

	// 通知界面DB Status
	int   NotifyUiDbStatus(CInvDatabase::DATABASE_STATUS eStatus);
	int   NotifyUiReaderStatus(CInvReader::READER_STATUS eStatus);

	CInvDatabase::DATABASE_STATUS  GetDbStatus();
	CInvReader::READER_STATUS      GetReaderStatus();

	// 通知界面盘点的item
	int   NotifyUiInventory(const TagItem * pItem);

	// 通知界面登录结果
	int   NotifyUiLoginRet();

	// 是否登录成功
	BOOL  IfLogined () const;

	// 用盘点的Tag ID信息登录
	int   LoginUserAsyn(const TagItem * pItem);
	int   LoginUser(const CTagItemParam * pItem);

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
	CInvReader     m_InvReader;
};


