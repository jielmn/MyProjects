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

	// ����Db
	int  ReconnectDbAsyn(DWORD dwDelayTime = 0);
	int  ReconnectDb();
	void OnDbStatusChange(CLmnOdbc::DATABASE_STATUS eNewStatus );
	CLmnOdbc::DATABASE_STATUS GetDbStatus();

	// ��¼
	int  LoginAsyn(const char * szUserName, const char * szPassword);
	int  Login( const CLoginParam * pParam );

	// ��Ӿ�����
	int  AddAgencyAsyn( const AgencyItem * pItem );
	int  AddAgency( const CAgencyParam * pParam );

	// ��ȡ���о�����
	int  GetAllAgencyAsyn();
	int  GetAllAgency();

	// �޸ľ�����
	int  ModifyAgencyAsyn(const AgencyItem * pItem);
	int  ModifyAgency(const CAgencyParam * pParam);

	// ɾ��������
	int  DeleteAgencyAsyn(DWORD  dwAgentId, const char * szAgentId);
	int  DeleteAgency(const CAgencyParam * pParam);

	// ��ȡ���е�����
	int  GetAllSalesAsyn();
	int  GetAllSales();

	// ��ȡ���о�����
	int  GetAllAgencyForTargetAsyn();
	int  GetAllAgencyForTarget();

	// ���ö�ʱ��
	int   SetTimerAsyn(DWORD dwTimerId, DWORD dwDelayTime);
	int   NotifyUiTimer(DWORD dwTimerId);

	int   SaveInvOutAsyn(int nTargetType, const DuiLib::CDuiString & strTargetId, const std::vector<DuiLib::CDuiString *> & vBig, const std::vector<DuiLib::CDuiString *> & vSmall);
	int   SaveInvOut( const CSaveInvOutParam * pParam );

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

private:
	static CBusiness *  pInstance;

	void Clear();

	// ��Ϣ����
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





