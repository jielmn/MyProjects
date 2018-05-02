#include "Business.h"

CBusiness *  CBusiness::pInstance = 0;

CBusiness::CBusiness() {
	memset(m_szOdbcString, 0, sizeof(m_szOdbcString));

	m_db.sigStatusChange.connect(this, &CBusiness::OnDbStatusChange);
}

CBusiness::~CBusiness() {
	DeInit();
}

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

int CBusiness::Init() {

	g_log = new FileLog();
	if (0 == g_log) {
		return -1;
	}
	g_log->Init("InvOut.log");

	g_cfg = new FileConfigEx();
	if (0 == g_cfg) {
		return -1;
	}
	g_cfg->Init("InvOut.cfg");

	m_thrd_db.Start();



	char  buf[8192];
	g_cfg->GetConfig( "odbc string", buf, sizeof(buf), "" );	

	DWORD dwLen = sizeof(m_szOdbcString);
	// 解密
	MyDecrypt(buf, m_szOdbcString, dwLen);


	ReconnectDbAsyn(200);

	return 0;
}

int CBusiness::DeInit() {
	m_thrd_db.Stop();

	Clear();
	return 0;
}


void CBusiness::Clear() {
	if (g_log) {
		g_log->Deinit();
		delete g_log;
		g_log = 0;
	}

	if (g_cfg) {
		g_cfg->Deinit();
		delete g_cfg;
		g_cfg = 0;
	}
}



int  CBusiness::ReconnectDbAsyn( DWORD dwDelayTime /*= 0*/ ) {
	if (0 == dwDelayTime) {
		m_thrd_db.PostMessage( this, MSG_RECONNECT_DB );
	}
	else {
		m_thrd_db.PostDelayMessage( dwDelayTime, this, MSG_RECONNECT_DB );
	}
	return 0;
}

int  CBusiness::ReconnectDb() {
	int ret = m_db.ConnectDb(m_szOdbcString);
	// 如果连接失败
	if (0 != ret) {
		ReconnectDbAsyn(RECONNECT_DB_TIME);
	}
	return 0;
}

void  CBusiness::OnDbStatusChange(CLmnOdbc::DATABASE_STATUS eNewStatus) {
	m_sigStatusChange.emit(eNewStatus);

	// 如果数据状态由开变为关，重连
	if (eNewStatus == CLmnOdbc::STATUS_CLOSE) {
		ReconnectDbAsyn(RECONNECT_DB_TIME);
	}
}

CLmnOdbc::DATABASE_STATUS CBusiness::GetDbStatus() {
	return m_db.GetStatus();
}


int  CBusiness::LoginAsyn(const char * szUserName, const char * szPassword) {
	m_thrd_db.PostMessage( this, MSG_LOGIN, new CLoginParam(szUserName, szPassword) );
	return 0;
}

int  CBusiness::Login( const CLoginParam * pParam ) {
	int ret = m_db.Login(pParam);
	m_sigLoginRet.emit(ret);
	return 0;
}

int  CBusiness::AddAgencyAsyn(const AgencyItem * pItem) {
	m_thrd_db.PostMessage(this, MSG_ADD_AGENCY, new CAgencyParam(pItem));
	return 0;
}

int  CBusiness::AddAgency(const CAgencyParam * pParam ) {
	DWORD  dwNewId = 0;
	int ret = m_db.AddAgency(pParam, dwNewId);
	m_sigAddAgency.emit(ret,dwNewId);
	return 0;
}

// 获取所有经销商
int  CBusiness::GetAllAgencyAsyn() {
	m_thrd_db.PostMessage(this, MSG_GET_ALL_AGENCY );	
	return 0;
}

int  CBusiness::GetAllAgency() {
	std::vector<AgencyItem *> vRet;
	int ret = m_db.GetAllAgency(vRet);
	m_sigGetAllAgency.emit(ret, vRet);
	ClearVector(vRet);
	return 0;
}

// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_RECONNECT_DB:
	{
		ReconnectDb();
	}
	break;

	case MSG_LOGIN:
	{
		CLoginParam * pParam = (CLoginParam *)pMessageData;
		Login( pParam );
	}
	break;

	case MSG_ADD_AGENCY:
	{
		CAgencyParam * pParam = (CAgencyParam *)pMessageData;
		AddAgency( pParam );
	}
	break;

	case MSG_GET_ALL_AGENCY:
	{
		GetAllAgency();
	}
	break;

	default:
		break;
	}
}