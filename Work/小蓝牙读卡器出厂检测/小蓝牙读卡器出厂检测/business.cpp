#include "business.h"

#define  RSP_TIME_LIMIT              12000               // 12秒内收到应答
#define  SLEEP_TIME                  100

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {
	m_bBluetoothCnn = FALSE;
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
	g_data.m_cfg->GetConfig("mac type", dwValue, 0);
	g_data.m_byMacType = (BYTE)dwValue;

	g_data.m_cfg->GetConfig("notify id", g_data.m_szNotifyCharId, sizeof(g_data.m_szNotifyCharId), "000F");
	g_data.m_cfg->GetConfig("write id", g_data.m_szWriteCharId, sizeof(g_data.m_szWriteCharId), "000D");

	char  szTempCmd[256];
	g_data.m_cfg->GetConfig("temperature command", szTempCmd, sizeof(szTempCmd), "C311");
	DWORD  dwSize = sizeof(g_data.m_adwTempCmd);
	String2Bytes(g_data.m_adwTempCmd, dwSize, szTempCmd);

	g_data.m_thrd_db = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_db) {
		return -1;
	}
	g_data.m_thrd_db->Start();

	g_data.m_thrd_com = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_com) {
		return -1;
	}
	g_data.m_thrd_com->Start();

	return 0;
}

int CBusiness::DeInit() {

	if (g_data.m_thrd_db) {
		g_data.m_thrd_db->Stop();
		delete g_data.m_thrd_db;
		g_data.m_thrd_db = 0;
	}

	if (g_data.m_thrd_com) {
		g_data.m_thrd_com->Stop();
		delete g_data.m_thrd_com;
		g_data.m_thrd_com = 0;
	}

	Clear();
	return 0;
}

void  CBusiness::CheckDevicesAsyn() {
	g_data.m_thrd_com->PostMessage(this, MSG_CHECK_DEVICES);
}

void  CBusiness::CheckDevices() {
	// 如果是串口打开状态
	if ( m_com.GetStatus() == CLmnSerialPort::OPEN ) {
		std::map<int, std::string> m;
		GetAllSerialPorts(m);

		std::map<int, std::string>::iterator it = m.find( m_com.GetPort() );
		// 如果串口已经不存在
		if ( it == m.end() ) {
			m_com.CloseUartPort();
			::PostMessage(g_data.m_hWnd, UM_COM_STATUS, 0, 0);
		}
	}
	// 如果是串口关闭状态
	else {
		std::map<int, std::string> m;
		GetAllSerialPorts(m);
		DWORD  dwLen = strlen(COM_STRING);

		std::map<int, std::string>::iterator it;
		std::map<int, std::string>::iterator it_find;
		int nFindCount = 0;

		for (it = m.begin(); it != m.end(); ++it) {
			std::string & s = it->second;
			if ( 0 == StrNiCmp( COM_STRING, s.c_str(), dwLen ) ) {
				it_find = it;
				nFindCount++;
			}
		}

		if ( nFindCount == 0 ) {
			::PostMessage(g_data.m_hWnd, UM_COM_STATUS, 0, 0);
		}
		else if (nFindCount > 1) {
			::PostMessage(g_data.m_hWnd, UM_COM_STATUS, -1, 0);
		}
		// 如果找到了1个蓝牙模块
		else {
			BOOL bRet = m_com.OpenUartPort(it_find->first);
			::PostMessage(g_data.m_hWnd, UM_COM_STATUS, it_find->first, bRet);

			// 关闭之前的蓝牙连接
			if (bRet) {
				char   data[256];
				DWORD  dwLen = 0;
				char   rsp[256];
				DWORD  dwRevLen = 0;

				// AT命令
				memcpy(data, "AT", 2);
				dwLen = 2;
				m_com.Write(data, dwLen);
				LmnSleep(1000);

				BOOL  bAtOk = FALSE;
				memset(rsp, 0, sizeof(rsp));
				dwRevLen = sizeof(rsp);
				if (m_com.Read(rsp, dwRevLen) && dwRevLen > 0) {
					if (0 == strcmp("OK", rsp) || 0 == strcmp("OK+LOST", rsp)) {
						bAtOk = TRUE;
					}
				}

			}
		}
	}
}

