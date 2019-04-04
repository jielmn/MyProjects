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
	g_data.m_cfg->GetConfig("max count", dwValue, 10);
	g_data.m_nMaxItemsCnt = dwValue;

	g_data.m_thrd_com = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_com) {
		return -1;
	}
	g_data.m_thrd_com->Start();

	return 0;
}

int CBusiness::DeInit() {

	if (g_data.m_thrd_com) {
		g_data.m_thrd_com->Stop();
		delete g_data.m_thrd_com;
		g_data.m_thrd_com = 0;
	}

	Clear();
	return 0;
}


int  CBusiness::OpenComPortAsyn(int nCom, int nBaud) {
	g_data.m_thrd_com->PostMessage(this, MSG_OPEN_COM_PORT, new COpenComPortParam(nCom, nBaud));
	return 0;
}

int  CBusiness::OpenComPort(const COpenComPortParam * pParam) {
	char  szComPort[32];
	int   nRegMsg = UM_OPEN_COM_PORT_RET;

	SNPRINTF(szComPort, sizeof(szComPort), "com%d", pParam->m_nCom);
	BOOL bRet = m_serial_port.OpenUartPort(szComPort, (DWORD)pParam->m_nBaud);
	if (!bRet) {
		::PostMessage(g_data.m_hWnd, nRegMsg, -1, 0);
		return 0;
	}

	::PostMessage(g_data.m_hWnd, nRegMsg, 0, 0);

	GetComDataAsyn(1000);
	return 0;
}

int  CBusiness::CloseComPortAsyn() {
	g_data.m_thrd_com->DeleteMessages();
	g_data.m_thrd_com->PostMessage(this, MSG_CLOSE_COM_PORT);
	return 0;
}

int  CBusiness::CloseComPort() {
	int   nRegMsg = UM_CLOSE_COM_PORT_RET;

	m_serial_port.CloseUartPort();
	
	::PostMessage(g_data.m_hWnd, nRegMsg, 0, 0);
	return 0;
}

int  CBusiness::GetComDataAsyn(DWORD dwDelay /*= 0*/) {
	if ( dwDelay == 0 )
		g_data.m_thrd_com->PostMessage(this, MSG_GET_COM_DATA);
	else
		g_data.m_thrd_com->PostDelayMessage( dwDelay, this, MSG_GET_COM_DATA);
	return 0;
}

int  CBusiness::GetComData() {
	char   achData[8192];
	DWORD  dwLen = sizeof(achData);

	BOOL bRet = m_serial_port.Read(achData, dwLen);
	if ( bRet && dwLen > 0 && g_data.m_hWnd != 0 ) {
		char * pNewData = new char[dwLen];
		memcpy(pNewData, achData, dwLen);
		::PostMessage(g_data.m_hWnd, UM_GET_COM_DATA_RET, (WPARAM)pNewData, dwLen);
	}
	
	GetComDataAsyn(1000);
	return 0;
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_OPEN_COM_PORT:
	{
		COpenComPortParam * pParam = (COpenComPortParam *)pMessageData;
		OpenComPort(pParam);
	}
	break;

	case MSG_CLOSE_COM_PORT:
	{
		CloseComPort();
	}
	break;

	case MSG_GET_COM_DATA:
	{
		GetComData();
	}
	break;

	default:
		break;
	}
}