#include "Launch.h"
#include "business.h"

CLaunch::CLaunch() {
	
}

CLaunch::~CLaunch() {

}

void  CLaunch::CloseLaunch() {
	CloseUartPort();
	m_recv_buf.Clear();
	m_sigStatus.emit(GetStatus());
}

// ����
int  CLaunch::Reconnect() {
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "launch reconnect \n");

	if (GetStatus() == CLmnSerialPort::OPEN) {
		CloseLaunch();
	}

	char  szComPort[16] = { 0 };

	if ( g_data.m_szLaunchPort[0] != 0 ) {
		STRNCPY(szComPort, g_data.m_szLaunchPort, sizeof(szComPort));
	}
	else {
		int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));
		// ̫���ch340
		if (nFindCount > 1) {
			m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
			return 0;
		}
		// û���ҵ�ch340
		else if (nFindCount <= 0) {
			m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
			return 0;
		}
	}

	BOOL bRet = OpenUartPort(szComPort);
	if (!bRet) {
		m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
		return 0;
	}

	// ֪ͨ�ɹ�
	m_sigStatus.emit(GetStatus());
	m_recv_buf.Clear();

	return 0;
}

// ��ȡ�¶�
int  CLaunch::QueryTemperature(DWORD dwArea, DWORD dwBedID) {

	// �������û�д�
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		return 0;
	}

#if !TEST_FLAG_1
	// ���������û������
	if (0 == dwArea) {
		return 0;
	}
#endif

	// �������û������
	if (0 == dwBedID) {
		return 0;
	}

	// �ȴ�������
	ReadComData();

	BYTE  send_buf[8];
	DWORD dwSendLen = 8;

	memcpy(send_buf, "\x00\x00\x00\x55\x01\x02\xDD\xAA", 8);
	send_buf[1] = (BYTE)dwBedID;
	send_buf[2] = (BYTE)dwArea;
	WriteLaunch(send_buf, dwSendLen);
	return 0;
}

