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

	g_data.m_nMaxHeartBeat = g_data.m_cfg->GetInt("warning.max heart beat", 120);
	g_data.m_nMinHeartBeat = g_data.m_cfg->GetInt("warning.min heart beat", 60);
	g_data.m_nMinOxy = g_data.m_cfg->GetInt("warning.min blood oxygen", 95);
	g_data.m_nMaxTemp = g_data.m_cfg->GetInt("warning.max temperature", 3800);
	g_data.m_nMinTemp = g_data.m_cfg->GetInt("warning.max heart beat", 3500);

	m_sqlite.InitDb();

	g_data.m_thrd_com = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_com) {
		return -1;
	}
	g_data.m_thrd_com->Start();

	g_data.m_thrd_db = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_db) {
		return -1;
	}
	g_data.m_thrd_db->Start();

	return 0;
}

int CBusiness::DeInit() {
	g_data.m_cfg->Save();

	if (g_data.m_thrd_com) {
		g_data.m_thrd_com->Stop();
		delete g_data.m_thrd_com;
		g_data.m_thrd_com = 0;
	}

	if (g_data.m_thrd_db) {
		g_data.m_thrd_db->Stop();
		delete g_data.m_thrd_db;
		g_data.m_thrd_db = 0;
	}

	Clear();
	m_sqlite.DeinitDb();
	return 0;
}

void  CBusiness::ReconnectAsyn(BOOL bCloseFirst /*= FALSE*/) {

	if (!bCloseFirst) {
		// 如果已经连接串口
		if (m_com.GetStatus() == CLmnSerialPort::OPEN) {
			return;
		}
	}	

	if (g_data.m_nComPort <= 0)
		return;

	g_data.m_thrd_com->DeleteMessages();
	g_data.m_thrd_com->PostMessage(this, MSG_RECONNECT);
}

void  CBusiness::Reconnect() {
	m_com.CloseUartPort();
	m_buf.Clear();

	BOOL bRet = m_com.OpenUartPort(g_data.m_nComPort);
	::PostMessage(g_data.m_hWnd, UM_RECONNECT_RET, bRet, 0);
	if ( bRet ) {
		ReadDataAsyn();
	}
}

void  CBusiness::ReadDataAsyn(DWORD  dwDelay /*= 0*/) {
	if (dwDelay == 0) {
		g_data.m_thrd_com->PostMessage(this, MSG_READ_DATA);
	}
	else {
		g_data.m_thrd_com->PostDelayMessage( dwDelay, this, MSG_READ_DATA);
	}
}

void  CBusiness::ReadData() {
	BYTE   achData[8192];
	DWORD  dwLen = sizeof(achData);
	BOOL   bRet  = m_com.Read(achData, dwLen);
	// 读取所有的数据
	while (bRet && dwLen > 0) {
		m_buf.Append(achData, dwLen);
		dwLen = sizeof(achData);
		bRet = m_com.Read(achData, dwLen);
	}

	// 如果读取失败，串口断开
	if ( !bRet ) {
		m_com.CloseUartPort();
		m_buf.Clear();
		::PostMessage(g_data.m_hWnd, UM_RECONNECT_RET, FALSE, 0);
		return;
	}

	const DWORD  DATA_ITEM_LENGTH = 13;

	while ( m_buf.GetDataLength() >= DATA_ITEM_LENGTH ) {
		// 读取一项数据
		m_buf.Read( achData, DATA_ITEM_LENGTH );

		CWearItem * pItem = new CWearItem;

		if (achData[3] > 0) {
			pItem->m_nHearbeat = achData[2];
		}

		if (achData[5] > 0) {
			pItem->m_nOxy = achData[4];
		}

		pItem->m_nTemp = achData[6] * 100 + achData[7];
		pItem->m_nPose = achData[8];
		SNPRINTF(pItem->m_szDeviceId, sizeof(pItem->m_szDeviceId), "%d", (int)achData[10]);

		::PostMessage(g_data.m_hWnd, UM_DATA_ITEM, (WPARAM)pItem, 0);
	}
	m_buf.Reform();

	ReadDataAsyn(100);
}

void  CBusiness::GetDeviceUserNameAsyn(const char * szDeviceId) {
	g_data.m_thrd_db->PostMessage(this, MSG_GET_NAME, new CGetNameParam(szDeviceId) );
}

void  CBusiness::GetDeviceUserName(const CGetNameParam * pParam) {
	QueryNameRet * pRet = new QueryNameRet;
	STRNCPY(pRet->szDeviceId, pParam->m_szDeviceId, sizeof(pRet->szDeviceId));
	BOOL bRet = m_sqlite.GetDeviceUserName(pParam, pRet->szName, sizeof(pRet->szName));
	::PostMessage(g_data.m_hWnd, UM_GET_NAME_RET, bRet, (LPARAM)pRet);		
}

void  CBusiness::SaveDeviceUserNameAsyn(const char * szDeviceId, const char * szName) {
	g_data.m_thrd_db->PostMessage(this, MSG_SAVE_NAME, new CSaveNameParam(szDeviceId, szName));
}

void  CBusiness::SaveDeviceUserName(const CSaveNameParam * pParam) {
	m_sqlite.SaveDeviceUserName(pParam);
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

	case MSG_READ_DATA:
	{
		ReadData();
	}
	break;

	case MSG_GET_NAME:
	{
		CGetNameParam * pParam = (CGetNameParam *)pMessageData;
		GetDeviceUserName(pParam);
	}
	break;

	case MSG_SAVE_NAME:
	{
		CSaveNameParam * pParam = (CSaveNameParam *)pMessageData;
		SaveDeviceUserName(pParam);
	}
	break;

	default:
		break;
	}
}