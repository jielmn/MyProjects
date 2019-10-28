#include "business.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {

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

	g_data.m_cfg->GetConfig("baud", g_data.m_dwBaud, 9600);

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
	return 0;
}


void  CBusiness::StartAsyn(int nComPort) {
	g_data.m_thrd_db->PostMessage(this, MSG_START, new CStartParam(nComPort) );
}

void  CBusiness::Start(const CStartParam * pParam) {
	char szCom[256];
	SNPRINTF(szCom, sizeof(szCom), "com%d", pParam->m_nComPort);
	BOOL bRet = m_serial_port.OpenUartPort(szCom, g_data.m_dwBaud);
	PostMessage(g_data.m_hWnd, UM_START_RET, bRet, 0);

	g_data.m_thrd_db->PostMessage(this, MSG_READ);
}

void  CBusiness::StopAsyn() {
	g_data.m_thrd_db->PostMessage(this, MSG_STOP);
}

void  CBusiness::Stop() {
	BOOL bRet = m_serial_port.CloseUartPort();
	PostMessage(g_data.m_hWnd, UM_STOP_RET, bRet, 0);
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_START:
	{
		CStartParam * pParam = (CStartParam *)pMessageData;
		Start(pParam);
	}
	break;

	case MSG_STOP:
	{
		Stop();
	}
	break;

	case MSG_READ:
	{
		BYTE  buf[8192];
		DWORD  dwSize = sizeof(buf);
		m_serial_port.Read(buf, dwSize);
#if !NEW_VERSION_FLAG
		if ( dwSize == 8 ) {
#else
		if (dwSize == 9) {
#endif
			DWORD  dwWrite = 2;
			m_serial_port.Write("OK", dwWrite);
		}
		g_data.m_thrd_db->PostMessage(this, MSG_READ);
	}
	break;

	default:
		break;
	}
}