// ��ȡ�������ݲ���������
int  CLaunch::ReadComData() {
	// �������û�д�
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		m_sigCheck.emit();
		return 0;
	}

	BYTE buf[8192];
	DWORD  dwBufLen = sizeof(buf);
	if (Read(buf, dwBufLen)) {
		if (dwBufLen > 0) {
			m_recv_buf.Append(buf, dwBufLen);
		}
	}
	else {
		CloseLaunch();
		m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
		m_sigCheck.emit();
		return 0;
	}

	// ��������
	//
	// �¶�
	//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
	// -----------------------------------------------------------------------------------------
	// 55 1A 00 06 01 45 52 00 00 03 00 00 00 00 00 01 8F 50 D9 93 CD 59 02 E0 02 07 08 05 FF 
	//
	// ����
	// 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16
	//---------------------------------------------------------
	// 55 0E 00 06 01 45 52 00 00 03 00 00 00 00 00 01 FF

	char debug_buf[8192];
	const int MIN_DATA_LENGTH = 19;
	const int TEMP_DATA_LENGTH = 31;

	if (m_recv_buf.GetDataLength() >= MIN_DATA_LENGTH) {
		m_recv_buf.Read(buf, MIN_DATA_LENGTH);

		// ����ͷ����
		if (buf[0] != (BYTE)'\x55') {
			DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
			g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������ͷ��\n%s\n", debug_buf);

			CloseLaunch();
			m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
		}
		else {
			// �������������
			if (buf[1] == (BYTE)'\x0E') {
				m_recv_buf.Reform();
				// ������������β
				if (buf[16] != (BYTE)'\xFF') {
					DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
					g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������β��\n%s\n", debug_buf);

					CloseLaunch();
					m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
				}
				else {
					WORD   dwBedNo = buf[2] * 256 + buf[3];
					DWORD  dwAreaNo = buf[4];
					// ��������Ų�ͬ
					if (dwAreaNo != g_data.m_CfgData.m_dwAreaNo) {
						DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
						g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "�����ź����õĲ�ƥ�䣺\n%s\n", debug_buf);
					}
					else {
						DWORD dwRet = FindReaderIndexByBed(dwBedNo);
						// ����ҵ���λ��
						if (dwRet != -1) {
							m_sigReaderStatus.emit(LOWORD(dwRet), HIWORD(dwRet), READER_STATUS_OPEN);
						}
						else {
							DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
							g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "�õ������ڵĴ�����������\n%s\n", debug_buf);
						}
					}
				}
			} // ���� ����
			  // ������¶�����
			else if (buf[1] == (BYTE)'\x1A') {
				//DebugStream(debug_buf, sizeof(debug_buf), buf, 29);
				//g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���ݣ�\n%s\n", debug_buf);

				// ����õ��㹻����
				if (m_recv_buf.GetDataLength() >= TEMP_DATA_LENGTH - MIN_DATA_LENGTH) {
					m_recv_buf.Read(buf + MIN_DATA_LENGTH, TEMP_DATA_LENGTH - MIN_DATA_LENGTH);
					m_recv_buf.Reform();

					if (buf[28] != (BYTE)'\xFF') {
						DebugStream(debug_buf, sizeof(debug_buf), buf, TEMP_DATA_LENGTH);
						g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������β��\n%s\n", debug_buf);

						CloseLaunch();
						m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
					}
					else {
						DWORD  dwBedNo = buf[2] * 256 + buf[3];
						DWORD  dwAreaNo = buf[4];
						DWORD  dwTemp = buf[24] * 1000 + buf[25] * 100 + buf[26] * 10 + buf[27];

#if TEST_FLAG_1
						dwAreaNo = g_data.m_CfgData.m_dwAreaNo;
#endif

						// ��������Ų�ͬ
						if (dwAreaNo != g_data.m_CfgData.m_dwAreaNo) {
							DebugStream(debug_buf, sizeof(debug_buf), buf, TEMP_DATA_LENGTH);
							g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "�����ź����õĲ�ƥ�䣺\n%s\n", debug_buf);
						}
						else {
							DWORD dwRet = FindReaderIndexByBed(dwBedNo);

							// �ҵ�
							if (dwRet != -1) {
								LastTemp t;
								memset(&t, 0, sizeof(t));
								t.m_dwTemp = dwTemp;
								GetTagId(t.m_szTagId, sizeof(t.m_szTagId), buf + 16, 8);
								GetReaderId(t.m_szReaderId, sizeof(t.m_szReaderId), buf + 5, 11 );
								m_sigReaderTemp.emit( LOWORD(dwRet), HIWORD(dwRet), t );
							}
							else {
								DebugStream(debug_buf, sizeof(debug_buf), buf, TEMP_DATA_LENGTH);
								g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "�õ������ڵĴ������¶ȣ�\n%s\n", debug_buf);
							}
						}
					}
				}
				else {
					m_recv_buf.ResetReadPos();
				}
			}
		}
	}

	m_sigCheck.emit();
	return 0;
}

BOOL  CLaunch::WriteLaunch(const void * WriteBuf, DWORD & WriteDataLen) {
	BOOL bRet = Write(WriteBuf, WriteDataLen);
	LmnSleep(WRITE_SLEEP_TIME);
	return bRet;
}

int  CLaunch::CheckStatus() {
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "check status \n");

	// �ر�״̬
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		char szComPort[16];
		int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));
		// ����ҵ�1��ch340���ڣ��������ӷ�����
		if (1 == nFindCount) {
			m_sigReconnect.emit(0);
		}
		else if (g_data.m_szLaunchPort[0] != 0) {
			m_sigReconnect.emit(0);
		}
	}
	// ��״̬
	else {
		// ��������Ѿ�����
		if (!CheckComPortExist(this->GetPort())) {
			CloseLaunch();
			m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
		}
	}

	return 0;
}