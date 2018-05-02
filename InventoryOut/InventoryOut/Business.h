#pragma once

#include "LmnThread.h"
#include "InvOutCommon.h"
#include "InvOutDatabase.h"

class CBusiness : public LmnToolkits::MessageHandler, public sigslot::has_slots<> {

public:
	CBusiness();
	~CBusiness();

	static CBusiness *  GetInstance();

	int Init();
	int DeInit();

	// 重连Db
	int  ReconnectDbAsyn(DWORD dwDelayTime = 0);
	int  ReconnectDb();
	void OnDbStatusChange(CLmnOdbc::DATABASE_STATUS eNewStatus );
	CLmnOdbc::DATABASE_STATUS GetDbStatus();

	// 登录
	int  LoginAsyn(const char * szUserName, const char * szPassword);
	int  Login( const CLoginParam * pParam );

	// 添加经销商
	int  AddAgencyAsyn( const AgencyItem * pItem );
	int  AddAgency( const CAgencyParam * pParam );

	// 获取所有经销商
	int  GetAllAgencyAsyn();
	int  GetAllAgency();

public:
	sigslot::signal1<CLmnOdbc::DATABASE_STATUS>                     m_sigStatusChange;
	sigslot::signal1<int>                                           m_sigLoginRet;
	sigslot::signal2<int, DWORD>                                    m_sigAddAgency;
	sigslot::signal2<int, const std::vector<AgencyItem *> & >       m_sigGetAllAgency;

private:
	static CBusiness *  pInstance;

	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	LmnToolkits::Thread   m_thrd_db;

private:
	CInvoutDatabase       m_db;
	char                  m_szOdbcString[256];
};





