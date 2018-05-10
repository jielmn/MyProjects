#include <afx.h>
#include <assert.h>
#include "Business.h"


CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

#ifndef READER_TYPE_INV
CBusiness::CBusiness() : m_InvDatabase(this) 
#else
CBusiness::CBusiness() : m_InvDatabase(this), m_InvReader(this)
#endif
{
	sigInit.connect(this, &CBusiness::OnInit);
	sigDeinit.connect(this, &CBusiness::OnDeInit);
	m_bLogin = FALSE;
	memset(&m_user, 0, sizeof(m_user));

#ifndef READER_TYPE_INV
	m_InvReader.m_sigStatusChange.connect(this, &CBusiness::OnReaderStatusChange);
	m_InvReader.m_sigReconnect.connect(this, &CBusiness::OnReaderReconnectAsyn);
	m_InvReader.m_sigInvTagIetm.connect(this, &CBusiness::OnInvTagItem);
	m_InvReader.m_sigInventoryAsyn.connect(this, &CBusiness::OnInventoryAsyn);
#endif

}

CBusiness::~CBusiness() {
	Clear();
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

	m_bLogin = FALSE;
}




void CBusiness::OnInit(int * ret) {
	assert(ret);
	g_log = new FileLog();
	if (0 == g_log) {
		*ret = INV_ERR_NO_MEMORY;
		return;
	}
	g_log->Init("Inventory.log");

	g_cfg = new FileConfig();
	if (0 == g_cfg) {
		*ret = INV_ERR_NO_MEMORY;
		return;
	}
	g_cfg->Init("Inventory.cfg");

#ifdef READER_TYPE_INV
	err_t iRet = RDR_LoadReaderDrivers(_T("\\Drivers"));
	if (0 != iRet) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to load reader drivers!\n");
	}
#endif

	g_cfg->GetConfig("serial port sleep time", SERIAL_PORT_SLEEP_TIME, 6000);

#ifdef READER_TYPE_INV
	g_cfg->GetConfig("reader inventory interval", m_InvReader.m_dwInventoryInterval, 1000);
#endif

	g_thrd_db = new LmnToolkits::Thread();
	if (0 == g_thrd_db) {
		*ret = INV_ERR_NO_MEMORY;
		return;
	}
	g_thrd_db->Start();

	g_thrd_reader = new LmnToolkits::Thread();
	if (0 == g_thrd_reader) {
		*ret = INV_ERR_NO_MEMORY;
		return;
	}
	g_thrd_reader->Start();

	// timer线程
	g_thrd_timer = new LmnToolkits::Thread();
	if (0 == g_thrd_timer) {
		*ret = INV_ERR_NO_MEMORY;
		return;
	}
	g_thrd_timer->Start();

	

	*ret = 0;
}

void CBusiness::OnDeInit(int * ret) {
	assert(ret);

	if (g_thrd_db) {
		g_thrd_db->Stop();
		delete g_thrd_db;
		g_thrd_db = 0;
	}
	
	
	if (g_thrd_reader) {
		g_thrd_reader->Stop();
		delete g_thrd_reader;
		g_thrd_reader = 0;
	}

	if (g_thrd_timer) {
		g_thrd_timer->Stop();
		delete g_thrd_timer;
		g_thrd_timer = 0;
	}
	
	Clear();

	*ret = 0;
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	int ret = 0;

	switch ( dwMessageId )
	{
	case MSG_RECONNECT_DB:
	{
		ReconnectDatabase();
	}
	break;

	case MSG_RECONNECT_READER:
	{
		ReconnectReader();
	}
	break;

	case MSG_READER_INVENTORY:
	{
		Inventory();
	}
	break;

	case MSG_USER_LOGIN:
	{
		CTagItemParam * pItem = (CTagItemParam *)pMessageData;
		LoginUser(pItem);

	}
	break;

	case MSG_INV_SMALL_SAVE:
	{
		CInvSmallSaveParam * pParam = (CInvSmallSaveParam *)pMessageData;
		InvSmallSave(pParam);
	}
	break;

	case MSG_CHECK_TAG:
	{
		CTagItemParam * pItem = (CTagItemParam *)pMessageData;
		CheckTag(pItem);
	}
	break;

	case MSG_INV_BIG_SAVE: {
		CInvBigSaveParam * pParam = (CInvBigSaveParam *)pMessageData;
		InvBigSave(pParam);
	}
	break;

	case MSG_QUERY:
	{
		CQueryParam * pParam = (CQueryParam *)pMessageData;
		Query(pParam);
	}
	break;

	case MSG_QUERY_SMALL:
	{
		CQuerySmallParam * pParam = (CQuerySmallParam *)pMessageData;
		QuerySmall(pParam);
	}
	break;

	case MSG_QUERY_BIG:
	{
		CQueryBigParam * pParam = (CQueryBigParam *)pMessageData;
		QueryBig(pParam);
	}
	break;

	default:
	{
		// 定时器范围是10000~19999
		if ( dwMessageId >= MIN_TIMER_ID && dwMessageId <= MAX_TIMER_ID ) {
			NotifyUiTimer(dwMessageId);
		}
	}
	break;

	}
}


