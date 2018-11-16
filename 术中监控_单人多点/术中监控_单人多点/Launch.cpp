#include "Launch.h"
#include "business.h"
#include "LmnTelSvr.h"

// 测温如果失败，则连续再测量2次。
// 如果连续3次失败，则认为连接有问题

#define  MAX_RETRY_TEMP_TIME        3

// 当由于没有病区号，重试的间隔
#define  NEXT_RETRY_INTERVAL_TIME        2000

CLaunch::CLaunch(CBusiness * pBusiness) {
	memset( m_dwGridRetryTime, 0, sizeof(m_dwGridRetryTime) );
	sigLaunchError.connect(pBusiness, &CBusiness::OnLaunchError);
	sigHeartBeatRet.connect(pBusiness, &CBusiness::OnHeartBeatRet);
	sigTempRet.connect(pBusiness, &CBusiness::OnTempRet);
}

CLaunch::~CLaunch() {

} 

void  CLaunch::CloseLaunch() {
	CloseUartPort();
	m_recv_buf.Clear();
}

BOOL  CLaunch::WriteLaunch(const void * WriteBuf, DWORD & WriteDataLen) {
	BOOL bRet = Write(WriteBuf, WriteDataLen);
	LmnSleep(2000);
	return bRet;
}

int  CLaunch::Reconnect() {
	g_log->Output(ILog::LOG_SEVERITY_INFO, "launch reconnect \n");

	if ( GetStatus() == CLmnSerialPort::OPEN ) {
		CloseLaunch();
	}

	char  szComPort[16] = { 0 };
	if ( g_data.m_szLaunchComPort[0] != 0 ) {
		STRNCPY(szComPort, g_data.m_szLaunchComPort, sizeof(szComPort));
	}
	else {
		int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));

		// 太多的ch340
		if ( nFindCount > 1 ) {
			return -1;
		}
		// 没有找到ch340
		else if (nFindCount == 0) {
			return -1;
		}
	}

	BOOL bRet = OpenUartPort(szComPort);
	if (!bRet) {
		return -1;
	}
	m_recv_buf.Clear();	
	return 0;
}

// 硬件改动，检查状态
int  CLaunch::CheckStatus() {
	g_log->Output(ILog::LOG_SEVERITY_INFO, "check status \n");

	// 关闭状态
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		char szComPort[16];
		int nFindCount = GetCh340Count(szComPort, sizeof(szComPort));
		// 如果找到1个ch340串口，立即连接发射器
		if ( 1 == nFindCount ) {
			CBusiness::GetInstance()->ReconnectLaunchAsyn();
		}
		else if (g_data.m_szLaunchComPort[0] != 0) {
			CBusiness::GetInstance()->ReconnectLaunchAsyn();
		}
	}
	// 打开状态
	else {
		// 如果串口已经不在
		if (!CheckComPortExist(this->GetPort())) {
			sigLaunchError.emit();
		}
	}

	return 0;
}

// 心跳
int  CLaunch::HeartBeat(const CReaderHeartBeatParam * pParam) {
	// g_log->Output(ILog::LOG_SEVERITY_INFO, "query heart beat[%lu] \n", pParam->m_dwGridIndex);

	DWORD  dwGridIndex = pParam->m_dwGridIndex;
	assert(dwGridIndex < MAX_GRID_COUNT);

	// 如果串口没有打开
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		return 0;
	}

	// 如果病区号没有设置
	if (0 == g_data.m_dwAreaNo ) {
		return 0;
	}

	// 如果床号没有设置
	if (0 == g_data.m_dwBedNo[dwGridIndex]) {
		return 0;
	}

	// 如果没有打开reader开关
	if ( !g_data.m_bReaderSwitch[dwGridIndex] ) {
		return 0;
	}

	// 先处理数据
	ReadComData();

	BYTE  send_buf[8];
	DWORD dwSendLen = 8;

	memcpy(send_buf, "\x00\x00\x00\x55\x01\x01\xDD\xAA", 8);
	send_buf[1] = (BYTE)g_data.m_dwBedNo[dwGridIndex];
	send_buf[2] = (BYTE)g_data.m_dwAreaNo;
	//g_log->Output(ILog::LOG_SEVERITY_INFO, "write bed = %lu\n", g_dwBedNo[dwGridIndex]);
	WriteLaunch(send_buf, dwSendLen);
	return 0;
}

// 获取温度
int  CLaunch::QueryTemperature(const CGetTemperatureParam * pParam) {
	// g_log->Output(ILog::LOG_SEVERITY_INFO, "query temperature[%lu] \n", pParam->m_dwGridIndex);

	DWORD  dwGridIndex = pParam->m_dwGridIndex;
	assert(dwGridIndex < MAX_GRID_COUNT);

	// 如果串口没有打开
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		return 0;
	}

	// 如果病区号没有设置
	if (0 == g_data.m_dwAreaNo) {
		return 0;
	}

	// 如果床号没有设置
	if (0 == g_data.m_dwBedNo[dwGridIndex]) {
		return 0;
	}

	// 如果没有打开reader开关
	if (!g_data.m_bReaderSwitch[dwGridIndex]) {
		return 0;
	}

	// 先处理数据
	ReadComData();

	BYTE  send_buf[8];
	DWORD dwSendLen = 8;

	memcpy(send_buf, "\x00\x00\x00\x55\x01\x02\xDD\xAA", 8);
	send_buf[1] = (BYTE)g_data.m_dwBedNo[dwGridIndex];
	send_buf[2] = (BYTE)g_data.m_dwAreaNo;
	//g_log->Output(ILog::LOG_SEVERITY_INFO, "write bed = %lu\n", g_dwBedNo[dwGridIndex]);
	WriteLaunch(send_buf, dwSendLen);
	return 0;
}


