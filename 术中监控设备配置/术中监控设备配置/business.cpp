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
	if (g_log) {
		g_log->Deinit();
		delete g_log;
		g_log = 0;
	}

	if (g_cfg) {
		g_cfg->Deinit();
		delete g_cfg;
		g_cfg = 0;
	}

	if (g_cfg_area) {
		g_cfg_area->Deinit();
		delete g_cfg_area;
		g_cfg_area = 0;
	}

	ClearVector(g_vArea);
}

int CBusiness::Init() {
	g_log = new FileLog();
	if (0 == g_log) {
		return -1;
	}
	g_log->Init(LOG_FILE_NAME);

	g_cfg = new FileConfigEx();
	if (0 == g_cfg) {
		return -1;
	} 
	g_cfg->Init(CONFIG_FILE_NAME);

	g_cfg_area = new FileConfigEx();
	if (0 == g_cfg_area) {
		return -1;
	}
	g_cfg_area->Init("area.cfg");

	DuiLib::CDuiString strText;
	for (int i = 0; i < MAX_AREA_COUNT; i++) {
		TArea area;
		strText.Format("area name %d", i + 1);

		g_cfg_area->GetConfig( strText, area.szAreaName, sizeof(area.szAreaName), "" );
		if (area.szAreaName[0] == '\0') {
			break;
		}

		strText.Format("area no %d", i + 1);
		g_cfg_area->GetConfig(strText, area.dwAreaNo, 0 );
		if (0 == area.dwAreaNo || area.dwAreaNo > 100 ) {
			break;
		}

		TArea * pArea = new TArea;
		memcpy(pArea, &area, sizeof(TArea));
		g_vArea.push_back(pArea);
	}
	



	g_thrd_worker = new LmnToolkits::Thread();
	if (0 == g_thrd_worker) {
		return -1;
	}
	g_thrd_worker->Start();

	return 0;
}

int CBusiness::DeInit() {

	if (g_thrd_worker) {
		g_thrd_worker->Stop();
		delete g_thrd_worker;
		g_thrd_worker = 0;
	}

	Clear();
	return 0;
}

int  CBusiness::SettingReaderAsyn(int nAreaNo, int nBedNo, int nComPort) {
	g_thrd_worker->PostMessage(this, MSG_SETTING_READER, new CSettingReaderParam(nAreaNo, nBedNo, nComPort));
	return 0;
}

int  CBusiness::SettingReader(const CSettingReaderParam * pParam) {
	CLmnSerialPort  serial_port;

	char  szComPort[32];
	char  write_data[256];
	DWORD dwWriteLen = 7;

	SNPRINTF(szComPort, sizeof(szComPort), "com%d", pParam->m_nComPort);
	BOOL bRet = serial_port.OpenUartPort(szComPort);
	if (!bRet) {
		::PostMessage(g_hWnd, UM_SETTTING_READER_RET, -1, 0);
		return 0;
	}

	memcpy(write_data, "\xC0\x00\x00\x00\xC0\xDD\xAA", dwWriteLen);
	write_data[1] = (pParam->m_nBedNo >> 8) & 0xFF;
	write_data[2] = pParam->m_nBedNo & 0xFF;
	write_data[3] = pParam->m_nAreaNo;
	serial_port.Write(write_data, dwWriteLen);
	serial_port.CloseUartPort();

	::PostMessage(g_hWnd, UM_SETTTING_READER_RET, 0, 0);
	return 0;
}

int  CBusiness::SettingGwAsyn(int nAreaNo, int nComPort) {
	g_thrd_worker->PostMessage(this, MSG_SETTING_GW, new CSettingGwParam(nAreaNo, nComPort));
	return 0;
}

int  CBusiness::SettingGw(const CSettingGwParam * pParam) {
	CLmnSerialPort  serial_port;

	char  szComPort[32];
	char  write_data[256];
	DWORD dwWriteLen = 6;
	int ret = -1;

	SNPRINTF(szComPort, sizeof(szComPort), "com%d", pParam->m_nComPort);
	BOOL bRet = serial_port.OpenUartPort(szComPort);
	if (!bRet) {
		::PostMessage(g_hWnd, UM_SETTTING_GW_RET, -1, 0);
		return 0;
	}

	memcpy(write_data, "\xC0\x10\x00\x1A\x00\xC0", dwWriteLen);
	write_data[4] = pParam->m_nAreaNo;
	serial_port.Write(write_data, dwWriteLen);

	LmnSleep(1000);

	char buf[256];
	DWORD  dwRecvLen = sizeof(buf);
	serial_port.Read(buf, dwRecvLen);
	if (dwRecvLen == 6) {
		if ( 0 == memcmp(buf, write_data, dwRecvLen) ) {
			ret = 0;
		}
	}
	serial_port.CloseUartPort();

	::PostMessage(g_hWnd, UM_SETTTING_GW_RET, ret, 0);
	return 0;
}

int  CBusiness::QueryGwAsyn(int nComPort) {
	g_thrd_worker->PostMessage(this, MSG_QUERY_GW, new CQueryGwParam(nComPort));
	return 0;
}

int  CBusiness::QueryGw(const CQueryGwParam * pParam) {
	CLmnSerialPort  serial_port;

	char  szComPort[32];
	char  write_data[256];
	DWORD dwWriteLen = 3;
	int ret = -1;

	SNPRINTF(szComPort, sizeof(szComPort), "com%d", pParam->m_nComPort);
	BOOL bRet = serial_port.OpenUartPort(szComPort);    
	if (!bRet) {
		::PostMessage(g_hWnd, UM_QUERY_GW_RET, -1, 0);
		return 0;
	}

	memcpy(write_data, "\xC1\xC1\xC1", dwWriteLen);
	serial_port.Write(write_data, dwWriteLen);

	LmnSleep(1000);

	char buf[256];
	DWORD  dwRecvLen = sizeof(buf);
	serial_port.Read(buf, dwRecvLen);
	if (dwRecvLen == 6) {
		ret = 0;
	}
	serial_port.CloseUartPort();

	::PostMessage(g_hWnd, UM_QUERY_GW_RET, ret, buf[4] );
	return 0;
}


// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_SETTING_READER:
	{
		CSettingReaderParam * pParam = (CSettingReaderParam *)pMessageData;
		SettingReader(pParam);
	}
	break;

	case MSG_SETTING_GW:
	{
		CSettingGwParam * pParam = (CSettingGwParam *)pMessageData;
		SettingGw(pParam);
	}
	break;

	case MSG_QUERY_GW:
	{
		CQueryGwParam * pParam = (CQueryGwParam *)pMessageData;
		QueryGw(pParam);
	}
	break;

	default:
		break;
	}
}