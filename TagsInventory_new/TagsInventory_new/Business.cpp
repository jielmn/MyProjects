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

CBusiness::CBusiness() : m_InvDatabase(this), m_InvReader(this) {
	sigInit.connect(this, &CBusiness::OnInit);
	sigDeinit.connect(this, &CBusiness::OnDeInit);
	m_bLogin = FALSE;
	memset(&m_user, 0, sizeof(m_user));
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

	err_t iRet = RDR_LoadReaderDrivers(_T("\\Drivers"));
	if (0 != iRet) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to load reader drivers!\n");
	}

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

	default:
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
	int ret = m_InvReader.ReconnectReader();
	return ret;
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

int   CBusiness::NotifyUiReaderStatus(CInvReader::READER_STATUS eStatus) {
	::PostMessage( g_hWnd, UM_SHOW_READER_STATUS, eStatus, 0);
	return 0;
}

int   CBusiness::InventoryAsyn() {
	g_thrd_reader->PostMessage( this, MSG_READER_INVENTORY );
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

CInvReader::READER_STATUS    CBusiness::GetReaderStatus() {
	return  m_InvReader.GetStatus();
}