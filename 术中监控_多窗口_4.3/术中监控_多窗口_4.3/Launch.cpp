#include "Launch.h"
#include "business.h"

#define   WRITE_SLEEP_TIME               1100

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
			return 0;
		}
		// 没有找到ch340
		else if (nFindCount <= 0) {
			return 0;
		}
	}

	BOOL bRet = OpenUartPort(szComPort);
	if (!bRet) {
		return 0;
	}

	// 通知成功
	m_sigStatus.emit(GetStatus());
	m_recv_buf.Clear();

	return 0;
}

// 获取温度
int  CLaunch::QueryTemperature(BYTE byArea, WORD wBedID) {

	// 如果串口没有打开
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		return 0;
	}

	// 如果病区号没有设置
	if (0 == byArea) {
		return 0;
	}

	// 如果床号没有设置
	if (0 == wBedID) {
		return 0;
	}

	// 先处理数据
	ReadComData();

	BYTE  send_buf[8];
	DWORD dwSendLen = 8;

	memcpy(send_buf, "\x00\x00\x00\x55\x01\x02\xDD\xAA", 8);
	send_buf[0] = (wBedID>>8)&0xFF;
	send_buf[1] = wBedID & 0xFF;
	send_buf[2] = byArea;
	WriteLaunch(send_buf, dwSendLen);
	return 0;
}

// 读取串口数据并处理数据
int  CLaunch::ReadComData() {
	// 如果串口没有打开
	if (GetStatus() == CLmnSerialPort::CLOSE) {
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
		return 0;
	}

	// 术中读卡器温度
	//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
	// -----------------------------------------------------------------------------------------
	// 55 1A 00 06 01 45 52 00 00 03 00 00 00 00 00 01 8F 50 D9 93 CD 59 02 E0 02 07 08 05 FF 

	// 手持温度数据
	//  0  1  2  3     4  5  6  7      8  9 10 11 12 13 14 15     16 17     18 19 20 21     22 23 24 25 26 27 28 29     30 31
	// ------------  -------------    ------------------------    ------    ------------    ------------------------    ------
	// 55 1E 0B 02    00 00 00 01     34 4C 8C 7E E3 59 02 E0     10 5A     00 00 00 00     2E F1 79 A4 00 01 04 E0     00 FF 
	//     信道 地址      SN码                 TagID               温度                             白卡ID

	char debug_buf[8192];
	const int MIN_DATA_LENGTH = 29;
	const int SURGENCY_TEMP_DATA_LENGTH = 29;
	const int HAND_TEMP_DATA_LENGTH     = 32;

	// 长度不够
	if ( m_recv_buf.GetDataLength() < MIN_DATA_LENGTH ) {
		return 0;
	}

	m_recv_buf.Read(buf, MIN_DATA_LENGTH);

	// 数据头错误
	if (buf[0] != (BYTE)'\x55') {
		DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "错误的数据头：\n%s\n", debug_buf);
		CloseLaunch();
		return 0;
	}
		
	// 如果是术中读卡器温度数据
	if ( buf[1] == 0x1A ) {
		m_recv_buf.Reform();

		if (buf[28] != (BYTE)'\xFF') {
			DebugStream(debug_buf, sizeof(debug_buf), buf, SURGENCY_TEMP_DATA_LENGTH);
			g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "错误的数据尾：\n%s\n", debug_buf);
			CloseLaunch();
			return 0;
		}

		WORD   wBedNo   = (buf[2] << 8) | buf[3];
		BYTE   byAreaNo = buf[4];
		DWORD  dwTemp   = buf[24] * 1000 + buf[25] * 100 + buf[26] * 10 + buf[27];

		// 如果病区号不同
		if ( byAreaNo != g_data.m_CfgData.m_dwAreaNo) {
			DebugStream(debug_buf, sizeof(debug_buf), buf, SURGENCY_TEMP_DATA_LENGTH);
			g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "病区号和设置的不匹配：\n%s\n", debug_buf);
			return 0;
		}

		//DWORD dwRet = FindReaderIndexByBed(dwBedNo);

		//// 找到
		//if (dwRet != -1) {
		//	LastTemp t;
		//	memset(&t, 0, sizeof(t));
		//	t.m_dwTemp = dwTemp;
		//	GetTagId(t.m_szTagId, sizeof(t.m_szTagId), buf + 16, 8);
		//	GetReaderId(t.m_szReaderId, sizeof(t.m_szReaderId), buf + 5, 11);
		//	m_sigReaderTemp.emit(LOWORD(dwRet), HIWORD(dwRet), t);
		//}
		//else {
		//	DebugStream(debug_buf, sizeof(debug_buf), buf, TEMP_DATA_LENGTH);
		//	g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "得到不存在的窗格子温度：\n%s\n", debug_buf);
		//}

		//// 清除结尾可能存在的"dd aa"
		//if ( m_recv_buf.GetDataLength() >= 2 ) {
		//	m_recv_buf.Read(buf, 2);
		//	// 如果是 dd aa结尾
		//	if ( buf[0] == (BYTE)'\xDD' && buf[1] == (BYTE)'\xAA' ) {
		//		m_recv_buf.Reform();
		//	}
		//	else {
		//		m_recv_buf.ResetReadPos();
		//	}
		//}
		
	
	}
	// 如果是手持温度数据
	else if (buf[1] == (BYTE)'\x1E') {
	}
	
	
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
	if ( GetStatus() == CLmnSerialPort::CLOSE ) {
		Reconnect();
	}
	// 打开状态
	else {
		// 如果串口已经不在
		if (!CheckComPortExist(this->GetPort())) {
			CloseLaunch();
		}
	}

	return 0;
}