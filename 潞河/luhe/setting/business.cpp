#include "business.h"
#include "UIlib.h"
using namespace DuiLib;
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

int  CBusiness::SettingGwAsyn(int nAreaNo, int nComPort) {
	g_data.m_thrd_db->PostMessage(this, MSG_SETTING_GW, new CSettingGwParam(nAreaNo, nComPort));
	return 0;
}

int  CBusiness::SettingGw(const CSettingGwParam * pParam) {
	CLmnSerialPort  serial_port;

	char  szComPort[32];
	char  write_data[256];
	DWORD dwWriteLen = 7;
	int ret = -1;

	SNPRINTF(szComPort, sizeof(szComPort), "com%d", pParam->m_nComPort);
	BOOL bRet = serial_port.OpenUartPort(szComPort);
	if (!bRet) {
		::PostMessage(g_hWnd, UM_SETTTING_GW_RET, -1, 0);
		return 0;
	}

	memcpy(write_data, "\xC0\x00\x01\x1A\x04\xDD\xAA", dwWriteLen);
	write_data[4] = pParam->m_nAreaNo;
	serial_port.Write(write_data, dwWriteLen);

	LmnSleep(2000);

	BYTE buf[256];
	DWORD  dwRecvLen = sizeof(buf);
	serial_port.Read(buf, dwRecvLen);
	if (dwRecvLen == 3) {
		if ( buf[0] == 0x55 && buf[1] == (BYTE)pParam->m_nAreaNo && buf[2] == 0xFF ) {
			ret = 0;
		}
	}
	serial_port.CloseUartPort();

	::PostMessage(g_hWnd, UM_SETTTING_GW_RET, ret, 0);
	return 0;
}

int  CBusiness::SettingReaderAsyn(int nAreaNo, int nBedNo, int nComPort) {
	g_data.m_thrd_db->PostMessage(this, MSG_SETTING_READER, new CSettingReaderParam(nAreaNo, nBedNo, nComPort));
	return 0;
}

int  CBusiness::SettingReader(const CSettingReaderParam * pParam) {
	CLmnSerialPort  serial_port;

	char  szComPort[32];
	BYTE  write_data[256];
	DWORD dwWriteLen = 6;
	int ret = -1;

	SNPRINTF(szComPort, sizeof(szComPort), "com%d", pParam->m_nComPort);
	BOOL bRet = serial_port.OpenUartPort(szComPort);
	if (!bRet) {
		::PostMessage(g_hWnd, UM_SETTTING_READER_RET, -1, 0);
		return 0;
	}

	memcpy(write_data, "\xC0\x00\x01\x04\xDD\xAA", dwWriteLen);
	write_data[2] = pParam->m_nBedNo & 0xFF;
	write_data[3] = pParam->m_nAreaNo;
	serial_port.Write(write_data, dwWriteLen);

	LmnSleep(2000);

	BYTE buf[256];
	DWORD  dwRecvLen = sizeof(buf);
	serial_port.Read(buf, dwRecvLen);
	if (dwRecvLen == 4) {
		if (buf[0] == 0 && buf[1] == (pParam->m_nBedNo & 0xFF) && buf[2] == pParam->m_nAreaNo && buf[3] == 0xFF ) {
			ret = 0;
		}
	}
	serial_port.CloseUartPort();

	::PostMessage(g_hWnd, UM_SETTTING_READER_RET, ret, 0);
	return 0;
}

int  CBusiness::SettingSnAsyn(int nSn, int nComPort) {
	g_data.m_thrd_db->PostMessage( this, MSG_SETTING_SN, new CSettingSnParam(nSn, nComPort) );
	return 0;
}

int  CBusiness::SettingSn(const CSettingSnParam * pParam) {
	CLmnSerialPort  serial_port;

	char  szComPort[32];
	BYTE  write_data[256];
	DWORD dwWriteLen = 9;
	int ret = -1;

	SNPRINTF(szComPort, sizeof(szComPort), "com%d", pParam->m_nComPort);
	BOOL bRet = serial_port.OpenUartPort(szComPort);
	if (!bRet) {
		::PostMessage(g_hWnd, UM_SETTTING_SN_RET, -1, 0);
		return 0;
	}
	
	memcpy(write_data, "\x66\x00\x00\x00\x00\x00\x01\xDD\xAA", dwWriteLen);
	long n = htonl(pParam->m_nSn);
	memcpy(write_data + 3, &n, 4);
	serial_port.Write(write_data, dwWriteLen);

	LmnSleep(2000);

	BYTE buf[256];
	DWORD  dwRecvLen = sizeof(buf);
	serial_port.Read(buf, dwRecvLen);
	if (dwRecvLen == 13) {
		if ( buf[0] == 0x55 && buf[1] == 0x45 && buf[2] == 0x52 && buf[3] == 0x00 && buf[4] == 0x00 && buf[5] == 0x04
			&& buf[6] == 0x00 && buf[7] == 0x00 ) {
			DWORD  d = buf[11] + buf[10] * 256 + buf[9] * 256 * 256 + buf[8] * 256 * 256 * 256;
			if ( d == pParam->m_nSn &&  buf[12] == 0xFF ) {
				ret = 0;
			}
		}
	}
	serial_port.CloseUartPort();

	::PostMessage(g_hWnd, UM_SETTTING_SN_RET, ret, 0);
	return 0;
}



// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_SETTING_GW:
	{
		CSettingGwParam * pParam = (CSettingGwParam *)pMessageData;
		SettingGw(pParam);
	}
	break;

	case MSG_SETTING_READER:
	{
		CSettingReaderParam * pParam = (CSettingReaderParam *)pMessageData;
		SettingReader(pParam);
	}
	break;

	case MSG_SETTING_SN:
	{
		CSettingSnParam * pParam = (CSettingSnParam *)pMessageData;
		SettingSn(pParam);
	}
	break;

	default:
		break;
	}
}