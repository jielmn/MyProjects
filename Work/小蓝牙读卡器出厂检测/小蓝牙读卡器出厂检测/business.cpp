#include "business.h"

#define  RSP_TIME_LIMIT              20000               // 12秒内收到应答
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

	m_db.InitDb();

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

	ReadComDataAsyn();
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

	m_db.DeinitDb();
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

//#if TEST_FLAG
//		BOOL bRet1 = m_com.OpenUartPort(17);
//		::PostMessage(g_data.m_hWnd, UM_COM_STATUS, 17, bRet1);
//		return;
//#endif

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
				DisconnectBle();
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
	::PostMessage(g_data.m_hWnd, UM_INFO_MSG, ATING, 0);
	BOOL  bAtOk = DisconnectBle();
	::PostMessage(g_data.m_hWnd, UM_INFO_MSG, ATING_RET, bAtOk);

	if (!bAtOk) {
		::PostMessage(g_data.m_hWnd, UM_AUTO_TEST_FIN, RET_AT_FAILED, 0);
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

	CDataBuf buf;
	while (dwCurTick - dwLastTick < RSP_TIME_LIMIT && !g_data.m_bQuiting ) {
		dwRevLen = sizeof(rsp);

		if ( m_com.Read(rsp, dwRevLen) && dwRevLen > 0 ) {	
			buf.Append(rsp, dwRevLen);

			PrintComData(rsp, dwRevLen);

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
		::PostMessage(g_data.m_hWnd, UM_AUTO_TEST_FIN, RET_BLE_FAILED, 0);
		return;
	}
	
	// AT+NOTIFY_ON000F
	memcpy(data, "AT+NOTIFY_ON", 12);
	memcpy(data + 12, g_data.m_szNotifyCharId, 4);
	dwLen = 16;
	m_com.Write(data, dwLen);
	::PostMessage(g_data.m_hWnd, UM_INFO_MSG, SETTING_NOTIFYID, 0);

	BOOL bDataOk = FALSE;
	dwLastTick = LmnGetTickCount();
	dwCurTick = dwLastTick;
	buf.Clear();

	while (dwCurTick - dwLastTick < 1000) {
		dwRevLen = sizeof(rsp);
		if (m_com.Read(rsp, dwRevLen) && dwRevLen > 0) {
			PrintComData(rsp, dwRevLen);
			buf.Append(rsp, dwRevLen);

			if (buf.GetDataLength() == 12) {
				if (0 == strncmp((const char *)buf.GetData(), "OK+DATA-OK\r\n", 12)) {
					bDataOk = TRUE;
					break;
				}
				else {
					break;
				}
			}
			else if (buf.GetDataLength() > 12) {
				break;
			}
		}
		LmnSleep(100);
		dwCurTick = LmnGetTickCount();
	}

	::PostMessage(g_data.m_hWnd, UM_INFO_MSG, SETTING_NOTIFYID_RET, bDataOk);

	if (!bDataOk) {
		::PostMessage(g_data.m_hWnd, UM_AUTO_TEST_FIN, RET_NOTIFYID_FAILED, 0);
		return;
	}


	//// AT+SET_WAYWR000D
	//memcpy(data, "AT+SET_WAYWR", 12);
	//memcpy(data + 12, g_data.m_szWriteCharId, 4);
	//dwLen = 16;
	//m_com.Write(data, dwLen);

	//LmnSleep(1000);
	//memset(rsp, 0, sizeof(rsp));
	//dwRevLen = sizeof(rsp);
	//// "OK+SEND-OK\r\n"
	//m_com.Read(rsp, dwRevLen);


	// 41542B53454E445F44415441574E30303044C311   (十六进制下)   AT+SEND_DATAWN000D
	memcpy(data, "AT+SEND_DATAWN", 14);
	memcpy(data + 14, g_data.m_szWriteCharId, 4);
	memcpy(data + 18, g_data.m_adwTempCmd, 2);
	dwLen = 20;
	m_com.Write(data, dwLen);
	::PostMessage(g_data.m_hWnd, UM_INFO_MSG, TEMPING, 0);

	dwLastTick = LmnGetTickCount();
	dwCurTick = dwLastTick;
	BOOL  bGetTemp = FALSE;
	char  szTagId[20] = { 0 };
	DWORD  dwTemp = 0;
	buf.Clear();

	while ( dwCurTick - dwLastTick < RSP_TIME_LIMIT && !g_data.m_bQuiting ) {
		dwRevLen = sizeof(rsp);		

		if (m_com.Read(rsp, dwRevLen) && dwRevLen > 0) {
			buf.Append(rsp, dwRevLen);
			PrintComData(rsp, dwRevLen);
		}

		if ( buf.GetDataLength() >= 12 ) {
			buf.Read(rsp, 12);

			// 格式错误
			if ( !((BYTE)rsp[0] == 0xc0 && (BYTE)rsp[1] == 0x0a) ) {
				break;
			}

			bGetTemp = TRUE;
			dwTemp = (DWORD)( rsp[10] * 100 + rsp[11] );

			BYTE  abyTagId[8];
			for (int i = 0; i < 8; i++) {
				abyTagId[i] = (BYTE)rsp[9-i];
			}
			
			Bytes2String(szTagId, sizeof(szTagId), abyTagId, 8);

			TempItem * pItem = new TempItem;
			memset(pItem, 0, sizeof(TempItem));
			pItem->dwTemp = dwTemp;
			STRNCPY(pItem->szTagId, szTagId, sizeof(pItem->szTagId));
			::PostMessage(g_data.m_hWnd, UM_INFO_MSG, TEMP_RET, (LPARAM)pItem);

			break;
		}
		else if (buf.GetDataLength() == 2) {
			buf.Read(rsp, 2);
			bGetTemp = FALSE;
			break;
		}

		LmnSleep(SLEEP_TIME);
		dwCurTick = LmnGetTickCount();
	}

	if (!bGetTemp) {
		::PostMessage(g_data.m_hWnd, UM_INFO_MSG, TEMP_RET, 0);
		::PostMessage(g_data.m_hWnd, UM_AUTO_TEST_FIN, RET_TEMP_FAILED, 0);
		return;
	}
	
	::PostMessage(g_data.m_hWnd, UM_AUTO_TEST_FIN, 0, 0);	
}

void  CBusiness::StopAutoTestAsyn() {
	g_data.m_thrd_com->PostMessage(this, MSG_STOP_AUTO_TEST );
}

void  CBusiness::StopAutoTest() {
	DisconnectBle();
	::PostMessage(g_data.m_hWnd, UM_BLE_DISCONNECTED, 0, 0);
	::PostMessage(g_data.m_hWnd, UM_STOP_AUTO_TEST_RET, 0, 0);
}

void  CBusiness::PrintComData(void * pData, DWORD  dwDataLen) {
#if _DEBUG
	char szDebug[8094];
	DebugStream(szDebug, sizeof(szDebug), pData, dwDataLen);
	OutputDebugString(szDebug);
	OutputDebugString("\n");
#endif
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "\n%s\n", szDebug);
}

