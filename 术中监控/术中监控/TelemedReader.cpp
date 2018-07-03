#include <assert.h>
#include "Business.h"
#include "TelemedReader.h"

CTelemedReader::CTelemedReader(CBusiness * pBusiness) : m_pBusiness(pBusiness) {
	m_eStatus = STATUS_CLOSE;
	m_hComm = 0;
}

CTelemedReader::~CTelemedReader() {
	Clear();
}

void CTelemedReader::Clear() {
	m_received_data.Clear();
}

CTelemedReader::READER_STATUS CTelemedReader::GetStatus() {
	return m_eStatus;
}

int  CTelemedReader::Reconnect() {
	// 如果已经连接上
	if (m_eStatus == STATUS_OPEN) {
		return 0;
	}

	char szPort[256];
	g_cfg->GetConfig("com port", szPort, sizeof(szPort), "");

	std::vector<std::string>  vCom;
	if (szPort[0] != '\0') {
		vCom.push_back(szPort);
	}
	else {
		// 获取所有的串口信息
		GetAllSerialPortName(vCom);
	}

	

	// 先关闭之前可能打开的串口
	CloseUartPort();

	BOOL  bPrepared = FALSE;
	// 尝试打开从系统获得的串口列表
	do
	{
		std::vector<std::string>::iterator it;
		for (it = vCom.begin(); it != vCom.end(); it++) {
			std::string  sItem = *it;

			g_log->Output(ILog::LOG_SEVERITY_INFO, "try port %s \n", sItem.c_str());

			BOOL bRet = OpenUartPort(sItem.c_str());
			if (!bRet) {
				g_log->Output(ILog::LOG_SEVERITY_INFO, "port %s failed \n", sItem.c_str());
				CloseUartPort();
				continue;
			}

			Prepare();
			int ret = ReadPrepareRet();
			if (0 == ret) {
				bPrepared = TRUE;
				break;
			}

			g_log->Output(ILog::LOG_SEVERITY_INFO, "port %s failed \n", sItem.c_str());
			CloseUartPort();
		}
	} while (0);

	if (bPrepared) {
		m_eStatus = STATUS_OPEN;
		m_pBusiness->NotifyUiReaderStatus(m_eStatus);
	}
	else {
		m_pBusiness->ReconnectReaderAsyn(RECONNECT_READER_DELAY);
	}

	return 0;
}


// 读取温度
int CTelemedReader::ReadTagTemp(DWORD & dwTemp) {

	if (m_eStatus == STATUS_CLOSE) {
		return EXH_ERR_READER_CLOSE;
	}

	// g_log->Output(ILog::LOG_SEVERITY_INFO, "send get temperature command \n");

	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort(m_hComm, READ_TAG_DATA_COMMAND.abyCommand, READ_TAG_DATA_COMMAND.dwCommandLength, &dwWrited);

	if (!bRet) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to send read tag command! \n");

		m_eStatus = STATUS_CLOSE;
		m_pBusiness->NotifyUiReaderStatus(m_eStatus);
		m_pBusiness->ReconnectReaderAsyn(RECONNECT_READER_DELAY);
		return EXH_ERR_READER_FAILED_TO_WRITE;
	}

#ifdef TELEMED_READER_TYPE_1
	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 14);
#else
	DWORD dwReceived = 0;
	do
	{
		Sleep(SERIAL_PORT_SLEEP_TIME);
		Receive(dwReceived);
		// 如果再无数据了
		if (0 == dwReceived) {
			break;
		}
	} while (TRUE);
#endif

	// g_log->Output(ILog::LOG_SEVERITY_INFO, "RESPONSE OR TIMEOUT \n");

	BYTE pData[8192];
#ifdef TELEMED_READER_TYPE_1
	if (m_received_data.GetDataLength() >= 14) {
		m_received_data.Read(pData, 14);
		if (pData[0] == 0xFF && pData[13] == 0xFF) {
			dwTemp = pData[1] * 1000 + pData[2] * 100  + pData[3] * 10 + pData[4];
			return 0;
		}
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to receive temp data, wrong format! \n");
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to receive temp data, received data length =%lu \n", m_received_data.GetDataLength());
	}
#else
	// 02 12 04 0E 10 02 E0 02 59 CD 93 D9 3D 5E 21 5E 01 0D 0A 
	if (m_received_data.GetDataLength() >= 19) {
		m_received_data.Read(pData, 19);
		// 如果最后两个字节是0D 0A，则OK
		if (0 == memcmp(pData + 17, READER_TAIL, 2)) {
			dwTemp = pData[14] * 100 + pData[15];
			//m_received_data.Reform();
			return 0;
		}

		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to receive temp data, wrong format! \n");
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to receive temp data, received data length =%lu \n", m_received_data.GetDataLength() );
	}
#endif

	m_eStatus = STATUS_CLOSE;
	m_pBusiness->NotifyUiReaderStatus(m_eStatus);
	m_pBusiness->ReconnectReaderAsyn(RECONNECT_READER_DELAY);

	//m_received_data.Reform();
	return EXH_ERR_READER_TIMEOUT_OR_WRONG_FORMAT; 
}








