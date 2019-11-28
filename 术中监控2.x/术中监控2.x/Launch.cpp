#include "Launch.h"
#include "business.h"

#define   WRITE_SLEEP_TIME               1100

CLaunch::CLaunch() {
	
}

CLaunch::~CLaunch() {

}

void  CLaunch::CloseLaunch() {
	JTelSvrPrint("launch close");
	CloseUartPort();
	m_recv_buf.Clear();
	m_sigStatus.emit(GetStatus());
}

// ����
int  CLaunch::Reconnect() {
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "launch reconnect \n");
	JTelSvrPrint("launch reconnect");

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
			return 0;
		}
		// û���ҵ�ch340
		else if (nFindCount <= 0) {
			return 0;
		}
	}

	JTelSvrPrint("config com port = %s", szComPort);
	BOOL bRet = OpenUartPort(szComPort);
	if (!bRet) {
		return 0;
	}

	// ֪ͨ�ɹ�
	m_sigStatus.emit(GetStatus());
	m_recv_buf.Clear();

	return 0;
}

// ��ȡ�¶�
int  CLaunch::QueryTemperature(BYTE byArea, WORD wBedID) {

	// �������û�д�
	if (GetStatus() == CLmnSerialPort::CLOSE) {
		return 0;
	}

	// ���������û������
	if (0 == byArea) {
		return 0;
	}

	// �������û������
	if (0 == wBedID) {
		return 0;
	}

	// �ȴ�������
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

// ��ȡ�������ݲ���������
int  CLaunch::ReadComData() {
	// �������û�д�
	if ( GetStatus() == CLmnSerialPort::CLOSE ) {
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

	// ���ж������¶�
	//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
	// -----------------------------------------------------------------------------------------
	// 55 1A 00 06 01 45 52 00 00 03 00 00 00 00 00 01 8F 50 D9 93 CD 59 02 E0 02 07 08 05 FF 

	// �ֳ��¶�����
	//  0  1  2  3     4  5  6  7      8  9 10 11 12 13 14 15     16 17     18 19 20 21     22 23 24 25 26 27 28 29     30 31
	// ------------  -------------    ------------------------    ------    ------------    ------------------------    ------
	// 55 1E 0B 02    00 00 00 01     34 4C 8C 7E E3 59 02 E0     10 5A     00 00 00 00     2E F1 79 A4 00 01 04 E0     00 FF 
	//     �ŵ� ��ַ      SN��                 TagID               �¶�                             �׿�ID

	char debug_buf[8192];

#if !TRI_TAGS_FLAG
	const int MIN_DATA_LENGTH = 29;
	const int SURGENCY_TEMP_DATA_LENGTH = 29;
	const int HAND_TEMP_DATA_LENGTH     = 32;

	// ���Ȳ���
	if ( m_recv_buf.GetDataLength() < MIN_DATA_LENGTH ) {
		return 0;
	}

	m_recv_buf.Read(buf, MIN_DATA_LENGTH);

	// ����ͷ����
	if (buf[0] != 0x55) {
		DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������ͷ��\n%s\n", debug_buf);
		CloseLaunch();
		return 0;
	}
		
	// ��������ж������¶�����
	if ( buf[1] == 0x1A ) {
		m_recv_buf.Reform();
		ProcSurReader(buf, SURGENCY_TEMP_DATA_LENGTH);
		// �����β���ܴ��ڵ�"dd aa"
		ProcTail();
	}
	// ������ֳ��¶�����
	else if (buf[1] == 0x1E) {
		// ���û���㹻����
		if ( m_recv_buf.GetDataLength() < HAND_TEMP_DATA_LENGTH - MIN_DATA_LENGTH ) {
			m_recv_buf.ResetReadPos();
			return 0;
		}

		// �������������� 
		m_recv_buf.Read(buf + MIN_DATA_LENGTH, HAND_TEMP_DATA_LENGTH - MIN_DATA_LENGTH);
		m_recv_buf.Reform();

		ProcHandeReader(buf, HAND_TEMP_DATA_LENGTH);
		// �����β���ܴ��ڵ�"dd aa"
		ProcTail();
	}
#else

	const int MIN_DATA_LENGTH = 16;
	const int BATERRY_TEMP_DATAl_LENGTH = 16;       // ��Դtag
	const int SURGENCY_TEMP_DATA_LENGTH = 29;
	const int HAND_TEMP_DATA_LENGTH = 32;

	if (m_recv_buf.GetDataLength() < MIN_DATA_LENGTH) {
		return 0;
	}
	m_recv_buf.Read(buf, MIN_DATA_LENGTH);

	if (buf[0] != 0x55) {
		DebugStream(debug_buf, sizeof(debug_buf), buf, MIN_DATA_LENGTH);
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������ͷ��\n%s\n", debug_buf);
		CloseLaunch();
		return 0;
	}

	// �������Դ����
	if (buf[1] == 0x0D) {
		m_recv_buf.Reform();
		ProcBatteryTemp(buf, BATERRY_TEMP_DATAl_LENGTH);
		ProcTail();
	}
	// ��������ж������¶�����
	else if (buf[1] == 0x1A) {
		DWORD  dwExtraLength = SURGENCY_TEMP_DATA_LENGTH - MIN_DATA_LENGTH;
		DWORD  dwExpectedLength = SURGENCY_TEMP_DATA_LENGTH;

		// ���û���㹻����
		if (m_recv_buf.GetDataLength() < dwExtraLength ) {
			m_recv_buf.ResetReadPos();
			return 0;
		}

		// �������������� 
		m_recv_buf.Read(buf + MIN_DATA_LENGTH, dwExtraLength);

		m_recv_buf.Reform();
		ProcSurReader(buf, dwExpectedLength);
		ProcTail();
	}
	// ������ֳ��¶�����
	else if (buf[1] == 0x1E) {
		DWORD  dwExtraLength = HAND_TEMP_DATA_LENGTH - MIN_DATA_LENGTH;
		DWORD  dwExpectedLength = HAND_TEMP_DATA_LENGTH;

		// ���û���㹻����
		if (m_recv_buf.GetDataLength() < dwExtraLength) {
			m_recv_buf.ResetReadPos();
			return 0;
		}

		// �������������� 
		m_recv_buf.Read(buf + MIN_DATA_LENGTH, dwExtraLength);
		m_recv_buf.Reform();

		ProcHandeReader(buf, dwExpectedLength);
		// �����β���ܴ��ڵ�"dd aa"
		ProcTail();
	}

#endif
	return 0;
}

// ������Դ����
// Э�飺  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
//        55 0D 01 02 E1 01 FF DA A1 05 12 41 23 14 00 FF
void   CLaunch::ProcBatteryTemp(const BYTE * pData, DWORD dwDataLen) {
	char debug_buf[8192];

	if (pData[15] != (BYTE)'\xFF') {
		DebugStream(debug_buf, sizeof(debug_buf), pData, dwDataLen);
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������β(������������)��\n%s\n", debug_buf);
		return;
	}

	BYTE   byIndex = pData[3];
	DWORD  dwTemp = pData[12] * 100 + pData[13];

	TempItem temp_item;
	memset(&temp_item, 0, sizeof(temp_item));

	temp_item.m_dwTemp = dwTemp;
	temp_item.m_time = time(0);

	Bytes2String(temp_item.m_szTagId, sizeof(temp_item.m_szTagId), pData+4, 8);

	char szDeviceId[20] = {0};
	Bytes2String(szDeviceId, sizeof(szDeviceId), pData + 4, 4);

	WORD  wBedNo = 0;
	for ( DWORD i = 0; i < g_data.m_dwBatteryBindCnt; i++ ) {
		if (   0 == strcmp(g_data.m_battery_binding[i].m_szDeviceId, szDeviceId) 
			&& g_data.m_battery_binding[i].m_nIndex == (int)byIndex ) {
			wBedNo = (g_data.m_battery_binding[i].m_nGridIndex - 1) * MAX_READERS_PER_GRID + 1;
		}
	}

	if ( wBedNo > 0 )
		m_sigReaderTemp.emit(wBedNo, temp_item);
}

// ���ж������¶�
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
// -----------------------------------------------------------------------------------------
// 55 1A 00 06 01 45 52 00 00 03 00 00 00 00 00 01 8F 50 D9 93 CD 59 02 E0 02 07 08 05 FF 
// 
void  CLaunch::ProcSurReader(const BYTE * pData, DWORD dwDataLen) {
	char debug_buf[8192];

	if ( pData[28] != (BYTE)'\xFF' ) {
		DebugStream( debug_buf, sizeof(debug_buf), pData, dwDataLen );
#if HANDLE_WRONG_FORMAT_1
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������β(������������)��\n%s\n", debug_buf);
#else
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������β(�Ͽ�����)��\n%s\n", debug_buf);
		CloseLaunch();
#endif
		return;
	}

	WORD   wBedNo   = (pData[2] << 8) | pData[3];
	BYTE   byAreaNo = pData[4];
	DWORD  dwTemp   = pData[24] * 1000 + pData[25] * 100 + pData[26] * 10 + pData[27];

#if !TRI_TAGS_FLAG
	// ��������Ų�ͬ
	if (byAreaNo != g_data.m_CfgData.m_dwAreaNo) {
		DebugStream(debug_buf, sizeof(debug_buf), pData, dwDataLen);
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "�����ź����õĲ�ƥ�䣺\n%s\n", debug_buf);
		return;
	}
#endif

	// ��λ��̫��
	if (wBedNo > MAX_READERS_PER_GRID * MAX_GRID_COUNT) {
		DebugStream(debug_buf, sizeof(debug_buf), pData, dwDataLen);
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "̫��Ĵ�λ�ţ�\n%s\n", debug_buf);
		return;
	}

	TempItem temp_item;
	memset(&temp_item, 0, sizeof(temp_item));

	temp_item.m_dwTemp = dwTemp;
	temp_item.m_time = time(0);
	GetTagId(temp_item.m_szTagId, sizeof(temp_item.m_szTagId), pData + 16, 8);
	GetSurReaderId(temp_item.m_szReaderId, sizeof(temp_item.m_szReaderId), pData + 5, 11);
	m_sigReaderTemp.emit(wBedNo, temp_item);
}

