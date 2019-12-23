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
		g_data.m_cfg->DeInit();
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

	g_data.m_cfg = new CLuaCfg;
	if (0 == g_data.m_cfg) {
		return -1;
	}
	g_data.m_cfg->Init(CONFIG_FILE_NAME);

	const char * str = 0;
	int nValue = 0;
	int ret = 0;
	char buf[1024];

	str = g_data.m_cfg->GetString("serial port");
	Str2Lower(str, buf, sizeof(buf));
	ret = sscanf( buf, "com%d", &nValue );
	if (1 == ret) {
		g_data.m_nComPort = nValue;
	}

	g_data.m_thrd_com = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_com) {
		return -1;
	}
	g_data.m_thrd_com->Start();

	return 0;
}

int CBusiness::DeInit() {
	g_data.m_cfg->Save();

	if (g_data.m_thrd_com) {
		g_data.m_thrd_com->Stop();
		delete g_data.m_thrd_com;
		g_data.m_thrd_com = 0;
	}

	Clear();
	return 0;
}

void  CBusiness::ReconnectAsyn(BOOL bCloseFirst /*= FALSE*/) {

	if (!bCloseFirst) {
		// 如果已经连接串口
		if (m_com.GetStatus() == CLmnSerialPort::OPEN) {
			return;
		}
	}	

	g_data.m_thrd_com->DeleteMessages();
	g_data.m_thrd_com->PostMessage(this, MSG_RECONNECT);
}

void  CBusiness::Reconnect() {
	m_com.CloseUartPort();
	BOOL bRet = m_com.OpenUartPort(g_data.m_nComPort);
	::PostMessage(g_data.m_hWnd, UM_RECONNECT_RET, bRet, 0);
}



// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_RECONNECT:
	{
		Reconnect();
	}
	break;

	default:
		break;
	}
}