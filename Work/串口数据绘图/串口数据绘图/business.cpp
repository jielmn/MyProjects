#include "business.h"
#include "LmnTemplates.h"

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
	
	g_color.insert(std::make_pair(1, 0xFFCA5100));
	g_color.insert(std::make_pair(2, 0xFF1b9375));	
	g_color.insert(std::make_pair(3, 0xFF007ACC));
	g_color.insert(std::make_pair(4, 0xFF333333));

	for ( int i = 0; i < MAX_CHANNEL_COUNT; i++ ) {
		SetBit(g_data.m_dwChannels, i+1);
	}


	g_data.m_cfg = new FileConfigEx();
	if (0 == g_data.m_cfg) {
		return -1;
	}
	g_data.m_cfg->Init(CONFIG_FILE_NAME);

	g_data.m_thrd_com = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_com) {
		return -1;
	}
	g_data.m_thrd_com->Start();

	g_data.m_thrd_com->PostDelayMessage(200, this, MSG_READ_COM);

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

void  CBusiness::OpenComAsyn(int nCom, int nBaud) {
	g_data.m_thrd_com->PostMessage(this, MSG_OPEN_COM, new COpenComParam(nCom, nBaud) );
}

void  CBusiness::OpenCom(const COpenComParam * pParam) {
	BOOL bRet = m_com.OpenUartPort(pParam->m_nCom, pParam->m_nBaud);
	::PostMessage(g_data.m_hWnd, UM_OPEN_COM_RET, bRet, 0);
}

void CBusiness::CloseComAsyn() {
	g_data.m_thrd_com->PostMessage(this, MSG_CLOSE_COM);
}

void CBusiness::CloseCom() {
	m_com.CloseUartPort();
	::PostMessage(g_data.m_hWnd, UM_CLOSE_COM_RET, 0, 0);
}



// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_OPEN_COM:
	{
		COpenComParam * pParam = (COpenComParam *)pMessageData;
		OpenCom(pParam);
	}
	break;

	case MSG_READ_COM:
	{
		if ( m_com.GetStatus() == CLmnSerialPort::OPEN ) {
			BYTE  data[8192];
			DWORD  dwLen = sizeof(data);
			BOOL bRet = m_com.Read(data, dwLen);
			while (bRet && dwLen > 0) {
				BYTE * pData = new BYTE[dwLen];
				memcpy(pData, data, dwLen);
				::PostMessage(g_data.m_hWnd, UM_COM_DATA, (WPARAM)pData, dwLen);
				dwLen = sizeof(data);
				bRet = m_com.Read(data, dwLen);
			}
		}
		g_data.m_thrd_com->PostDelayMessage(200, this, MSG_READ_COM);
	}
	break;

	case MSG_CLOSE_COM:
	{
		CloseCom();
	}
	break;

	default:
		break;
	}
}