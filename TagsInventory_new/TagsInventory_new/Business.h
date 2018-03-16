#pragma once

#include "InvCommon.h"
#include "InvDatabase.h"

class CBusiness : public sigslot::has_slots<>, public LmnToolkits::MessageHandler {

public:
	CBusiness();
	~CBusiness();

	// 重连数据库(异步，只发送消息到db线程)
	int   ReconnectDatabaseAsyn( DWORD dwDelayTime = 0 );
	int   ReconnectDatabase();

	// 重连Reader(异步)
	int   ReconnectReaderAsyn();
	int   ReconnectReader();

	// 是否登录成功
	BOOL  IfLogined () const;

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
	CInvDatabase   m_InvDatabase;                           // 数据库
};


