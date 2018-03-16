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

CBusiness::CBusiness() {
	sigInit.connect(this, &CBusiness::OnInit);
	sigDeinit.connect(this, &CBusiness::OnDeInit);
	m_bLogin = FALSE;
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
	switch ( dwMessageId )
	{
	case MSG_RECONNECT_DB:
	{
		ReconnectDatabase();
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
	return 0;
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
int   CBusiness::ReconnectReaderAsyn() {
	return 0;
}


// 是否登录成功
BOOL  CBusiness::IfLogined() const {
	return m_bLogin;
}