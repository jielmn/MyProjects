#include "business.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {
	for (DWORD i = 0; i < MAX_TAG_CNT; i++) {
		for (DWORD j = 0; j < 8; j++) {
			m_tagid[i][j] = (BYTE)GetRand(0, 255);
		}
	}
}

CBusiness::~CBusiness() {
	Clear();	
}

void CBusiness::Clear() {
	if (g_data.m_log) {
		g_data.m_log->Deinit();
		delete g_data.m_log;
		g_data.m_log = 0;
	}

	if (g_data.m_cfg) {
		g_data.m_cfg->Deinit();
		delete g_data.m_cfg;
		g_data.m_cfg = 0;
	}
}

int CBusiness::Init() {
	g_data.m_log = new FileLog();
	if (0 == g_data.m_log) {
		return -1;
	}

#ifdef _DEBUG
	g_data.m_log->Init(LOG_FILE_NAME);
#else
	g_data.m_log->Init(LOG_FILE_NAME, 0, ILog::LOG_SEVERITY_INFO, TRUE);
#endif	


	g_data.m_cfg = new FileConfigEx();
	if (0 == g_data.m_cfg) {
		return -1;
	}
	g_data.m_cfg->Init(CONFIG_FILE_NAME);

	g_data.m_thrd_db = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_db) {
		return -1;
	}
	g_data.m_thrd_db->Start();

	return 0;
}

int CBusiness::DeInit() {

	if (g_data.m_thrd_db) {
		g_data.m_thrd_db->Stop();
		delete g_data.m_thrd_db;
		g_data.m_thrd_db = 0;
	}

	Clear();

	m_com.CloseUartPort();
	return 0;
}

void  CBusiness::OpenComAsyn() {
	g_data.m_thrd_db->PostMessage(this, 1001);
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case 1001:
	{
		g_data.m_cfg->GetConfig("com", g_data.m_szCom, sizeof(g_data.m_szCom), "com18");

		BOOL bRet = m_com.OpenUartPort(g_data.m_szCom);
		::PostMessage(g_data.m_hWnd, UM_COM_RET, bRet, 0);

		g_data.m_thrd_db->PostDelayMessage(10000, this, 1002);
	}
	break;

	case 1002: 
	{
		if ( m_com.GetStatus() == CLmnSerialPort::OPEN && g_data.m_bStart ) {
			BYTE  data[19];
			memcpy(data, "\x55\x11\x01\x01\x00\x00\x01\x00\xE2\x8C\x8A\x8D\x8B\x5F\x5C\x6D\x23\x0C\xFF", 19);
			DWORD i = GetRand(0, MAX_TAG_CNT - 1);
			memcpy(data + 8, m_tagid[i], 8);
			WORD  wTemp = GetRand(3500, 4000);
			data[16] = wTemp / 100;
			data[17] = wTemp % 100;
			DWORD  dwLen = 19;
			m_com.Write(data, dwLen);
		}

		g_data.m_thrd_db->PostDelayMessage(10000, this, 1002);
	}
	break;

	default:
		break;
	}
}