// 重连数据库
int   CBusiness::ReconnectDatabase() {
	int ret = m_InvDatabase.ReconnectDb();
	return ret;
}

// 重连Reader
int   CBusiness::ReconnectReader() {
#ifdef READER_TYPE_INV
	int ret = m_InvReader.ReconnectReader();
#else
	int ret = m_InvReader.Reconnect();
#endif
	return ret;
}

void  CBusiness::OnReaderStatusChange(int eStatus ) {
	NotifyUiReaderStatus(eStatus);
}

void  CBusiness::OnReaderReconnectAsyn(DWORD dwRelayTime) {
	ReconnectReaderAsyn(dwRelayTime);
}

void  CBusiness::OnInvTagItem(const BYTE * abyUid, DWORD dwUidLen ) {
	TagItem * pItem = new TagItem;
	memcpy(pItem->abyUid, abyUid, dwUidLen);
	pItem->dwUidLen = dwUidLen;
	NotifyUiInventory(pItem);
}

void   CBusiness::OnInventoryAsyn() {
	InventoryAsyn();
}

// 重连数据库(异步)
int   CBusiness::ReconnectDatabaseAsyn(DWORD dwDelayTime /*= 0*/) {
	if ( 0 == dwDelayTime ) {
		g_thrd_db->PostMessage(this, MSG_RECONNECT_DB);
	}
	else {
		g_thrd_db->PostDelayMessage( dwDelayTime, this, MSG_RECONNECT_DB );
	}	
	return 0;
}

// 重连Reader(异步)
int   CBusiness::ReconnectReaderAsyn(DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_reader->PostMessage(this, MSG_RECONNECT_READER);
	}
	else {
		g_thrd_reader->PostDelayMessage(dwDelayTime, this, MSG_RECONNECT_READER);
	}
	return 0;
}

// 通知界面db status
int    CBusiness::NotifyUiDbStatus(CInvDatabase::DATABASE_STATUS eStatus) {
	::PostMessage( g_hWnd, UM_SHOW_DB_STATUS, eStatus, 0 );
	return 0;
}

int   CBusiness::NotifyUiReaderStatus(int eStatus) {
	::PostMessage( g_hWnd, UM_SHOW_READER_STATUS, eStatus, 0);
	return 0;
}

int   CBusiness::InventoryAsyn(DWORD dwDelayTime /*= 0*/) {
	if (0 == dwDelayTime) {
		g_thrd_reader->PostMessage(this, MSG_READER_INVENTORY);
	}
	else {
		g_thrd_reader->PostDelayMessage( dwDelayTime, this, MSG_READER_INVENTORY);
	}	
	return 0;
}

int   CBusiness::Inventory() {
	m_InvReader.Inventory();
	return 0;
}

// 是否登录成功
BOOL  CBusiness::IfLogined() const {
	return m_bLogin;
}

int   CBusiness::NotifyUiInventory(const TagItem * pItem) {
	::PostMessage(g_hWnd, UM_INVENTORY_RESULT, (WPARAM)pItem, 0);
	return 0;
}


// 用盘点的Tag ID信息登录
int   CBusiness::LoginUserAsyn(const TagItem * pItem) {
	g_log->Output(ILog::LOG_SEVERITY_INFO, "login...\n");
	g_thrd_db->PostMessage( this, MSG_USER_LOGIN, new CTagItemParam(pItem) );
	return 0;
}

int   CBusiness::LoginUser(const CTagItemParam * pItem) {
	int ret = m_InvDatabase.LoginUser( pItem, &m_user);

	// 登录成功
	if (0 == ret) {
		m_bLogin = TRUE;		
	}
	
	NotifyUiLoginRet();
	return 0;
}

// 通知界面登录结果
int   CBusiness::NotifyUiLoginRet() {
	if (m_bLogin) {
		User * pUser = new User;
		memcpy(pUser, &m_user, sizeof(m_user));
		::PostMessage(g_hWnd, UM_LOGIN_RESULT, (WPARAM)m_bLogin, (LPARAM)pUser);
	}
	else {
		::PostMessage(g_hWnd, UM_LOGIN_RESULT, (WPARAM)m_bLogin, 0 );
	}
	
	return 0;
}

