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

	if (g_cfg_area) {
		g_cfg_area->Deinit();
		delete g_cfg_area;
		g_cfg_area = 0;
	}

	ClearVector(g_vArea);
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

	g_cfg_area = new FileConfigEx();
	if (0 == g_cfg_area) {
		return -1;
	}
	g_cfg_area->Init("area.cfg");

	DuiLib::CDuiString strText;
	for (int i = 0; i < MAX_AREA_COUNT; i++) {
		TArea area;
		strText.Format("area name %d", i + 1);

		g_cfg_area->GetConfig(strText, area.szAreaName, sizeof(area.szAreaName), "");
		if (area.szAreaName[0] == '\0') {
			break;
		}

		strText.Format("area no %d", i + 1);
		g_cfg_area->GetConfig(strText, area.dwAreaNo, 0);
		if (0 == area.dwAreaNo || area.dwAreaNo > 100) {
			break;
		}

		TArea * pArea = new TArea;
		memcpy(pArea, &area, sizeof(TArea));
		g_vArea.push_back(pArea);
	}

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


int  CBusiness::SetHandReaderAsyn(BYTE byChannel, BYTE byAddr, int nCom) {
	g_data.m_thrd_com->PostMessage(this, MSG_SETTING_HAND_READER, new CSettingHandReaderParam(byChannel, byAddr, nCom) );
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
	write_data[1] = (BYTE)pParam->m_byAddr;
	write_data[2] = (BYTE)pParam->m_byChannel;
	serial_port.Write(write_data, dwWriteLen);
	LmnSleep(1000);

	dwWriteLen = sizeof(write_data);
	serial_port.Read(write_data, dwWriteLen);
	int ret = -1;
	if ( dwWriteLen == 2 && write_data[0] == 'O' && write_data[1] == 'K') {
		ret = 0;
	}
	serial_port.CloseUartPort();

	::PostMessage(g_data.m_hWnd, UM_SETTING_HAND_READER_RET, ret, 0);
	return 0;
}

int  CBusiness::SetHandReaderSnAsyn(DWORD  dwSn, int nCom) {
	g_data.m_thrd_com->PostMessage(this, MSG_SET_HAND_READER_SN, new CSetHandReaderSnParam(dwSn, nCom));
	return 0;
}

int  CBusiness::SetHandReaderSn(const CSetHandReaderSnParam * pParam) {
	CLmnSerialPort  serial_port;

	char  szComPort[32];
	char  write_data[256];
	DWORD dwWriteLen = 7;
	int   nRegMsg = UM_SET_HAND_READER_SN_RET;

	SNPRINTF(szComPort, sizeof(szComPort), "com%d", pParam->m_nCom);
	BOOL bRet = serial_port.OpenUartPort(szComPort);
	if (!bRet) {
		::PostMessage(g_data.m_hWnd, nRegMsg, -1, 0);
		return 0;
	}

	//                     地址 信道
	memcpy(write_data, "\x66\x00\x00\x00\x01\xDD\xAA", dwWriteLen);
	write_data[1] = (BYTE)(pParam->m_dwSn >> 24);
	write_data[2] = (BYTE)( (pParam->m_dwSn >> 16) & 0xFF );
	write_data[3] = (BYTE)( (pParam->m_dwSn >> 8) & 0xFF );
	write_data[4] = (BYTE)( pParam->m_dwSn & 0xFF );
	serial_port.Write(write_data, dwWriteLen);
	LmnSleep(1000);

	dwWriteLen = sizeof(write_data);
	serial_port.Read(write_data, dwWriteLen);
	int ret = -1;
	if (dwWriteLen == 2 && write_data[0] == 'O' && write_data[1] == 'K') {
		ret = 0;
	}
	serial_port.CloseUartPort();

	::PostMessage(g_data.m_hWnd, nRegMsg, ret, 0);
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

	case MSG_SET_HAND_READER_SN:
	{
		CSetHandReaderSnParam * pParam = (CSetHandReaderSnParam *)pMessageData;
		SetHandReaderSn(pParam);
	}
	break;

	default:
		break;
	}
}