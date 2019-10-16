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

void  CBusiness::OpenComPortAsyn(int nCom) {
	g_data.m_thrd_db->PostMessage(this, MSG_OPEN_COM_PORT, new COpenComParam(nCom));
}

void  CBusiness::OpenComPort(const COpenComParam * pParam) {
	BOOL bRet = m_serial_port.OpenUartPort(pParam->m_nCom, 115200);
	::PostMessage(g_data.m_hWnd, UM_OPEN_COM_RET, bRet, 0);
	if (bRet) {
		g_data.m_thrd_db->PostDelayMessage(200, this, MSG_READ_COM);
	}
}

void  CBusiness::CloseComPortAsyn() {
	g_data.m_thrd_db->DeleteMessages();
	g_data.m_thrd_db->PostMessage(this, MSG_CLOSE_COM);
}

void  CBusiness::CloseComPort() {
	BOOL bRet = m_serial_port.CloseUartPort();
	::PostMessage(g_data.m_hWnd, UM_CLOSE_COM_RET, bRet, 0);
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_OPEN_COM_PORT:
	{
		COpenComParam * pParam = (COpenComParam *)pMessageData;
		OpenComPort(pParam);
	}
	break;

	case MSG_CLOSE_COM:
	{
		CloseComPort();
	}
	break;

	case MSG_READ_COM:
	{
		BYTE  buf[8192];
		DWORD  dwSize = sizeof(buf);
		BOOL bRet = m_serial_port.Read(buf, dwSize);
		if (bRet && dwSize > 0) {
			BYTE * pNewData = new BYTE[dwSize];
			memcpy(pNewData, buf, dwSize);
			::PostMessage(g_data.m_hWnd, UM_READ_COM_RET, (WPARAM)pNewData, dwSize);
		}
		g_data.m_thrd_db->PostDelayMessage(200, this, MSG_READ_COM);
	}
	break;

	default:
		break;
	}
}