CInvDatabase::DATABASE_STATUS  CBusiness::GetDbStatus() {
	return  m_InvDatabase.GetStatus();
}

int    CBusiness::GetReaderStatus() {
	return  m_InvReader.GetStatus();
}

const char * CBusiness::GetUserName() {
	if (m_bLogin) {
		return m_user.szUserName;
	}
	else {
		return "";
	}
}

// 判断盘点的id是否是用户登录的id
BOOL  CBusiness::IfUserId(const TagItem * pItem) const {
	if ( 0 == pItem) {
		return FALSE;
	}

	if (!m_bLogin) {
		return FALSE;
	}

	return ( 0 == CompTag(pItem, &m_user.tTagId) );
}


int   CBusiness::InvSmallSaveAsyn( const CString & strBatchId, const std::vector<TagItem *> & v) {

	CInvSmallSaveParam * pParam = new CInvSmallSaveParam;
	pParam->m_strBatchId = strBatchId;

	std::vector<TagItem *>::const_iterator it;
	for ( it = v.begin(); it != v.end(); it++ ) {
		TagItem * pItem = *it;
		TagItem * pNewItem = new TagItem;
		if ( 0 == pNewItem ) {
			g_log->Output(ILog::LOG_SEVERITY_ERROR, "no memory!\n");
			delete pParam;
			return -1;
		}

		memcpy(pNewItem, pItem, sizeof(TagItem));
		pParam->m_items.push_back(pNewItem);
	}

	g_thrd_db->PostMessage(this, MSG_INV_SMALL_SAVE, pParam);
	return 0;
}

int   CBusiness::InvSmallSave(const CInvSmallSaveParam * pParam ) {
	CString strBatchId;
	int ret = m_InvDatabase.InvSmallSave(pParam, m_user.szUserId, strBatchId );
	NotifyUiInvSmallSaveRet(ret, strBatchId);
	return 0;
}

int   CBusiness::NotifyUiInvSmallSaveRet(int nError, const CString & strBatchId ) {
	if (0 == nError) {
		CString * pNewString = new CString(strBatchId);
		::PostMessage(g_hWnd, UM_INV_SMALL_SAVE_RESULT, (WPARAM)nError, (LPARAM)pNewString );
	}
	else {
		::PostMessage(g_hWnd, UM_INV_SMALL_SAVE_RESULT, (WPARAM)nError, 0);
	}	
	return 0;
}


int  CBusiness::CheckTagAsyn(const TagItem * pItem) {
	g_thrd_db->PostMessage( this, MSG_CHECK_TAG, new CTagItemParam(pItem) );
	return 0;
}

int   CBusiness::CheckTag(const CTagItemParam * pItem) {
	int ret = m_InvDatabase.CheckTag(pItem);
	NotifyUiCheckTagRet(ret, pItem);
	return 0;
}

int   CBusiness::NotifyUiCheckTagRet(int nRet, const CTagItemParam * pItem) {
	TagItem * pNewItem = new TagItem;
	memcpy(pNewItem, &pItem->m_item, sizeof(TagItem));
	::PostMessage(g_hWnd, UM_CHECK_TAG_RESULT, (WPARAM)nRet, (LPARAM)pNewItem );
	return 0;
}

int   CBusiness::SetTimer( DWORD dwTimerId, DWORD dwDelayTime ) {
	// 定时器范围错误
	if ( !(dwTimerId >= MIN_TIMER_ID && dwTimerId <= MAX_TIMER_ID) ) {
		return -1;
	}

	g_thrd_timer->PostDelayMessage( dwDelayTime, this, dwTimerId, 0, TRUE );
	return 0;
}

int   CBusiness::NotifyUiTimer(DWORD dwTimerId) {
	::PostMessage(g_hWnd, UM_TIMER, (WPARAM)dwTimerId,  0);
	return 0;
}


int   CBusiness::InvBigSaveAsyn(const CString & strBatchId, const std::vector<CString *> & v) {
	CInvBigSaveParam * pParam = new CInvBigSaveParam;
	pParam->m_strBatchId = strBatchId;

	std::vector<CString *>::const_iterator it;
	for (it = v.begin(); it != v.end(); it++) {
		CString * pItem = *it;
		CString * pNewItem = new CString(*pItem);
		if (0 == pNewItem) {
			g_log->Output(ILog::LOG_SEVERITY_ERROR, "no memory!\n");
			delete pParam;
			return -1;
		}

		pParam->m_items.push_back(pNewItem);
	}

	g_thrd_db->PostMessage(this, MSG_INV_BIG_SAVE, pParam);
	return 0;
}

