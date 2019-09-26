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

	DWORD  dwValue = 0;
	g_data.m_cfg->GetConfig("serial port", dwValue, 0);
	g_data.m_nComPort = dwValue;

	g_data.m_cfg->GetConfig("min beat", dwValue, 50);
	g_data.m_byMinBeat = (BYTE)dwValue;

	g_data.m_cfg->GetConfig("max beat", dwValue, 100);
	g_data.m_byMaxBeat = (BYTE)dwValue;

	g_data.m_cfg->GetConfig("min oxy", dwValue, 90);
	g_data.m_byMinOxy = (BYTE)dwValue;

	g_data.m_cfg->GetConfig("min temperature", dwValue, 0);
	g_data.m_nMinTemp = dwValue;

	g_data.m_cfg->GetConfig("max temperature", dwValue, 3800);
	g_data.m_nMaxTemp = dwValue;

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

void  CBusiness::OpenComAsyn() {
	g_data.m_thrd_db->PostMessage(this, MSG_OPEN_COM);
}

// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_OPEN_COM:
	{
		if ( g_data.m_nComPort > 0 ) {
			char szCom[256];
			SNPRINTF(szCom, sizeof(szCom), "com%d", g_data.m_nComPort);
			BOOL bRet = m_serial_port.OpenUartPort(szCom, 19200);
			PostMessage(g_data.m_hWnd, UM_COM_STATUS, bRet, 0);

			g_data.m_thrd_db->PostDelayMessage(100, this, MSG_READ_COM);
		}
	}
	break;

	case MSG_READ_COM:
	{
		char   achData[8192];
		DWORD  dwLen = sizeof(achData);
		BOOL bRet = m_serial_port.Read(achData, dwLen);
		if (bRet && dwLen > 0) {
			m_buf.Append(achData, dwLen);
		}

		while ( m_buf.GetDataLength() >= 13 ) {
			m_buf.Read(achData, 13);
			char * pNewData = new char[13];
			memcpy(pNewData, achData, 13);
			::PostMessage(g_data.m_hWnd, UM_COM_DATA, (WPARAM)pNewData, 13);
		}
		m_buf.Reform();

		g_data.m_thrd_db->PostDelayMessage(100, this, MSG_READ_COM);
	}
	break;

	default:
		break;
	}
}