void  CBusiness::DisconnectBleAsyn() {
	g_data.m_thrd_com->PostMessage(this, MSG_DISCONNECT_BLE);
}

BOOL  CBusiness::DisconnectBle() {
	if (m_com.GetStatus() == CLmnSerialPort::CLOSE)
		return TRUE;

	char   data[256];
	DWORD  dwLen = 0;

	char   rsp[256];
	DWORD  dwRevLen = 0;

	memcpy(data, "AT", 2);
	dwLen = 2;
	m_com.Write(data, dwLen);

	DWORD  dwLastTick = LmnGetTickCount();
	DWORD  dwCurTick = dwLastTick;
	BOOL  bAtOk = FALSE;
	CDataBuf buf;

	while (dwCurTick - dwLastTick < 1000) {
		dwRevLen = sizeof(rsp);
		if (m_com.Read(rsp, dwRevLen) && dwRevLen > 0) {
			PrintComData(rsp, dwRevLen);
			buf.Append(rsp, dwRevLen);

			if ( buf.GetDataLength() == 9 ) {
				if ( 0 == strncmp( (const char *)buf.GetData(), "OK+LOST\r\n", 9) ) {
					bAtOk = TRUE;
					break;
				}
			}
			else if (buf.GetDataLength() == 2) {
				if (0 == strncmp((const char *)buf.GetData(), "OK", 2)) {
					bAtOk = TRUE;
					break;
				}
			}
		}
		LmnSleep(100);
		dwCurTick = LmnGetTickCount();
	}

	return bAtOk;
}

void  CBusiness::ReadComDataAsyn() {
	g_data.m_thrd_com->PostDelayMessage(1000, this, MSG_READ_COM_DATA);
}

void  CBusiness::ReadComData() {
	if (m_com.GetStatus() == CLmnSerialPort::CLOSE)
		return;

	char   rsp[256];
	DWORD  dwRevLen = 0;

	dwRevLen = sizeof(rsp);
	if ( m_com.Read(rsp, dwRevLen) && dwRevLen > 0 ) {
		PrintComData(rsp, dwRevLen);
		if ( 9 == dwRevLen && 0 == strncmp(rsp, "OK+LOST\r\n", 9 ) ) {
			::PostMessage(g_data.m_hWnd, UM_BLE_DISCONNECTED, 0, 0);
		}
	}

	ReadComDataAsyn();
}

