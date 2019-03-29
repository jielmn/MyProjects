#include "business.h"
#include "LmnSerialPort.h"

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


int  CBusiness::SetHandReaderAsyn(int nChannel, int nAddr, int nCom) {
	g_data.m_thrd_com->PostMessage(this, MSG_SETTING_HAND_READER, new CSettingHandReaderParam(nChannel, nAddr, nCom) );
	return 0;
}

int  CBusiness::SetHandReader(const CSettingHandReaderParam * pParam) {
	CLmnSerialPort  serial_port;

	char  szComPort[32];
	char  write_data[256];
	DWORD dwWriteLen = 5;

	SNPRINTF(szComPort, sizeof(szComPort), "com%d", pParam->m_nCom);
	BOOL bRet = serial_port.OpenUartPort(szComPort);
	if (!bRet) {
		::PostMessage(g_data.m_hWnd, UM_SETTING_HAND_READER_RET, -1, 0);
		return 0;
	}

	//                     地址 信道
	memcpy(write_data, "\xC0\x01\x02\xDD\xAA", dwWriteLen);
	write_data[1] = (BYTE)pParam->m_nAddr;
	write_data[2] = (BYTE)pParam->m_nChannel;
	serial_port.Write(write_data, dwWriteLen);
	serial_port.CloseUartPort();

	::PostMessage(g_data.m_hWnd, UM_SETTING_HAND_READER_RET, 0, 0);
	return 0;
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_SETTING_HAND_READER:
	{
		CSettingHandReaderParam * pParam = (CSettingHandReaderParam *)pMessageData;
		SetHandReader(pParam);
	}
	break;

	default:
		break;
	}
}