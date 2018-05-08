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
	m_thrd_timer.Start();


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
	m_thrd_timer.Stop();

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
	int ret = m_db.Login(pParam, m_strLoginId, m_strLoginName);
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

int  CBusiness::GetAllAgencyForTargetAsyn() {
	m_thrd_db.PostMessage(this, MSG_GET_ALL_AGENCY_FOR_TARGET);
	return 0;
}

int  CBusiness::GetAllAgencyForTarget() {
	std::vector<AgencyItem *> vRet;
	int ret = m_db.GetAllAgency(vRet);
	m_sigGetAllAgencyForTarget.emit(ret, vRet);
	ClearVector(vRet);
	return 0;
}

// 修改经销商
int  CBusiness::ModifyAgencyAsyn(const AgencyItem * pItem) {
	m_thrd_db.PostMessage(this, MSG_MODIFY_AGENCY, new CAgencyParam(pItem));
	return 0;
}

int  CBusiness::ModifyAgency(const CAgencyParam * pParam) {
	int ret = m_db.ModifyAgency(pParam);
	m_sigModifyAgency.emit(ret);
	return 0;
}

// 删除经销商
int  CBusiness::DeleteAgencyAsyn(DWORD  dwAgentId,const char * szAgentId) {
	m_thrd_db.PostMessage(this, MSG_DELETE_AGENCY, new CAgencyParam(dwAgentId, szAgentId));
	return 0;
}

int  CBusiness::DeleteAgency(const CAgencyParam * pParam ) {
	int ret = m_db.DeleteAgency(pParam);
	m_sigDeleteAgency.emit(ret, pParam->m_tAgency.dwId);
	return 0;
}

// 获取所有的销售
int  CBusiness::GetAllSalesAsyn() {
	m_thrd_db.PostMessage(this, MSG_GET_ALL_SALES);
	return 0;
}

int  CBusiness::GetAllSales() {
	std::vector<SaleStaff *> vRet;
	int ret = m_db.GetAllSales(vRet );

	m_sigGetAllSaleStaff.emit(ret, vRet);

	ClearVector(vRet);
	return 0;
}

// 设置定时器
int   CBusiness::SetTimerAsyn(DWORD dwTimerId, DWORD dwDelayTime) {
	// 定时器范围错误
	if (!(dwTimerId >= MIN_TIMER_ID && dwTimerId <= MAX_TIMER_ID)) {
		return -1;
	}

	m_thrd_timer.PostDelayMessage(dwDelayTime, this, dwTimerId, 0, TRUE);
	return 0;
}

int   CBusiness::NotifyUiTimer(DWORD dwTimerId) {
	m_sigTimer.emit(dwTimerId);
	return 0;
}

int   CBusiness::SaveInvOutAsyn( int nTargetType, const DuiLib::CDuiString & strTargetId, 
	                             const std::vector<DuiLib::CDuiString *> & vBig, 
	                             const std::vector<DuiLib::CDuiString *> & vSmall) {
	m_thrd_db.PostMessage(this, MSG_SAVE_INV_OUT, new CSaveInvOutParam( nTargetType, strTargetId, m_strLoginId, vBig, vSmall));
	return 0;
}

int   CBusiness::SaveInvOut( const CSaveInvOutParam * pParam ) {
	int ret = m_db.SaveInvOut(pParam);
	m_sigSaveInvOutRet.emit(ret);
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

	case MSG_MODIFY_AGENCY:
	{
		CAgencyParam * pParam = (CAgencyParam *)pMessageData;
		ModifyAgency(pParam);
	}
	break;

	case MSG_DELETE_AGENCY:
	{
		CAgencyParam * pParam = (CAgencyParam *)pMessageData;
		DeleteAgency(pParam);
	}
	break;

	case MSG_GET_ALL_SALES:
	{
		GetAllSales();
	}
	break;

	case MSG_GET_ALL_AGENCY_FOR_TARGET:
	{
		GetAllAgencyForTarget();
	}
	break;

	case MSG_SAVE_INV_OUT:
	{
		CSaveInvOutParam * pParam = (CSaveInvOutParam *)pMessageData;
		SaveInvOut(pParam);
	}
	break;

	default:
	{
		// 定时器范围是10000~19999
		if (dwMessageId >= MIN_TIMER_ID && dwMessageId <= MAX_TIMER_ID) {
			NotifyUiTimer(dwMessageId);
		}
	}
	break;

	}
}