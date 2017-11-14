#include "stdafx.h"
#include "SerialPort.h"

// 一个温度数据20字节
#define  TAG_DATA_SIZE                  20

CSerialPort::CSerialPort()
{
	m_hComm = 0;
	m_eMainStatus = COM_STATUS_CLOSED;
	m_eSubStatus  = COM_STATUS_IDLE;
	m_bPrepared = FALSE;
}

CSerialPort::~CSerialPort() {
	m_received_data.Clear();
}

ComStatus  CSerialPort::GetMainStatus() const {
	return m_eMainStatus;
}

ComStatus  CSerialPort::GetSubStatus() const {
	return m_eSubStatus;
}

BOOL   CSerialPort::IfPrepared() const {
	return m_bPrepared;
}

BOOL CSerialPort::InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits) {
	BOOL bResult = true;
	char buffer[50] = "";

	//设置接收缓冲区和输出缓冲区的大小  
	DWORD dwInQueue = 1024;
	DWORD dwOutQueue = 1024;
	if (!SetupComm(hComm, dwInQueue, dwOutQueue))
	{
		bResult = false;
	}
	else
	{
		//设置读写的超时时间  以毫秒计算  
		COMMTIMEOUTS timeouts;
		//for read ReadTotalTimeouts = ReadTotalTimeoutMultiplier * ToReadByteNumber + ReadTotalTimeoutConstant，  
		timeouts.ReadIntervalTimeout = MAXDWORD;                    //接收两个字符之间的最长超时时间  
		timeouts.ReadTotalTimeoutMultiplier = 0;            //与读取要读字节数相乘的系数  
		timeouts.ReadTotalTimeoutConstant = 0;              //读取总超时时间常量  
															//for write WriteTotalTimeouts = WriteTotalTimeoutMultiplier * ToWriteByteNumber + WriteTotalTimeoutConstant  
															//timeouts.WriteTotalTimeoutMultiplier = 0;  
															//timeouts.WriteTotalTimeoutConstant = 0;  

		if (!SetCommTimeouts(hComm, &timeouts))
		{
			bResult = false;
		}
		else
		{
			DCB dcb;
			if (!GetCommState(hComm, &dcb))
			{
				bResult = false;
			}
			else
			{
				memset(&dcb, 0, sizeof(dcb));
				dcb.BaudRate = BaudRate;
				dcb.ByteSize = ByteSize;
				dcb.Parity = Parity;
				dcb.StopBits = StopBits;
				dcb.XonChar = 0;

				if (!SetCommState(hComm, &dcb))
				{
					bResult = false;
				}
				else
				{
					//清空接收缓存和输出缓存的buffer  
					if (!PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR))
					{
						//PrintLogCom("Clean up in buffer and out buffer Failed!!!\n");  
						bResult = false;
					}
					else
					{
						//PrintLogCom("Clean up in buffer and out buffer OK!!!\n");  
					}
				}
			}
		}
	}

	return bResult;
}

BOOL CSerialPort::OpenUartPort(char *UartPortName)
{
	BOOL bResult = TRUE;
	if (0 == UartPortName) {
		return FALSE;
	}

	if (m_hComm != 0) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "Open serial port failed, please close first\n");
		return FALSE;
	}

	HANDLE hComm = CreateFile(
		UartPortName,
		GENERIC_WRITE | GENERIC_READ, //访问权限  
		0,                            //不共享  
		NULL,                         //返回的句柄不允许被子进程继承  
		OPEN_EXISTING,
		0,                            //0：同步模式，FILE_FLAG_OVERLAPPED：异步模式  
		0                             //不使用临时文件句柄  
	);

	if (INVALID_HANDLE_VALUE == hComm)
	{
		g_log->Output( ILog::LOG_SEVERITY_ERROR, "Open Failed!!!\n" );
		bResult = FALSE;
	}
	else
	{
		if (InitUartPort(hComm, 9600, 8, NOPARITY, ONESTOPBIT))
		{
			m_hComm = hComm;
			g_log->Output(ILog::LOG_SEVERITY_INFO, "Init Uart Port OK!!!\n");
		}
		else
		{
			g_log->Output(ILog::LOG_SEVERITY_ERROR, "Init Uart Port Failed!!!\n");
			bResult = FALSE;
		}
	}

	return bResult;
}

BOOL   CSerialPort::WriteUartPort(HANDLE hComm, const void * WriteBuf, DWORD ToWriteDataLen, DWORD *WritedDataLen) {
	DWORD     dwError = 0;
	COMSTAT   ComStat;
	if (0 == hComm) {
		return FALSE;
	}

	ClearCommError(hComm, &dwError, &ComStat);//返回串口错误和报告(也可以查看缓冲区状态)
	BOOL bWriteStat = WriteFile( hComm, WriteBuf, ToWriteDataLen, WritedDataLen, NULL );
	if (!bWriteStat)
	{
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "写串口失败!\n");
		return FALSE;
	}

	return TRUE;
}

