#pragma once

#include "LmnThread.h"
#include "InvOutCommon.h"
#include "InvOutDatabase.h"
#include "UIlib.h"

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
	int  DeleteAgencyAsyn(DWORD  dwAgentId, const char * szAgentId);
	int  DeleteAgency(const CAgencyParam * pParam);

	// 获取所有的销售
	int  GetAllSalesAsyn();
	int  GetAllSales();

	// 获取所有经销商
	int  GetAllAgencyForTargetAsyn();
	int  GetAllAgencyForTarget();

	// 设置定时器
	int   SetTimerAsyn(DWORD dwTimerId, DWORD dwDelayTime);
	int   NotifyUiTimer(DWORD dwTimerId);

	// 保存出库
	int   SaveInvOutAsyn(int nTargetType, const DuiLib::CDuiString & strTargetId, const std::vector<DuiLib::CDuiString *> & vBig, const std::vector<DuiLib::CDuiString *> & vSmall);
	int   SaveInvOut( const CSaveInvOutParam * pParam );

	// 根据时间查询结果
	int   QueryByTimeAsyn(time_t tStart, time_t tEnd, int nTargetType, const char * szTargetId);
	int   QueryByTime( const CQueryByTimeParam * pParam );

	// 根据大包装查询
	int   QueryByBigPkgAsyn(const char * szBigPkgId);
	int   QueryByBigPkg(const CQueryByBigPkgParam * pParam);

	// 根据小包装查询
	int   QueryBySmallPkgAsyn(const char * szSmallPkgId);
	int   QueryBySmallPkg(const CQueryBySmallPkgParam * pParam);

	// 根据Tag查询
	int   QueryByTagAsyn(const char * szTagId);
	int   QueryByTag(const CQueryByTagParam * pParam);

public:
	sigslot::signal1<CLmnOdbc::DATABASE_STATUS>                     m_sigStatusChange;
	sigslot::signal1<int>                                           m_sigLoginRet;
	sigslot::signal2<int, DWORD>                                    m_sigAddAgency;
	sigslot::signal2<int, const std::vector<AgencyItem *> & >       m_sigGetAllAgency;
	sigslot::signal2<int, const std::vector<AgencyItem *> & >       m_sigGetAllAgencyForTarget;
	sigslot::signal1<int>                                           m_sigModifyAgency;
	sigslot::signal2<int, DWORD>                                    m_sigDeleteAgency;

	sigslot::signal2<int, const std::vector<SaleStaff *> & >        m_sigGetAllSaleStaff;

	sigslot::signal1<DWORD>                                         m_sigTimer;
	sigslot::signal1<int>                                           m_sigSaveInvOutRet;

	// 查询
	sigslot::signal2<int, const std::vector<QueryByTimeItem*> & >   m_sigQueryByTime;
	sigslot::signal3<int, const std::vector<PkgItem*> &, const std::vector<PkgItem*> & >   m_sigQueryByBigPkg;
	sigslot::signal2<int, const std::vector<PkgItem*> &>                                   m_sigQueryBySmallPkg;
	sigslot::signal2<int, const TagItem &>                                                 m_sigQueryByTag;

private:
	static CBusiness *  pInstance;

	void Clear();

	// 消息处理
	void OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData);
	BOOL CanBeFreed() { return false; }

private:
	LmnToolkits::Thread   m_thrd_db;
	LmnToolkits::Thread   m_thrd_timer;

private:
	CInvoutDatabase       m_db;
	char                  m_szOdbcString[256];
	DuiLib::CDuiString    m_strLoginId;

public:
	DuiLib::CDuiString    m_strLoginName;
};