int   CBusiness::InvBigSave(const CInvBigSaveParam * pParam) {
	CString strBatchId;
	CString strWrongSmallPkgId;
	int ret = m_InvDatabase.InvBigSave(pParam, m_user.szUserId, strBatchId, strWrongSmallPkgId);
	NotifyUiInvBigSaveRet(ret, strBatchId, strWrongSmallPkgId);
	return 0;
}

int   CBusiness::NotifyUiInvBigSaveRet(int nError, const CString & strBatchId, const CString & strWrongSmallPkgId) {
	if (0 == nError) {
		CString * pNewString = new CString(strBatchId);
		::PostMessage(g_hWnd, UM_INV_BIG_SAVE_RESULT, (WPARAM)nError, (LPARAM)pNewString);
	}
	else {
		CString * pNewString = new CString(strWrongSmallPkgId);
		::PostMessage(g_hWnd, UM_INV_BIG_SAVE_RESULT, (WPARAM)nError, (LPARAM)pNewString);
	}
	return 0;
}

int   CBusiness::QueryAsyn(time_t tStart, time_t tEnd, const char * szBatchId, const char * szOperator, int nQueryType) {
	g_thrd_db->PostMessage( this, MSG_QUERY, new CQueryParam(tStart, tEnd, szBatchId, szOperator, nQueryType) );
	return 0;
}

int   CBusiness::Query(const CQueryParam * pParam) {
	std::vector<QueryResultItem *> * pvRet = new std::vector<QueryResultItem *>;
	int ret = m_InvDatabase.Query(pParam, *pvRet);
	NotifyUiQueryRet(ret, pParam->m_nQueryType, pvRet);
	return 0;
}

int   CBusiness::NotifyUiQueryRet(int nError, int nQueryType, std::vector<QueryResultItem *> * pvRet) {
	if (0 == nError) {
		QueryResult * pRet = new QueryResult;
		memset(pRet, 0, sizeof(QueryResult));

		pRet->nQueryType = nQueryType;
		pRet->pvRet = pvRet;

		::PostMessage(g_hWnd, UM_QUERY_RESULT, (WPARAM)nError, (LPARAM)pRet);
	}
	else {
		::PostMessage(g_hWnd, UM_QUERY_RESULT, (WPARAM)nError, (LPARAM)0);

		if (pvRet) {
			ClearVector(*pvRet);
			delete pvRet;
		}
	}
	return 0;
}


int  CBusiness::QuerySmallAsyn(DWORD dwId) {
	g_thrd_db->PostMessage( this, MSG_QUERY_SMALL, new CQuerySmallParam(dwId) );
	return 0;
}

int   CBusiness::QuerySmall(const CQuerySmallParam * pParam) {
	std::vector<QuerySmallResultItem *> * pvRet = new std::vector<QuerySmallResultItem *>;
	int ret = m_InvDatabase.QuerySmall(pParam, *pvRet);
	NotifyUiQuerySmallRet(ret, pvRet);
	return 0;
}

int   CBusiness::NotifyUiQuerySmallRet(int nError, std::vector<QuerySmallResultItem *> * pvRet) {
	if (0 == nError) {
		::PostMessage( g_hWnd, UM_QUERY_SMALL_RESULT, (WPARAM)nError, (LPARAM)pvRet );
	}
	else {
		::PostMessage(g_hWnd, UM_QUERY_SMALL_RESULT, (WPARAM)nError, (LPARAM)0);

		if (pvRet) {
			ClearVector(*pvRet);
			delete pvRet;
		}
	}
	return 0;
}


int  CBusiness::QueryBigAsyn(DWORD dwId) {
	g_thrd_db->PostMessage(this, MSG_QUERY_BIG, new CQueryBigParam(dwId));
	return 0;
}

int   CBusiness::QueryBig( const CQueryBigParam * pParam ) {
	std::vector<QueryResultItem *> * pvRet = new std::vector<QueryResultItem *>;
	int ret = m_InvDatabase.QueryBig(pParam, *pvRet);
	NotifyUiQueryBigRet(ret, pvRet);
	return 0;
}

int   CBusiness::NotifyUiQueryBigRet(int nError, std::vector<QueryResultItem *> * pvRet) {
	if (0 == nError) {
		::PostMessage(g_hWnd, UM_QUERY_BIG_RESULT, (WPARAM)nError, (LPARAM)pvRet);
	}
	else {
		::PostMessage(g_hWnd, UM_QUERY_BIG_RESULT, (WPARAM)nError, (LPARAM)0);

		if (pvRet) {
			ClearVector(*pvRet);
			delete pvRet;
		}
	}
	return 0;
}