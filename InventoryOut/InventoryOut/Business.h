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

	// 修改经销商
	int  ModifyAgencyAsyn(const AgencyItem * pItem);
	int  ModifyAgency(const CAgencyParam * pParam);

	// 删除经销商
	int  DeleteAgencyAsyn(DWORD  dwAgentId);
	int  DeleteAgency(const CAgencyParam * pParam);

	// 获取所有的销售
	int  GetAllSalesAsyn();
	int  GetAllSales();

	// 获取所有经销商
	int  GetAllAgencyForTargetAsyn();
	int  GetAllAgencyForTarget();

public:
	sigslot::signal1<CLmnOdbc::DATABASE_STATUS>                     m_sigStatusChange;
	sigslot::signal1<int>                                           m_sigLoginRet;
	sigslot::signal2<int, DWORD>                                    m_sigAddAgency;
	sigslot::signal2<int, const std::vector<AgencyItem *> & >       m_sigGetAllAgency;
	sigslot::signal2<int, const std::vector<AgencyItem *> & >       m_sigGetAllAgencyForTarget;
	sigslot::signal1<int>                                           m_sigModifyAgency;
	sigslot::signal2<int, DWORD>                                    m_sigDeleteAgency;

	sigslot::signal2<int, const std::vector<SaleStaff *> & >        m_sigGetAllSaleStaff;

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