void  CBusiness::MeasureTempAsyn() {
	g_data.m_thrd_com->PostMessage(this, MSG_MEASURE_TEMP);
}

void  CBusiness::MeasureTemp() {		
	char   data[256];
	DWORD  dwLen = 0;
	char   rsp[256];
	DWORD  dwRevLen = 0;

	memcpy(data, "AT+SEND_DATAWN", 14);
	memcpy(data + 14, g_data.m_szWriteCharId, 4);
	memcpy(data + 18, g_data.m_adwTempCmd, 2);
	dwLen = 20;
	m_com.Write(data, dwLen);
	::PostMessage(g_data.m_hWnd, UM_INFO_MSG, TEMPING, 0);

	DWORD dwLastTick = LmnGetTickCount();
	DWORD dwCurTick = dwLastTick;
	BOOL  bGetTemp = FALSE;
	char  szTagId[20] = { 0 };
	DWORD  dwTemp = 0;
	CDataBuf buf;

	while (dwCurTick - dwLastTick < RSP_TIME_LIMIT) {
		dwRevLen = sizeof(rsp);

		if (m_com.Read(rsp, dwRevLen) && dwRevLen > 0) {
			buf.Append(rsp, dwRevLen);
			PrintComData(rsp, dwRevLen);
		}

		if (buf.GetDataLength() >= 12) {
			buf.Read(rsp, 12);

			// 格式错误
			if (!((BYTE)rsp[0] == 0xc0 && (BYTE)rsp[1] == 0x0a)) {
				break;
			}

			bGetTemp = TRUE;
			dwTemp = (DWORD)(rsp[10] * 100 + rsp[11]);

			BYTE  abyTagId[8];
			for (int i = 0; i < 8; i++) {
				abyTagId[i] = (BYTE)rsp[9 - i];
			}

			Bytes2String(szTagId, sizeof(szTagId), abyTagId, 8);

			TempItem * pItem = new TempItem;
			memset(pItem, 0, sizeof(TempItem));
			pItem->dwTemp = dwTemp;
			STRNCPY(pItem->szTagId, szTagId, sizeof(pItem->szTagId));
			::PostMessage(g_data.m_hWnd, UM_INFO_MSG, TEMP_RET, (LPARAM)pItem);

			break;
		}
		else if (buf.GetDataLength() == 2) {
			buf.Read(rsp, 2);
			bGetTemp = FALSE;
			break;
		}

		LmnSleep(SLEEP_TIME);
		dwCurTick = LmnGetTickCount();
	}

	if (!bGetTemp) {
		::PostMessage(g_data.m_hWnd, UM_INFO_MSG, TEMP_RET, 0);
		::PostMessage(g_data.m_hWnd, UM_MEASURE_TEMP_FIN, FALSE, 0);
		return;
	}

	::PostMessage(g_data.m_hWnd, UM_MEASURE_TEMP_FIN, TRUE, 0);
}

void  CBusiness::SaveResultAsyn(const char * szReaderId, BOOL bPass, DWORD  dwFact) {
	g_data.m_thrd_db->PostMessage( this, MSG_SAVE_RESULT, new CSaveResultParam(szReaderId, bPass, dwFact) );
}

void  CBusiness::SaveResult(const CSaveResultParam * pParam) {
	m_db.SaveResult(pParam);
	::PostMessage(g_data.m_hWnd, UM_SAVE_RESULT_FIN, 0, 0);
}

void  CBusiness::QueryDataAsyn(time_t t1, time_t t2, const char * szMac) {
	g_data.m_thrd_db->PostMessage(this, MSG_QUERY_DATA, new CQueryDataParam(t1, t2, szMac) );
}

void  CBusiness::QueryData(const CQueryDataParam * pParam) {
	std::vector<ReaderItem*> * pvRet = new std::vector<ReaderItem*>;
	m_db.QueryData(pParam, *pvRet);
	int a = 100;
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

	case MSG_DISCONNECT_BLE:
	{
		DisconnectBle();
	}
	break;

	case MSG_READ_COM_DATA:
	{
		ReadComData();
	}
	break;

	case MSG_STOP_AUTO_TEST:
	{
		StopAutoTest();
	}
	break;

	case MSG_MEASURE_TEMP: 
	{
		MeasureTemp();
	}
	break;

	case MSG_SAVE_RESULT:
	{
		CSaveResultParam * pParam = (CSaveResultParam *)pMessageData;
		SaveResult(pParam);
	}
	break;

	case MSG_QUERY_DATA:
	{
		CQueryDataParam * pParam = (CQueryDataParam *)pMessageData;
		QueryData(pParam);
	}
	break;

	default:
		break;
	}
}