// AT+CO018994455DDBB
// AT+NOTIFY_ON000F
// AT+SET_WAYWR000D
// 41542B53454E445F44415441574E30303044C311   (十六进制下)
void  CBusiness::StartAutoTestAsyn(const char * szMac) {
	g_data.m_thrd_com->PostMessage(this, MSG_AUTO_START_TEST, new CStartAutoTestParam(szMac));
}

void  CBusiness::StartAutoTest(const CStartAutoTestParam * pParam) {
	// 串口应该打开
	assert(m_com.GetStatus() == CLmnSerialPort::OPEN);
	// 蓝牙应该没有连上
	assert(!m_bBluetoothCnn);
	
	char   data[256];
	DWORD  dwLen = 0;

	char   rsp[256];
	DWORD  dwRevLen = 0;

	// AT命令
	memcpy(data, "AT", 2);
	dwLen = 2;
	m_com.Write(data, dwLen);
	LmnSleep(1000);

	BOOL  bAtOk = FALSE;
	memset(rsp, 0, sizeof(rsp));
	dwRevLen = sizeof(rsp);
	if (m_com.Read(rsp, dwRevLen) && dwRevLen > 0) {
		if ( 0 == strcmp("OK", rsp) || 0 == strcmp("OK+LOST",rsp) ) {
			bAtOk = TRUE;
		}
	}

	if (!bAtOk) {
		::PostMessage(g_data.m_hWnd, UM_STOP_AUTO_TEST_RET, 0, 0);
		return;
	}


	memcpy(data, "AT+CO", 5);
	data[5] = g_data.m_byMacType;
	memcpy(data + 6, pParam->m_szMac, 12);
	dwLen = 18;

	m_com.Write(data, dwLen);

	DWORD  dwLastTick = LmnGetTickCount();
	DWORD  dwCurTick = dwLastTick;
	
	BOOL   bConnected = FALSE;
	InfoType  infoType = CONNECTING;

	while (dwCurTick - dwLastTick < RSP_TIME_LIMIT) {
		dwRevLen = sizeof(rsp);
		memset(rsp, 0, sizeof(rsp));

		CDataBuf buf;
		if ( m_com.Read(rsp, dwRevLen) && dwRevLen > 0 ) {	
			buf.Append(rsp, dwRevLen);

			while ( TRUE ) {
				if (infoType == CONNECTING) {
					if (buf.GetDataLength() >= 8) {
						buf.Read(rsp, 8);
						rsp[8] = '\0';
						buf.Reform();

						if (0 == strcmp(rsp, "OK+CONNA")) {
							::PostMessage(g_data.m_hWnd, UM_INFO_MSG, CONNECTING, 0);
							infoType = CONNECTING_1;
						}
						else {
							infoType = CNN_FAILED;
							break;
						}
					}
					else {
						break;
					}
				}
				else if (infoType == CONNECTING_1) {
					if (buf.GetDataLength() >= 10) {
						// OK+CONNF\r\n
						// memset(rsp, 0, sizeof(rsp));
						// buf.Read(rsp);
						infoType = CNN_FAILED;
						break;
					}
					else if (buf.GetDataLength() >= 9) {
						buf.Read(rsp, 9);
						rsp[9] = '\0';
						StrTrim(rsp);

						if (0 == strcmp(rsp, "OK+CONN")) {
							bConnected = TRUE;
							infoType = CNN_OK;
						}
						else {
							infoType = CNN_FAILED;
						}
						break;
					}
					else {
						break;
					}
				}
				else {
					break;
				}
			}	

			if (infoType == CNN_FAILED || infoType == CNN_OK) {
				break;
			}
		}

		LmnSleep(SLEEP_TIME);
		dwCurTick = LmnGetTickCount();
	}

	::PostMessage(g_data.m_hWnd, UM_BLUETOOTH_CNN_RET, bConnected, 0);

	if ( !bConnected ) {
		::PostMessage(g_data.m_hWnd, UM_STOP_AUTO_TEST_RET, 0, 0);
		return;
	}
	
}

void  CBusiness::StopAutoTestAsyn() {

}

void  CBusiness::StopAutoTest() {

}



// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_CHECK_DEVICES:
	{
		CheckDevices();
	}
	break;

	case MSG_AUTO_START_TEST:
	{
		CStartAutoTestParam * pParam = (CStartAutoTestParam *)pMessageData;
		StartAutoTest(pParam);
	}
	break;

	default:
		break;
	}
}