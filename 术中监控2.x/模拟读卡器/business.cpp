#include "business.h"

CBusiness * CBusiness::pInstance = 0;

CBusiness *  CBusiness::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CBusiness;
	}
	return pInstance;
}

CBusiness::CBusiness() {
	memset(m_arrTemp, 0, sizeof(m_arrTemp));
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
	g_data.m_cfg->GetConfig("com port", dwValue, 3);
	g_data.m_nComPort = dwValue;	

	g_data.m_thrd_launch = new LmnToolkits::Thread();
	if (0 == g_data.m_thrd_launch) {
		return -1;
	}
	g_data.m_thrd_launch->Start();   

	return 0;
}

int CBusiness::DeInit() {

	if (g_data.m_thrd_launch) {
		g_data.m_thrd_launch->Stop();
		delete g_data.m_thrd_launch;
		g_data.m_thrd_launch = 0;
	}

	Clear();
	return 0;
}


void CBusiness::ConnectComPortAsyn() {
	g_data.m_thrd_launch->PostMessage(this,MSG_CONNECT_COM);
}

void  CBusiness::ConnectComPort() {
	InitRand(TRUE, 1);

	if (g_data.m_nComPort > 0) {
		char szComPort[256];
		SNPRINTF(szComPort, sizeof(szComPort), "com%d", g_data.m_nComPort);
		BOOL bRet = m_serial_port.OpenUartPort(szComPort);
		if (bRet) {
			::PostMessage(g_data.m_hWnd, UM_STATUS, TRUE, 0);

			// 清除缓存的数据
			BYTE  buf[8192];
			DWORD  dwRecv = sizeof(buf);
			m_serial_port.Read(buf, dwRecv);
			while (dwRecv > 0) {
				dwRecv = sizeof(buf);
				m_serial_port.Read(buf, dwRecv);
			}

			m_buf.Clear();
			ReadDataAsyn(1000);
		}
	}
}

void  CBusiness::ReadDataAsyn(DWORD  dwDelay /*= 0*/) {
	if ( dwDelay > 0 )
		g_data.m_thrd_launch->PostDelayMessage( dwDelay, this, MSG_READ_DATA);
	else 
		g_data.m_thrd_launch->PostMessage(this, MSG_READ_DATA);
}

void  CBusiness::ReadData() {
	BYTE   buf[8192];
	DWORD  dwRecv = sizeof(buf);
	m_serial_port.Read(buf, dwRecv);
	while (dwRecv > 0) {
		m_buf.Append(buf, dwRecv);
		dwRecv = sizeof(buf);
		m_serial_port.Read(buf, dwRecv);
	}

	if ( m_buf.GetDataLength() < 8 ) {
		ReadDataAsyn(1000);
		return;
	}

	m_buf.Read(buf, 8);
	BYTE  * pNewData = new BYTE[8];
	memcpy(pNewData, buf, 8);
	::PostMessage(g_data.m_hWnd, UM_READ_DATA, (WPARAM)pNewData, 8);

	// 如果打开了术中开关
	if ( g_data.m_bSurgery ) {
		WORD wBed   = (buf[0] << 8) | buf[1];
		BYTE byArea = buf[2];

		// 如果床位号小于180(30床)
		if ( wBed > 0 && wBed <= MAX_READERS_PER_GRID * MAX_GRID ) {
			BYTE  bySend[256] = { 0 };
			memcpy(bySend, "\x55\x1A\x00\x06\x01\x45\x52\x00\x00\x03\x00\x00\x00\x00\x00\x01\x8F\x50\xD9\x93\xCD\x59\x02\xE0\x02\x07\x08\x05\xFF", 29);

			bySend[2] = buf[0];
			bySend[3] = buf[1];
			bySend[4] = buf[2];

			DWORD  dwWriteLen = 29;
			// 如果未初始化温度
			if ( m_arrTemp[wBed - 1] == 0 ) {
				m_arrTemp[wBed - 1] = GetRand(3200, 4080);
			}
			// 已经初始化温度
			else {
				DWORD a = GetRand(1, 100);
				DWORD b = GetRand(0, 20);
				if ( a > 50 ) {
					m_arrTemp[wBed - 1] += b;
				}
				else {
					m_arrTemp[wBed - 1] -= b;
				}

				if (m_arrTemp[wBed - 1] > 4080) {
					m_arrTemp[wBed - 1] = 4080;
				}
				else if (m_arrTemp[wBed - 1] < 3200) {
					m_arrTemp[wBed - 1] = 3200;
				}
			}
			DWORD dwTemp = m_arrTemp[wBed - 1];

			bySend[24] = (BYTE)(dwTemp / 1000);
			bySend[25] = (BYTE)((dwTemp / 100) % 10);
			bySend[26] = (BYTE)((dwTemp / 10) % 10);
			bySend[27] = (BYTE)(dwTemp % 10);

			memcpy(bySend + 5, "\x45\x52\x00\x00\x04\x00\x00\x00\x00\x00\x01", 11);
			bySend[13] = wBed / 100;
			bySend[14] = ( wBed / 10 ) % 10;
			bySend[15] = wBed % 10 ;

			memcpy(bySend + 16, "\x01\x00\x00\x00\x00\x00\x29\xe0", 8);
			bySend[16] = buf[1];
			bySend[17] = buf[0];

			m_serial_port.Write(bySend, dwWriteLen);

			pNewData = new BYTE[dwWriteLen];
			memcpy(pNewData, bySend, dwWriteLen);
			::PostMessage(g_data.m_hWnd, UM_SEND_DATA, (WPARAM)pNewData, dwWriteLen);
		}

	}

	ReadDataAsyn(1000);
}

