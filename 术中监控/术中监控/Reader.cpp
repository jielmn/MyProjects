#include <assert.h>
#include "Business.h"
#include "Reader.h"

CReader::CReader(CBusiness * pBusiness) : m_pBusiness(pBusiness) {
	m_eStatus = STATUS_CLOSE;
	m_hComm = 0;

	memset(&m_tag, 0, sizeof(TagItem));
	m_bFoundTag = FALSE;
}

CReader::~CReader() {
	Clear();
}

void CReader::Clear() {

}

CReader::READER_STATUS CReader::GetStatus() {
	return m_eStatus;
}

int  CReader::Reconnect() {
	// 如果已经连接上
	if ( m_eStatus == STATUS_OPEN ) {
		return 0;
	}

	std::vector<std::string>  vCom;
	// 获取所有的串口信息
	GetAllSerialPortName(vCom);

	// 先关闭之前可能打开的串口
	CloseUartPort();

	BOOL  bPrepared = FALSE;
	// 尝试打开从系统获得的串口列表
	do
	{
		std::vector<std::string>::iterator it;
		for (it = vCom.begin(); it != vCom.end(); it++) {
			std::string  sItem = *it;

			BOOL bRet = OpenUartPort(sItem.c_str());
			if (!bRet) {
				CloseUartPort();
				continue;
			}

			Prepare();
			int ret = ReadPrepareRet();
			if (0 == ret) {
				bPrepared = TRUE;
				break;
			}
			CloseUartPort();
		}
	} while (0);

	if (bPrepared) {
		m_eStatus = STATUS_OPEN;
		m_pBusiness->NotifyUiReaderStatus(m_eStatus);
	}
	else {
		m_pBusiness->ReconnectReaderAsyn(10000);
	}

	return 0;
}



// 打开串口
BOOL   CReader::OpenUartPort(const char *UartPortName) {
	BOOL bResult = TRUE;
	if (0 == UartPortName) {
		return FALSE;
	}

	if (m_hComm != 0) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "Open serial port failed, please close first\n");
		return FALSE;
	}

	char szPortName[256];
	_snprintf_s( szPortName, sizeof(szPortName), "\\\\.\\%s", UartPortName );

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
		if (InitUartPort(hComm, 115200, 8, NOPARITY, ONESTOPBIT))
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

// 关闭串口
BOOL   CReader::CloseUartPort() {
	if (0 != m_hComm) {
		CloseHandle(m_hComm);
		m_hComm = 0;
	}
	m_received_data.Clear();
	return TRUE;
}

BOOL   CReader::InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits) {
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

BOOL   CReader::WriteUartPort(HANDLE hComm, const void * WriteBuf, DWORD ToWriteDataLen, DWORD *WritedDataLen) {
	DWORD     dwError = 0;
	COMSTAT   ComStat;
	if (0 == hComm) {
		return FALSE;
	}

	ClearCommError(hComm, &dwError, &ComStat);//返回串口错误和报告(也可以查看缓冲区状态)
	BOOL bWriteStat = WriteFile(hComm, WriteBuf, ToWriteDataLen, WritedDataLen, NULL);
	if (!bWriteStat)
	{
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "写串口失败!\n");
		return FALSE;
	}

	return TRUE;
}

