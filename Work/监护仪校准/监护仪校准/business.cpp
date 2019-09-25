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

	g_data.m_cfg->GetConfig("sleep time", g_data.m_dwSleepTime, 1000);

	BOOL bRet = LoadStandardRes();
	assert(bRet);

	g_data.m_thrd_db = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_db) {
		return -1;
	}
	g_data.m_thrd_db->Start();

	return 0;
}

int CBusiness::DeInit() {

	if (g_data.m_thrd_db) {
		g_data.m_bQuit = TRUE;
		g_data.m_thrd_db->Stop();
		delete g_data.m_thrd_db;
		g_data.m_thrd_db = 0;
	}

	Clear();
	return 0;
}

void  CBusiness::AdjustAsyn(int nComPort, int nTemp, int nDutyCycle) {
	g_data.m_thrd_db->PostMessage(this, MSG_ADJUST, new	CAdjustParam(nComPort, nTemp, nDutyCycle));
}

void  CBusiness::Adjust(const CAdjustParam * pParam) {
	CLmnSerialPort  serial_port;

	char  szComPort[32];
	char  write_data[256];
	DWORD dwWriteLen = 8;
	int   nRegMsg = UM_ADJUST_RET;

	SNPRINTF(szComPort, sizeof(szComPort), "com%d", pParam->m_nComPort);
	BOOL bRet = serial_port.OpenUartPort(szComPort);
	if (!bRet) {
		::PostMessage(g_data.m_hWnd, nRegMsg, -1, 0);
		return;
	}

	memcpy(write_data, "\x66\x20\x01\x04\xB0\xFF\xDD\xAA", dwWriteLen);
	write_data[1] = pParam->m_nTemp / 100;
	write_data[2] = (pParam->m_nTemp / 10) % 10;
	write_data[3] = ( pParam->m_nDutyCycle >> 8 ) & 0xFF;
	write_data[4] = pParam->m_nDutyCycle & 0xFF;
	serial_port.Write(write_data, dwWriteLen);
	serial_port.CloseUartPort();

	::PostMessage(g_data.m_hWnd, nRegMsg, 0, 0);
}

void  CBusiness::AdjustAllAsyn(int nComPort, MachineType eType, TempAdjust * items, DWORD dwSize) {
	g_data.m_thrd_db->PostMessage(this, MSG_ADJUST_ALL, 
		new	CAdjustAllParam(nComPort, eType, items, dwSize));
}

void  CBusiness::AdjustAll(const CAdjustAllParam * pParam) {
	CLmnSerialPort  serial_port;

	char  szComPort[32];
	char  write_data[256];
	DWORD dwWriteLen = 8;
	int   nRegMsg = UM_ADJUST_ALL_RET;

	SNPRINTF(szComPort, sizeof(szComPort), "com%d", pParam->m_nComPort);
	BOOL bRet = serial_port.OpenUartPort(szComPort);
	if (!bRet) {
		::PostMessage(g_data.m_hWnd, nRegMsg, -1, 0);
		return;
	}

	for (DWORD i = 0; i < pParam->m_dwSize; i++) {
		dwWriteLen = 8;
		memcpy(write_data, "\x55\x01\x02\x03\xDD\xFF\xDD\xAA", dwWriteLen);
		// write_data[0] = 0x55;
		write_data[1] = (int)pParam->m_eType + 1;
		write_data[2] = pParam->m_items[i].m_nTemp / 100;
		write_data[3] = (pParam->m_items[i].m_nTemp / 10) % 10;
		write_data[4] = pParam->m_items[i].m_nOffset;		
		serial_port.Write(write_data, dwWriteLen);
		LmnSleep(g_data.m_dwSleepTime);

#ifndef _DEBUG
		dwWriteLen = sizeof(write_data);
		BOOL bRet = serial_port.Read(write_data, dwWriteLen);
		// 没有得到OK回应
		if ( !(dwWriteLen == 2 && write_data[0] == 'O' && write_data[1] == 'K') ) {
			serial_port.CloseUartPort();
			::PostMessage(g_data.m_hWnd, nRegMsg, -1, 0);
			return;
		}
#endif

		// 如果当前应用程序正在退出
		if (g_data.m_bQuit)
			break;
	}
	
	serial_port.CloseUartPort();
	::PostMessage(g_data.m_hWnd, nRegMsg, 0, 0);
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_ADJUST:
	{
		CAdjustParam * pParam = (CAdjustParam *)pMessageData;
		Adjust(pParam);
	}
	break;

	case MSG_ADJUST_ALL:
	{
		CAdjustAllParam * pParam = (CAdjustAllParam *)pMessageData;
		AdjustAll(pParam);
	}
	break;

	default:
		break;
	}
}