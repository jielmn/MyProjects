#include "business.h"
#include "LmnTelSvr.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {
	m_launch.m_sigReconnect.connect(this, &CBusiness::OnReconnectLaunch);
	m_launch.m_sigStatus.connect(this, &CBusiness::OnLaunchStatus);
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

	if (g_data.m_update_cfg) {
		g_data.m_update_cfg->Deinit();
		delete g_data.m_update_cfg;
		g_data.m_update_cfg = 0;
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

	g_data.m_update_cfg = new FileConfigEx();
	if (0 == g_data.m_update_cfg) {
		return -1;
	}
	g_data.m_update_cfg->Init(UPDATE_FILE_NAME);

	g_data.m_update_cfg->GetConfig("server address", g_data.m_szServerAddr, sizeof(g_data.m_szServerAddr), "");
	DWORD dwLen = strlen(g_data.m_szServerAddr);
	if (dwLen > 0 && (g_data.m_szServerAddr[dwLen - 1] == '/' || g_data.m_szServerAddr[dwLen - 1] == '\\')) {
		g_data.m_szServerAddr[dwLen - 1] = '\0';
	}

	g_data.m_cfg->GetBooleanConfig("binding reader", g_data.m_bBindingReader);


	g_data.m_thrd_com = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_com) {
		return -1;
	}
	g_data.m_thrd_com->Start();

	ReconnectLauncherAsyn();

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

int  CBusiness::ReconnectLauncherAsyn(DWORD  dwDelay /*= 0*/) {
	if (0 == dwDelay) {
		g_data.m_thrd_com->PostMessage(this, MSG_RECONNECT_LAUNCH, 0, TRUE);
	}
	else {
		g_data.m_thrd_com->PostDelayMessage(dwDelay, this, MSG_RECONNECT_LAUNCH, 0, TRUE);
	}
	return 0;
}

int  CBusiness::ReconnectLauncher() {
	JTelSvrPrint("reconnect launch");
	m_launch.Reconnect();
	return 0;
}

int  CBusiness::PrintStatusAsyn(DWORD  dwDelay /*= 0*/) {
	if (0 == dwDelay) {
		g_data.m_thrd_com->PostMessage(this, MSG_PRINT_STATUS, 0, TRUE);
	}
	else {
		g_data.m_thrd_com->PostDelayMessage(dwDelay, this, MSG_PRINT_STATUS, 0, TRUE);
	}
	return 0;
}

int  CBusiness::PrintStatus() {
	DWORD  dwCfgLaunchPort = 0;
	g_data.m_cfg->GetConfig("launch port", dwCfgLaunchPort, 0);
	JTelSvrPrint("launch port = %lu", dwCfgLaunchPort);
	JTelSvrPrint("server addr = %s", g_data.m_szServerAddr);	
	JTelSvrPrint("binding reader = %s", g_data.m_bBindingReader ? "true" : "false" );
	JTelSvrPrint("launch status: %s", m_launch.GetStatus() == CLmnSerialPort::OPEN ? "open" : "close"  );
	
	return 0;
}

int  CBusiness::GetStationDataAsyn(DWORD  dwDelay /*= 0*/) {
	if (0 == dwDelay) {
		g_data.m_thrd_com->PostMessage(this, MSG_GET_STATION_DATA, 0, TRUE);
	}
	else {
		g_data.m_thrd_com->PostDelayMessage(dwDelay, this, MSG_GET_STATION_DATA, 0, TRUE);
	}
	return 0;
}

int  CBusiness::GetStationData() {
	m_launch.GetData();
	GetStationDataAsyn();
	return 0;
}

void  CBusiness::OnLaunchStatus(CLmnSerialPort::PortStatus s) {
	if ( s == CLmnSerialPort::OPEN ) {
		GetStationDataAsyn();
	}
}

void  CBusiness::OnReconnectLaunch(DWORD dwDelay) {
	ReconnectLauncherAsyn(dwDelay);
}



// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_RECONNECT_LAUNCH:
	{
		ReconnectLauncher();
	}
	break;

	case MSG_PRINT_STATUS: 
	{
		PrintStatus();
	}
	break;

	case MSG_GET_STATION_DATA:
	{
		GetStationData();
	}
	break;

	default:
		break;
	}
}