BOOL   CReader::RcvDataFromUartPort(HANDLE hComm, void *RcvBuf, DWORD ToRcvDataLen, DWORD *RcvedDataLen, LPOVERLAPPED lpOverlapped) {
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

int   CReader::Receive(DWORD & dwReceivedCnt) {
	char buf[8192];
	dwReceivedCnt = 0;
	if (RcvDataFromUartPort(m_hComm, buf, sizeof(buf), &dwReceivedCnt, 0))
	{
		// 保存到缓冲中
		if (dwReceivedCnt) {
			m_received_data.Append(buf, dwReceivedCnt);
		}
	}
	return 0;
}

int   CReader::ReadToken(Token & t) {
	if ( m_received_data.GetDataLength() < 3 ) {
		return -1;
	}

	BYTE buf[8192];
	m_received_data.Read( buf, 3 );
	// 如果前三个字节不是 dd 11 ef
	if ( !(buf[0] == 0xdd && buf[1] == 0x11 && buf[2] == 0xef ) ) {
		return -1;
	}

	// 数据长度
	m_received_data.Read( &t.byDataLen, 1);

	// 如果数据长度不对，或者超过Token定义的最大长度
	if ( t.byDataLen < 8  ) {
		return -1;
	}

	if ( (t.byDataLen - 8) > MAX_TOKEN_LENGTH ) {
		return -2;
	}
	t.byDataLen -= 8;

	// 通道地址
	if ( m_received_data.GetDataLength() == 0 ) {
		return -1;
	}	
	m_received_data.Read(&t.byChannel, 1);

	// 命令字
	if (m_received_data.GetDataLength() == 0) {
		return -1;
	}	
	m_received_data.Read(&t.byOperation, 1);

	// 数据
	if (m_received_data.GetDataLength() < t.byDataLen ) {
		return -1;
	}
	m_received_data.Read( t.abyData, (DWORD)t.byDataLen );

	// 最后2字节校验码
	if (m_received_data.GetDataLength() < 2) {
		return -1;
	}
	m_received_data.Read(buf, 2);
	m_received_data.Reform();

	return 0;
}

int  CReader::Prepare() {

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

int  CReader::ReadPrepareRet() {
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

	Token  t;
	int ret = ReadToken(t);
	while (0 == ret) {
		// 如果是清点命令
		if ( t.byOperation == 0x01 ) {
			// 如果是4字节数据
			if (t.byDataLen == 4) {
				// 如果状态字是环境噪音，读取完成，场内无标签，读取失败
				if ( t.abyData[0] == STATUS_NOISE || t.abyData[0] == STATUS_COMPLETE || t.abyData[0] == STATUS_NO_TAG || t.abyData[0] == STATUS_FAIL ) {
					return 0;
				}
			}
		}
		ret = ReadToken(t);
	}

	return -1;
}

int   CReader::Inventory() {
	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort(m_hComm, INVENTORY_COMMAND.abyCommand, INVENTORY_COMMAND.dwCommandLength, &dwWrited);

	if (bRet) {
		return 0;
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to Inventory \n");
		m_eStatus = STATUS_CLOSE;
		return EXH_ERR_READER_FAILED_TO_WRITE;
	}
}

int   CReader::ReadInventoryRet() {
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

	Token  t;
	int ret = ReadToken(t);
	while (0 == ret) {
		// 如果是清点命令
		if (t.byOperation == 0x01 ) {
			// 如果是4字节数据
			if (t.byDataLen == 4) {
				// 如果状态字是环境噪音，读取完成，场内无标签，读取失败
				if (t.abyData[0] == STATUS_NOISE || t.abyData[0] == STATUS_COMPLETE || t.abyData[0] == STATUS_NO_TAG || t.abyData[0] == STATUS_FAIL) {
					return 0;
				}
			}
			//  Tag数据（每次保存最后一个）
			else if (t.byDataLen == 11) {
				m_tag.byUidLen = 8;
				memcpy(m_tag.abyUid, t.abyData + 3, 8);
				m_bFoundTag = TRUE;
			}
		}
		ret = ReadToken(t);
	}

	g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to ReadInventoryRet \n");
	m_eStatus = STATUS_CLOSE;
	return EXH_ERR_READER_TIMEOUT_OR_WRONG_FORMAT;
}

int   CReader::ReadTagBlock(int nStartBlock, int nReadLength) {
	//assert(m_bFoundTag);
	//assert(m_tag.byUidLen == 8);

	//// DD 11 EF 13 00 23    01 E0 02 59 D8 37 0E 43 98 7F 01     65 3B
	//BYTE   abyData[256];
	//DWORD  dwDataLen = 0;
	//memcpy(abyData, READ_TAG_DATA_COMMAND.abyCommand, READ_TAG_DATA_COMMAND.dwCommandLength);
	//dwDataLen += READ_TAG_DATA_COMMAND.dwCommandLength;

	//abyData[dwDataLen] = 0x01;
	//dwDataLen++;

	//memcpy(abyData + dwDataLen, m_tag.abyUid, m_tag.byUidLen);
	//dwDataLen += m_tag.byUidLen;

	//abyData[dwDataLen] = (BYTE)nStartBlock;
	//dwDataLen++;

	//abyData[dwDataLen] = (BYTE)nReadLength;
	//dwDataLen++;

	//memcpy(abyData + dwDataLen, "\x00\x00", 2);
	//dwDataLen += 2;

	//assert(dwDataLen == 19);

	//DWORD dwWrited = 0;
	//BOOL bRet = WriteUartPort(m_hComm, abyData, dwDataLen, &dwWrited);

	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort(m_hComm, READ_TAG_DATA_COMMAND.abyCommand, READ_TAG_DATA_COMMAND.dwCommandLength, &dwWrited);

	if (bRet) {
		return 0;
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to ReadTagBlock \n");
		m_eStatus = STATUS_CLOSE;
		return EXH_ERR_READER_FAILED_TO_WRITE;
	}
}

int   CReader::ReadTagBlockRet(std::vector<TagBlock*> &v) {
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

	char debug_buf[8192];
	DebugStream(debug_buf, sizeof(debug_buf), m_received_data.GetData(), m_received_data.GetDataLength());
	g_log->Output(ILog::LOG_SEVERITY_INFO, "%s\n", debug_buf);

	Token  t;
	int ret = ReadToken(t);
	while (0 == ret) {
		// 如果是获取Tag block命令
		if (t.byOperation == 0x23) {
			// 如果是1字节数据
			if (t.byDataLen == 4) {
				// 如果读取失败
				if (t.abyData[0] == STATUS_FAIL || t.abyData[0] == STATUS_NO_TAG) {
					return 0;
				}
			}
			//  Tag block
			else {
				if (t.byDataLen < 2) {
					break;
				}

				BYTE  byBlockCnt = t.abyData[0];
				BYTE  byBlockSize = t.abyData[1];
				assert(4 == byBlockSize);
				assert(byBlockCnt > 0);

				if ( (t.byDataLen - 2) != byBlockCnt * (4 + 1) ) {
					break;
				}
				
				for (BYTE i = 0; i < byBlockCnt; i++) {
					TagBlock* pBlock = new TagBlock;
					memcpy(pBlock, t.abyData + 2 + 5 * i + 1, 4);
					v.push_back(pBlock);
				}
				return 0;
			}
		}
		ret = ReadToken(t);
	}

	g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to ReadTagBlockRet \n");
	m_eStatus = STATUS_CLOSE;
	return EXH_ERR_READER_TIMEOUT_OR_WRONG_FORMAT;
}

// 获取Tag温度数据
int  CReader::GetTagTemperatureData() {
	int ret = 0;
	char buf[8192];

	g_log->Output(ILog::LOG_SEVERITY_INFO, " CReader::GetTagTemperatureData...\n");

	if ( m_eStatus == STATUS_CLOSE ) {

		g_log->Output(ILog::LOG_SEVERITY_INFO, " close status \n");
		return EXH_ERR_READER_CLOSE;
	}

	do 
	{
		// 如果没有获取tag
		//if (!m_bFoundTag) {
		//	ret = Inventory();
		//	if (0 != ret) {
		//		break;
		//	}

		//	ret = ReadInventoryRet();
		//	if (0 != ret) {
		//		break;
		//	}

		//	// 还是没有找到Tag
		//	if ( !m_bFoundTag ) {
		//		g_log->Output(ILog::LOG_SEVERITY_INFO, "NOT found tag \n");
		//		ret = EXH_ERR_NOT_FOUND_TAG;
		//		break;
		//	}
		//}

		// 查询Tag的温度数据
		ret = ReadTagBlock(127, 1);
		if (0 != ret) {
			break;
		}

		std::vector<TagBlock*> v;
		ret = ReadTagBlockRet(v);
		if ( 0 != ret ) {
			break;
		}

		// 读取温度数据失败
		if (0 == v.size()) {
			g_log->Output(ILog::LOG_SEVERITY_INFO, "block size is zero \n");
			break;
		}

		// 如果block 个数不是1
		if ( v.size() != 1 ) {
			g_log->Output(ILog::LOG_SEVERITY_INFO, "block size != 1, block size is %lu \n", v.size());
			m_eStatus = STATUS_CLOSE;
			ret = EXH_ERR_READER_TIMEOUT_OR_WRONG_FORMAT;
			break;
		}

		TagBlock * pBlock = v[0];
		// 从4字节buffer获取温度
		DebugStream( buf, sizeof(buf), pBlock->abyData, sizeof(pBlock->abyData) );
		g_log->Output(ILog::LOG_SEVERITY_INFO, "get temp data is: \n", buf);
		g_log->Output(ILog::LOG_SEVERITY_INFO, "%s \n", buf);

		ClearVector(v);

	} while ( 0 );

	if (m_eStatus == STATUS_CLOSE) {
		m_pBusiness->NotifyUiReaderStatus(m_eStatus);
		m_pBusiness->ReconnectReaderAsyn(10000);
	}
	
	return 0;
}