// �����β���ܴ��ڵ�"dd aa"
void  CLaunch::ProcTail() {
	if (m_recv_buf.GetDataLength() >= 2) {
		BYTE  buf[32];
		m_recv_buf.Read(buf, 2);
		// ����� dd aa��β
		if (buf[0] == 0xDD && buf[1] == 0xAA) {
			m_recv_buf.Reform();
		}
		else {
			m_recv_buf.ResetReadPos();
		}
	}
}

// �����ֳֶ���������
void   CLaunch::ProcHandeReader(const BYTE * pData, DWORD dwDataLen) {
	char debug_buf[8192];

	// ������һ���ֽڲ���0xFF
	if ( pData[dwDataLen-1] != 0xFF ) {
		DebugStream( debug_buf, sizeof(debug_buf), pData, dwDataLen );
#if HANDLE_WRONG_FORMAT_1
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������β(������������)��\n%s\n", debug_buf);
#else
		g_data.m_log->Output(ILog::LOG_SEVERITY_ERROR, "���������β(�Ͽ�����)��\n%s\n", debug_buf);
		CloseLaunch();
#endif
		return;
	}

	TempItem item;
	memset(&item, 0, sizeof(item));

	item.m_dwTemp = pData[16] * 100 + pData[17];
	item.m_time = time(0);
	GetTagId(item.m_szTagId, sizeof(item.m_szTagId), pData + 8, 8);
	GetHandReaderId( item.m_szReaderId, sizeof(item.m_szReaderId), pData + 4);
	m_sigHandReaderTemp.emit(item);
}

BOOL  CLaunch::WriteLaunch(const void * WriteBuf, DWORD & WriteDataLen) {
	BOOL bRet = Write(WriteBuf, WriteDataLen);
	LmnSleep(WRITE_SLEEP_TIME);
	return bRet;
}

int  CLaunch::CheckStatus() {
	g_data.m_log->Output(ILog::LOG_SEVERITY_INFO, "check status \n");

	// �ر�״̬
	if ( GetStatus() == CLmnSerialPort::CLOSE ) {
		Reconnect();
	}
	// ��״̬
	else {
		// ��������Ѿ�����
		if (!CheckComPortExist(this->GetPort())) {
			CloseLaunch();
		}
	}

	return 0;
}