void CBusiness::SendHandTempAsyn(DWORD dwIndex) {
	g_data.m_thrd_launch->PostMessage(this, MSG_SEND_HAND_TEMP,new CSendHandTempParam(dwIndex) );
}

void  CBusiness::SendHandTemp(const CSendHandTempParam * pParam) {
	if ( m_serial_port.GetStatus() == CLmnSerialPort::CLOSE ) {
		return;
	}

	BYTE   byIndex = (BYTE)pParam->m_dwIndex;
	BYTE   bySend[128];
	DWORD  dwDataLen = 32;
	memcpy(bySend, "\x55\x1E\x0B\x02\x00\x00\x00\x01\x34\x4C\x8C\x7E\xE3\x59\x02\xE0\x10\x5A\x00\x00\x00\x00\x2E\xF1\x79\xA4\x00\x01\x04\xE0\x00\xFF", dwDataLen);
	memcpy(bySend + 8,  "\x01\x00\x00\x00\x00\x00\x30\xe1", 8);
	bySend[8] += byIndex;

	memcpy(bySend + 22, "\x00\x00\x00\x00\x00\x00\x55\xe0", 8);
	bySend[22] += byIndex;

	// 如果未初始化温度
	if (m_arrHandTemp[byIndex] == 0) {
		m_arrHandTemp[byIndex] = GetRand(3200, 4080);
	}
	// 已经初始化温度
	else {
		DWORD a = GetRand(1, 100);
		DWORD b = GetRand(0, 20);
		if (a > 50) {
			m_arrHandTemp[byIndex] += b;
		}
		else {
			m_arrHandTemp[byIndex] -= b;
		}

		if (m_arrHandTemp[byIndex] > 4080) {
			m_arrHandTemp[byIndex] = 4080;
		}
		else if (m_arrHandTemp[byIndex] < 3200) {
			m_arrHandTemp[byIndex] = 3200;
		}
	}
	DWORD dwTemp = m_arrHandTemp[byIndex];
	bySend[16] = (BYTE)(dwTemp / 100);
	bySend[17] = (BYTE)(dwTemp % 100);
	m_serial_port.Write(bySend, dwDataLen);

	BYTE * pNewData = new BYTE[dwDataLen];
	memcpy(pNewData, bySend, dwDataLen);
	::PostMessage(g_data.m_hWnd, UM_SEND_DATA, (WPARAM)pNewData, dwDataLen);
}

// 消息处理
void CBusiness::OnMessage(DWORD dwMessageId, const  LmnToolkits::MessageData * pMessageData) {
	switch (dwMessageId)
	{
	case MSG_CONNECT_COM:
	{
		ConnectComPort();
	}
	break;

	case MSG_READ_DATA:
	{
		ReadData();
	}
	break;

	case MSG_SEND_HAND_TEMP:
	{
		CSendHandTempParam * pParam = (CSendHandTempParam *)pMessageData;
		SendHandTemp(pParam);
	}
	break;

	default:
		break;
	}
}