BOOL  CSerialPort::RcvDataFromUartPort(HANDLE hComm, void *RcvBuf, DWORD ToRcvDataLen, DWORD *RcvedDataLen, LPOVERLAPPED lpOverlapped) {
	BOOL bResult = TRUE;
	DWORD dwTempRcvedDataLen = 0;
	DWORD dwError;

	if (0 == hComm) {
		return FALSE;
	}

	if (ClearCommError(hComm, &dwError, NULL))
	{
		PurgeComm(hComm, PURGE_TXABORT | PURGE_TXCLEAR);
	}

	if (hComm != INVALID_HANDLE_VALUE)
	{
		if (!ReadFile(hComm, RcvBuf, ToRcvDataLen, &dwTempRcvedDataLen, lpOverlapped))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				while (!GetOverlappedResult(hComm, lpOverlapped, &dwTempRcvedDataLen, FALSE))
				{
					if (GetLastError() == ERROR_IO_INCOMPLETE)
					{
						continue;
					}
					else
					{
						ClearCommError(hComm, &dwError, NULL);
						bResult = FALSE;
						break;
					}
				}
			}
		}
	}
	else
	{
		bResult = FALSE;
	}
	*RcvedDataLen = dwTempRcvedDataLen;
	return bResult;
}

int   CSerialPort::Init() {
	// 已经打开状态
	if (COM_STATUS_OPEN == m_eMainStatus) {
		return -1;
	}

	char buf[8192] = { 0 };
	g_cfg->GetConfig("serial port", buf, sizeof(buf), "com1");

	if (!OpenUartPort(buf)) {
		m_eMainStatus = COM_STATUS_CLOSED;
		g_log->Output( ILog::LOG_SEVERITY_ERROR, "failed to open uport\n");
		return -1;
	}
	
	m_eMainStatus = COM_STATUS_OPEN;
	m_eSubStatus = COM_STATUS_IDLE;
	m_bPrepared = FALSE;
	m_received_data.Clear();
	return 0;
}

int   CSerialPort::Deinit() {
	if (0 != m_hComm) {
		CloseHandle(m_hComm);
		m_hComm = 0;
	}

	m_eMainStatus = COM_STATUS_CLOSED;
	m_eSubStatus = COM_STATUS_IDLE;
	m_bPrepared = FALSE;
	m_received_data.Clear();
	return 0;
}

int   CSerialPort::Sync() {

	// 已经打开状态
	if (COM_STATUS_OPEN != m_eMainStatus) {
		return -1;
	}

	if (COM_STATUS_IDLE != m_eSubStatus) {
		return -1;
	}

	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort( m_hComm, SYNC_COMMAND.abyCommand, SYNC_COMMAND.dwCommandLength, &dwWrited );

	if (bRet) {
		m_eSubStatus = COM_STATUS_UPLOADING;
		return 0;
	}
	else {		
		g_log->Output( ILog::LOG_SEVERITY_ERROR, "failed to Upload \n");
		Deinit();
		return -1;
	}
}

int   CSerialPort::Prepare() {
	// 已经打开状态
	if (COM_STATUS_OPEN != m_eMainStatus) {
		return -1;
	}

	if (COM_STATUS_IDLE != m_eSubStatus) {
		return -1;
	}

	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort(m_hComm, PREPARE_COMMAND.abyCommand, PREPARE_COMMAND.dwCommandLength, &dwWrited);

	if (bRet) {
		m_eSubStatus = COM_STATUS_PREPARING;
		return 0;
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to Prepare \n");
		Deinit();
		return -1;
	}
}

int  CSerialPort::ClearReader() {
	// 已经打开状态
	if (COM_STATUS_OPEN != m_eMainStatus) {
		return -1;
	}

	if (COM_STATUS_IDLE != m_eSubStatus) {
		return -1;
	}

	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort(m_hComm, CLEAR_COMMAND.abyCommand, CLEAR_COMMAND.dwCommandLength, &dwWrited);

	if (bRet) {
		m_eSubStatus = COM_STATUS_CLEARING;
		return 0;
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to Clear \n");
		Deinit();
		return -1;
	}
}

int   CSerialPort::Receive(DWORD & dwReceivedCnt) {
	// 已经打开状态
	if (COM_STATUS_OPEN != m_eMainStatus) {
		return -1;
	}

	char buf[8192];
	if ( RcvDataFromUartPort( m_hComm, buf, sizeof(buf), &dwReceivedCnt, 0) )
	{
		// 保存到缓冲中
		if (dwReceivedCnt) {
			m_received_data.Append(buf, dwReceivedCnt);
		}		
	}

	return 0;
}