// 打开串口
BOOL   CTelemedReader::OpenUartPort(const char *UartPortName) {
	BOOL bResult = TRUE;
	if (0 == UartPortName) {
		return FALSE;
	}

	if (m_hComm != 0) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "Open serial port failed, please close first\n");
		return FALSE;
	}

	char szPortName[256];
	_snprintf_s(szPortName, sizeof(szPortName), "\\\\.\\%s", UartPortName);

	HANDLE hComm = CreateFile(
		szPortName,
		GENERIC_WRITE | GENERIC_READ, //访问权限  
		0,                            //不共享  
		NULL,                         //返回的句柄不允许被子进程继承  
		OPEN_EXISTING,
		0,                            //0：同步模式，FILE_FLAG_OVERLAPPED：异步模式  
		0                             //不使用临时文件句柄  
	);

	if (INVALID_HANDLE_VALUE == hComm)
	{
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "Open Failed!!!\n");
		bResult = FALSE;
	}
	else
	{
		if (InitUartPort(hComm, 9600, 8, NOPARITY, ONESTOPBIT))
		{
			m_hComm = hComm;
			g_log->Output(ILog::LOG_SEVERITY_INFO, "Init Uart Port OK!!!\n");

			COMMTIMEOUTS  timeout;
			memset(&timeout, 0, sizeof(COMMTIMEOUTS));
			GetCommTimeouts(hComm, &timeout);
			// 写操作2秒 timeout
			timeout.WriteTotalTimeoutConstant = 2000;
			SetCommTimeouts(hComm, &timeout);
		}
		else
		{
			g_log->Output(ILog::LOG_SEVERITY_ERROR, "Init Uart Port Failed!!!\n");
			bResult = FALSE;
		}
	}

	return bResult;
}

// 关闭串口
BOOL   CTelemedReader::CloseUartPort() {
	if (0 != m_hComm) {
		CloseHandle(m_hComm);
		m_hComm = 0;
	}
	m_received_data.Clear();
	return TRUE;
}

BOOL   CTelemedReader::InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits) {
	BOOL bResult = true;

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

BOOL   CTelemedReader::WriteUartPort(HANDLE hComm, const void * WriteBuf, DWORD ToWriteDataLen, DWORD *WritedDataLen) {
	DWORD     dwError = 0;
	COMSTAT   ComStat;
	if (0 == hComm) {
		return FALSE;
	}

	ClearCommError(hComm, &dwError, &ComStat);//返回串口错误和报告(也可以查看缓冲区状态)
	BOOL bWriteStat = WriteFile(hComm, WriteBuf, ToWriteDataLen, WritedDataLen, NULL);
	if (!bWriteStat)
	{
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to write com port!\n");
		return FALSE;
	}

	return TRUE;
}

BOOL   CTelemedReader::RcvDataFromUartPort(HANDLE hComm, void *RcvBuf, DWORD ToRcvDataLen, DWORD *RcvedDataLen, LPOVERLAPPED lpOverlapped) {
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

int   CTelemedReader::Receive(DWORD & dwReceivedCnt) {
	char buf[8192];
	dwReceivedCnt = 0;
	if (RcvDataFromUartPort(m_hComm, buf, sizeof(buf), &dwReceivedCnt, 0))
	{
		// 保存到缓冲中
		if (dwReceivedCnt) {
			m_received_data.Reform();
			m_received_data.Append(buf, dwReceivedCnt);
		}
	}
	return 0;
}





int  CTelemedReader::Prepare() {

	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort(m_hComm, PREPARE_COMMAND.abyCommand, PREPARE_COMMAND.dwCommandLength, &dwWrited);

	if (bRet) {
		return 0;
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to Prepare \n");
		return -1;
	}
}

int  CTelemedReader::ReadPrepareRet() {
	DWORD dwReceived = 0;

#ifdef TELEMED_READER_TYPE_1
	//ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 4);
	ReceiveAsPossible(5000, 4);
#else
	do
	{
		Sleep(SERIAL_PORT_SLEEP_TIME);
		Receive(dwReceived);
		// 如果再无数据了
		if (0 == dwReceived) {
			break;
		}
	} while (TRUE);
#endif

	BYTE pData[8192];
#ifdef TELEMED_READER_TYPE_1
	if (m_received_data.GetDataLength() >= 4) {
		m_received_data.Read(pData, 4);
		if (0 == memcmp(pData, "\xFF\x55\x55\xFF", 4)) {
			return 0;
		}
	}
#else
	if (m_received_data.GetDataLength() >= 20) {
		m_received_data.Read(pData, 20);
		// 如果最后两个字节是0D 0A，则OK
		if (0 == memcmp(pData + 18, READER_TAIL, 2)) {
			return 0;
		}
	}
#endif

	return -1;
}



void  CTelemedReader::ReceiveAsPossible( DWORD dwMaxTime, DWORD dwMaxDataLength ) {
	const DWORD  dwIntervalTime = 200;
	DWORD  dwLeftTime = dwMaxTime;

	DWORD  dwReceived = 0;
	DWORD  dwReceivedCnt = m_received_data.GetDataLength();

	do
	{
		// 如果达到数据长度要求
		if (dwReceivedCnt >= dwMaxDataLength) {
			break;
		}

		Sleep(dwIntervalTime);
		Receive(dwReceived);

		dwReceivedCnt += dwReceived;
		
		if ( dwLeftTime > dwIntervalTime ) {
			dwLeftTime -= dwIntervalTime;
		}
		// 时间已经用完
		else {
			break;
		}

	} while (TRUE);
}