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

// 重连
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
		// 太多的ch340
		if (nFindCount > 1) {
			m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
			return 0;
		}
		// 没有找到ch340
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

	// 通知成功
	m_sigStatus.emit(GetStatus());
	m_recv_buf.Clear();

	return 0;
}

// 获取温度
int  CLaunch::QueryTemperature(DWORD dwArea, DWORD dwBedID) {

	// 如果串口没有打开
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		return 0;
	}

#if !TEST_FLAG_1
	// 如果病区号没有设置
	if (0 == dwArea) {
		return 0;
	}
#endif

	// 如果床号没有设置
	if (0 == dwBedID) {
		return 0;
	}

	// 先处理数据
	ReadComData();

	BYTE  send_buf[8];
	DWORD dwSendLen = 8;

	memcpy(send_buf, "\x00\x00\x00\x55\x01\x02\xDD\xAA", 8);
	send_buf[1] = (BYTE)dwBedID;
	send_buf[2] = (BYTE)dwArea;
	WriteLaunch(send_buf, dwSendLen);
	return 0;
}

// 读取串口数据并处理数据
int  CLaunch::ReadComData() {
	// 如果串口没有打开
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

	// 处理数据
	//
	// 温度
	//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
	// -----------------------------------------------------------------------------------------
	// 55 1A 00 06 01 45 52 00 00 03 00 00 00 00 00 01 8F 50 D9 93 CD 59 02 E0 02 07 08 05 FF 
	//
	// 心跳
	// 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16
	//---------------------------------------------------------
	// 55 0E 00 06 01 45 52 00 00 03 00 00 00 00 00 01 FF

	char debug_buf[8192];
	const int MIN_DATA_LENGTH = 19;
	const int TEMP_DATA_LENGTH = 31;

	if (m_recv_buf.GetDataLength() >= MIN_DATA_LENGTH) {
		m_recv_buf.Read(buf, MIN_DATA_LENGTH);

		// 数据头错误
		if (buf[0] != (BYTE)'\x55') {
			DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
			g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "错误的数据头：\n%s\n", debug_buf);

			CloseLaunch();
			m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
		}
		else {
			// 如果是心跳数据
			if (buf[1] == (BYTE)'\x0E') {
				m_recv_buf.Reform();
				// 如果错误的数据尾
				if (buf[16] != (BYTE)'\xFF') {
					DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
					g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "错误的数据尾：\n%s\n", debug_buf);

					CloseLaunch();
					m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
				}
				else {
					WORD   dwBedNo = buf[2] * 256 + buf[3];
					DWORD  dwAreaNo = buf[4];
					// 如果病区号不同
					if (dwAreaNo != g_data.m_CfgData.m_dwAreaNo) {
						DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
						g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "病区号和设置的不匹配：\n%s\n", debug_buf);
					}
					else {
						DWORD dwRet = FindReaderIndexByBed(dwBedNo);
						// 如果找到床位号
						if (dwRet != -1) {
							m_sigReaderStatus.emit(LOWORD(dwRet), HIWORD(dwRet), READER_STATUS_OPEN);
						}
						else {
							DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
							g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "得到不存在的窗格子心跳：\n%s\n", debug_buf);
						}
					}
				}
			} // 结束 心跳
			  // 如果是温度数据
			else if (buf[1] == (BYTE)'\x1A') {
				//DebugStream(debug_buf, sizeof(debug_buf), buf, 29);
				//g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "数据：\n%s\n", debug_buf);

				// 如果得到足够数据
				if (m_recv_buf.GetDataLength() >= TEMP_DATA_LENGTH - MIN_DATA_LENGTH) {
					m_recv_buf.Read(buf + MIN_DATA_LENGTH, TEMP_DATA_LENGTH - MIN_DATA_LENGTH);
					m_recv_buf.Reform();

					if (buf[28] != (BYTE)'\xFF') {
						DebugStream(debug_buf, sizeof(debug_buf), buf, TEMP_DATA_LENGTH);
						g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "错误的数据尾：\n%s\n", debug_buf);

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

						// 如果病区号不同
						if (dwAreaNo != g_data.m_CfgData.m_dwAreaNo) {
							DebugStream(debug_buf, sizeof(debug_buf), buf, TEMP_DATA_LENGTH);
							g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "病区号和设置的不匹配：\n%s\n", debug_buf);
						}
						else {
							DWORD dwRet = FindReaderIndexByBed(dwBedNo);

							// 找到
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
								g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "得到不存在的窗格子温度：\n%s\n", debug_buf);
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

	// 关闭状态
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		char szComPort[16];
		int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));
		// 如果找到1个ch340串口，立即连接发射器
		if (1 == nFindCount) {
			m_sigReconnect.emit(0);
		}
		else if (g_data.m_szLaunchPort[0] != 0) {
			m_sigReconnect.emit(0);
		}
	}
	// 打开状态
	else {
		// 如果串口已经不在
		if (!CheckComPortExist(this->GetPort())) {
			CloseLaunch();
			m_sigReconnect.emit(RECONNECT_LAUNCH_TIME_INTERVAL);
		}
	}

	return 0;
}