int  CSerialPort::FormatData(std::vector<TagData*> & v) {
	// 已经打开状态
	if (COM_STATUS_OPEN != m_eMainStatus) {
		m_received_data.Clear();
		return -1;
	}

	// 同步命令后的数据
	if (COM_STATUS_UPLOADING == m_eSubStatus) {
		if (m_received_data.GetDataLength() % TAG_DATA_SIZE != 0) {
			g_log->Output(ILog::LOG_SEVERITY_ERROR, "同步数据的结果不是20字节的整数倍，实际字节数=%d \n", m_received_data.GetDataLength());
			m_received_data.Clear();
			return -1;
		}

		char tmp[256];
		g_cfg->GetConfig("use system clock", tmp, sizeof(tmp), "false");
		BOOL bUseSystemClock = GetBoolean(tmp);

		// tag 温度数据的个数
		int nCnt = m_received_data.GetDataLength() / TAG_DATA_SIZE;
		BYTE buf[TAG_DATA_SIZE];

		for (int i = 0; i < nCnt; i++) {
			m_received_data.Read(buf, TAG_DATA_SIZE);

			int nOffset = 0;
			if (buf[0] == '\x02') {
				nOffset = 1;
			}
			else if (buf[0] == '\x0a' && buf[1] == '\x02') {
				nOffset = 2;
			}
			// 格式错误
			else {
				g_log->Output(ILog::LOG_SEVERITY_ERROR, "同步数据的结果格式错误：不是02或0a 02开头 \n");
				m_received_data.Clear();
				return -1;
			}

			BYTE * pBuf = buf + nOffset;

			TagData * pItem = new TagData;

			struct tm timeinfo;
			// 5个字节是时间
			// 如果使用系统时间
			if (bUseSystemClock) {
				time_t tTmp = time(0);
				localtime_s(&timeinfo, &tTmp);
				timeinfo.tm_hour = pBuf[3];
				timeinfo.tm_min = pBuf[4];
				timeinfo.tm_sec = 0;
				pItem->tTime = mktime(&timeinfo);
			}
			// 使用reader的时间
			else {
				timeinfo.tm_year = 2000 + pBuf[0] - 1900;
				timeinfo.tm_mon =  pBuf[1] - 1;
				timeinfo.tm_mday = pBuf[2];
				timeinfo.tm_hour = pBuf[3];
				timeinfo.tm_min = pBuf[4];
				timeinfo.tm_sec = 0;
				pItem->tTime = mktime(&timeinfo);
			}
			// 偏移5个字节
			pBuf += 5;

			// 8个字节是Tag ID
			memcpy(pItem->abyTagId, pBuf, 8);
			pBuf += 8;

			// 2个字节是温度
			pItem->dwTemperatur = pBuf[0] * 100 + pBuf[1];
			pBuf += 2;

			// 2个字节是个数
			DWORD dwCount = pBuf[0] * 256 + pBuf[1];
			if (dwCount == 0) {
				delete pItem;
				if (nCnt != 1) {
					g_log->Output(ILog::LOG_SEVERITY_ERROR, "同步数据的结果格式错误：根据总字节计算是%d个，数据内标识是%d个 \n", nCnt, (int)dwCount);
					return -1;
				}
				break;
			} else if (dwCount != nCnt) {
				g_log->Output(ILog::LOG_SEVERITY_ERROR, "同步数据的结果格式错误：根据总字节计算是%d个，数据内标识是%d个 \n", nCnt, (int)dwCount );
				delete pItem;
				ClearVector(v);
				m_received_data.Clear();
				return -1;
			}
			pBuf += 2;

			v.push_back(pItem);
		}
	}

	m_eSubStatus = COM_STATUS_IDLE;
	m_received_data.Clear();
	return 0;
}

int  CSerialPort::FormatData() {
	if (m_eSubStatus == COM_STATUS_PREPARING) {
		if (m_received_data.GetDataLength() != TAG_DATA_SIZE ) {
			g_log->Output(ILog::LOG_SEVERITY_ERROR, "Prepare数据的结果不是20字节，实际字节数是%d \n", m_received_data.GetDataLength());
			m_received_data.Clear();
			return -1;
		}
		m_bPrepared = TRUE;
	}
	else if (m_eSubStatus == COM_STATUS_CLEARING) {
		if (m_received_data.GetDataLength() != TAG_DATA_SIZE) {
			g_log->Output(ILog::LOG_SEVERITY_ERROR, "Clear数据的结果不是20字节，实际字节数是%d \n", m_received_data.GetDataLength());
			m_received_data.Clear();
			return -1;
		}
	}

	m_eSubStatus = COM_STATUS_IDLE;
	m_received_data.Clear();
	return 0;
}