// 读取串口数据
int  CLaunch::ReadComData() {
	// g_log->Output(ILog::LOG_SEVERITY_INFO, "handle read\n");
	//JTelSvrPrint("handle read");

	// 如果串口没有打开
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		return 0;
	}

	BYTE buf[8192];
	DWORD  dwBufLen = sizeof(buf);
	if ( Read(buf, dwBufLen) ) {
		if (dwBufLen > 0) {
			m_recv_buf.Append(buf, dwBufLen);
		}
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

	if ( m_recv_buf.GetDataLength() >= 17 ) {
		m_recv_buf.Read(buf, 17);

		// 数据头错误
		if (buf[0] != (BYTE)'\x55') {
			DebugStream(debug_buf, sizeof(debug_buf), buf, 17);
			g_log->Output(ILog::LOG_SEVERITY_ERROR, "错误的数据头：\n%s\n", debug_buf );
			sigLaunchError.emit();
		}
		else {
			// 如果是心跳数据
			if ( buf[1] == (BYTE)'\x0E' ) {
				m_recv_buf.Reform();
				// 如果错误的数据尾
				if ( buf[16] != (BYTE)'\xFF' ) {
					DebugStream(debug_buf, sizeof(debug_buf), buf, 17);
					g_log->Output(ILog::LOG_SEVERITY_ERROR, "错误的数据尾：\n%s\n", debug_buf);
					sigLaunchError.emit();
				}
				else {
					WORD   dwBedNo  = buf[2] * 256 + buf[3];
					DWORD  dwAreaNo = buf[4];
					// 如果病区号不同
					if (dwAreaNo != g_data.m_dwAreaNo) {
						DebugStream(debug_buf, sizeof(debug_buf), buf, 17);
						g_log->Output(ILog::LOG_SEVERITY_ERROR, "病区号和设置的不匹配：\n%s\n", debug_buf);
					}
					else {
						DWORD dwGridIndex = FindGridIndexByBed(dwBedNo);
						// 如果找到床位号
						if (dwGridIndex != -1) {
							sigHeartBeatRet.emit(dwGridIndex,0);							
						}
						else {
							DebugStream(debug_buf, sizeof(debug_buf), buf, 17);
							g_log->Output(ILog::LOG_SEVERITY_ERROR, "得到不存在的读卡器心跳：\n%s\n", debug_buf);
						}
					}
				}
			} // 结束 心跳
			// 如果是温度数据
			else if ( buf[1] == (BYTE)'\x1A' ) {
				// 如果得到足够数据
				if ( m_recv_buf.GetDataLength() >= 29 - 17 ) {
					m_recv_buf.Read( buf+17, 12 );
					m_recv_buf.Reform();

					if (buf[28] != (BYTE)'\xFF') {
						DebugStream(debug_buf, sizeof(debug_buf), buf, 29);
						g_log->Output(ILog::LOG_SEVERITY_ERROR, "错误的数据尾：\n%s\n", debug_buf);
						sigLaunchError.emit();
					}
					else {
						DWORD  dwBedNo = buf[2] * 256 + buf[3];
						DWORD  dwAreaNo = buf[4];
						DWORD  dwTemp = buf[24] * 1000 + buf[25] * 100 + buf[26] * 10 + buf[27];

						// 如果病区号不同
						if (dwAreaNo != g_data.m_dwAreaNo) {
							DebugStream(debug_buf, sizeof(debug_buf), buf, 29);
							g_log->Output(ILog::LOG_SEVERITY_ERROR, "病区号和设置的不匹配：\n%s\n", debug_buf);
						}
						else {
							DWORD dwGridIndex = FindGridIndexByBed(dwBedNo);

							// 找到
							if (dwGridIndex != -1) {
								sigTempRet.emit(dwGridIndex, dwTemp);
							}
							else {
								DebugStream(debug_buf, sizeof(debug_buf), buf, 29);
								g_log->Output(ILog::LOG_SEVERITY_ERROR, "得到不存在的读卡器温度：\n%s\n", debug_buf);
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

	DWORD   dwCurTick = LmnGetTickCount();

	// 处理超时问题
	for ( DWORD i = 0; i < MAX_READERS_COUNT; i++ ) {
		// 如果是close状态
		if ( g_data.m_nReaderStatus[i] == READER_STATUS_CLOSE ) {
			// 如果已经请求了心跳
			if ( g_data.m_dwLastQueryTick[i] != 0 ) {
				// 如果请求心跳超时
				if ( dwCurTick - g_data.m_dwLastQueryTick[i] >= 3500 ) {
					sigHeartBeatRet.emit(i, -1);
				}
			}
		}
		// 如果是open状态
		else {
			// 如果已经请求了温度
			if (g_data.m_dwLastQueryTick[i] != 0) {
				// 如果请求温度超时
				if (dwCurTick - g_data.m_dwLastQueryTick[i] >= 3500) {					
					g_data.m_nQueryTempRetryTime[i]++;
					g_data.m_dwLastQueryTick[i] = 0;
					if ( g_data.m_nQueryTempRetryTime[i] >= 3 ) {
						sigTempRet.emit(i, -1);
					}
					else {
						CBusiness::GetInstance()->QueryTemperatureAsyn(i);
					}
				}
			}
		}
	}

	return 0;
}