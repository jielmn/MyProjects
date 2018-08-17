#include "Launch.h"
#include "business.h"

// �������ʧ�ܣ��������ٲ���2�Ρ�
// �������3��ʧ�ܣ�����Ϊ����������

#define  MAX_RETRY_TEMP_TIME        3

// ������û�в����ţ����Եļ��
#define  NEXT_RETRY_INTERVAL_TIME        2000

CLaunch::CLaunch() {
	memset( m_dwGridRetryTime, 0, sizeof(m_dwGridRetryTime) );
	m_dwLastWriteTick = 0;
}

CLaunch::~CLaunch() {

} 

void  CLaunch::CloseLaunch() {
	CloseUartPort();
	m_recv_buf.Clear();
	CBusiness::GetInstance()->NotifyUiLaunchStatus(GetStatus());
}

BOOL  CLaunch::WriteLaunch(const void * WriteBuf, DWORD & WriteDataLen) {
	DWORD  dwTick = LmnGetTickCount();
	if ( m_dwLastWriteTick != 0 ) {
		// ���������ʱ����
		DWORD  dwElapsed = dwTick - m_dwLastWriteTick;
		if ( dwElapsed < g_dwLaunchWriteInterval ) {
			LmnSleep(g_dwLaunchWriteInterval - dwElapsed);
		}
	}
	m_dwLastWriteTick = dwTick;

	return Write(WriteBuf, WriteDataLen);
}

int  CLaunch::Reconnect() {

	if ( GetStatus() == CLmnSerialPort::OPEN ) {
		CloseLaunch();
	}

	char  szComPort[16] = { 0 };
	if ( g_szLaunchComPort[0] != 0 ) {
		STRNCPY(szComPort, g_szLaunchComPort, sizeof(szComPort));
	}
	else {
		int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));

		// ̫���ch340
		if ( nFindCount > 1 ) {
			CBusiness::GetInstance()->ReconnectLaunchAsyn(RECONNECT_LAUNCH_TIME_INTERVAL);
			return 0;
		}
		// û���ҵ�ch340
		else if (nFindCount == 0) {
			CBusiness::GetInstance()->ReconnectLaunchAsyn(RECONNECT_LAUNCH_TIME_INTERVAL);
			return 0;
		}
	}

	BOOL bRet = OpenUartPort(szComPort);
	if (!bRet) {
		CBusiness::GetInstance()->ReconnectLaunchAsyn(RECONNECT_LAUNCH_TIME_INTERVAL);
		return 0;
	}

	// ֪ͨ�ɹ�
	CBusiness::GetInstance()->NotifyUiLaunchStatus(GetStatus());
	m_recv_buf.Clear();

	// ��ȡ�¶�
	DWORD  dwCnt = g_dwLayoutColumns * g_dwLayoutRows;
	DWORD  dwDelay = 200;
	for (DWORD i = 0; i < dwCnt; i++) {
		CBusiness::GetInstance()->QueryTemperatureAsyn(i, dwDelay);
		dwDelay += 200;
	}

	CBusiness::GetInstance()->ReadLaunchAsyn();
	return 0;
}

// Ӳ���Ķ������״̬
int  CLaunch::CheckStatus() {

	if (GetStatus() == CLmnSerialPort::CLOSE) {
		return 0;
	}

	// ��������Ѿ�����
	if ( !CheckComPortExist( this->GetPort() ) ) {
		CloseUartPort();
		CBusiness::GetInstance()->NotifyUiLaunchStatus(GetStatus());
		CBusiness::GetInstance()->ReconnectLaunchAsyn(RECONNECT_LAUNCH_TIME_INTERVAL);
	}

	return 0;
}

// ��ȡ�¶�
int  CLaunch::QueryTemperature(const CGetTemperatureParam * pParam) {
	DWORD  dwGridIndex = pParam->m_dwGridIndex;
	assert(dwGridIndex < MAX_GRID_COUNT);

	// �������û�д�
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		return 0;
	}

	// ���������û������
	if (0 == g_dwAreaNo) {
		CBusiness::GetInstance()->QueryTemperatureAsyn(dwGridIndex, NEXT_RETRY_INTERVAL_TIME);
		return 0;
	}

	// �������û������
	if (0 == g_dwBedNo[dwGridIndex]) {
		return 0;
	}

	BYTE  send_buf[8];
	DWORD dwSendLen = 8;

	memcpy(send_buf, "\x00\x00\x00\x55\x01\x02\xDD\xAA", 8);
	send_buf[1] = (BYTE)g_dwBedNo[dwGridIndex];
	send_buf[2] = (BYTE)g_dwAreaNo;
	//g_log->Output(ILog::LOG_SEVERITY_INFO, "write bed = %lu\n", g_dwBedNo[dwGridIndex]);
	WriteLaunch(send_buf, dwSendLen);
	return 0;
}


// ��ȡ��������
int  CLaunch::ReadComData() {
	// �������û�д�
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		return 0;
	}

	char buf[8192];
	DWORD  dwBufLen = sizeof(buf);
	if ( Read(buf, dwBufLen) ) {
		if (dwBufLen > 0) {
			m_recv_buf.Append(buf, dwBufLen);
		}
	}

	// ��������
	//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
	// -----------------------------------------------------------------------------------------
	// 55 1A 00 06 01 45 52 00 00 03 00 00 00 00 00 01 8F 50 D9 93 CD 59 02 E0 02 07 08 05 FF 

	if ( m_recv_buf.GetDataLength() >= 29 ) {
		m_recv_buf.Read(buf, 29);
		m_recv_buf.Reform();

		if ( buf[0] == '\x55' && buf[1] == '\x1A' && buf[28] == '\xFF' ) {
			DWORD  dwBedNo  = buf[2] * 256 + buf[3];
			DWORD  dwAreaNo = buf[4];
			DWORD  dwTemp = buf[24] * 1000 + buf[25] * 100 + buf[26] * 10 + buf[27];
			// �����������ͬ
			if ( dwAreaNo == g_dwAreaNo ) {
				DWORD  dwCnt = g_dwLayoutColumns * g_dwLayoutRows;
				DWORD  dwGridIndex = -1;
				for (dwGridIndex = 0; dwGridIndex < dwCnt; dwGridIndex++ ) {
					// �ҵ���λ��
					if ( g_dwBedNo[dwGridIndex] == dwBedNo ) {
						break;
					}
				}

				// �ҵ�
				if (dwGridIndex != -1) {
					CBusiness::GetInstance()->NotifyUiTempData(dwGridIndex, dwTemp);
				}
			}
		}
		// ��������ݸ�ʽ
		else {
			char tmp[8192];
			DebugStream(tmp, sizeof(tmp), buf, 29);
			g_log->Output( ILog::LOG_SEVERITY_ERROR, "��������ݸ�ʽ��\n%s\n", tmp );

			CloseLaunch();
			CBusiness::GetInstance()->ReconnectLaunchAsyn(RECONNECT_LAUNCH_TIME_INTERVAL);
		}
	}


	return 0;
}