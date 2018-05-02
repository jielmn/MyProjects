#include <afx.h>
#include "Business.h"
#include "zsSyncReader.h"

#define   READER_VERSION          1

// 遍历系统里的所有串口
static BOOL GetAllSerialPortName(std::vector<std::string> & vCom) {

	// TODO: 在此添加控件通知处理程序代码
	HKEY hKey = NULL;
	vCom.clear();

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	CString valuename, databuffer;
	DWORD valuenamebufferlength = 200, valuetype, databuddersize = 200;

	int i = 0;
	while (RegEnumValue(hKey, i++, valuename.GetBuffer(200), &valuenamebufferlength, NULL, &valuetype, (BYTE*)databuffer.GetBuffer(200), &databuddersize) != ERROR_NO_MORE_ITEMS)
	{
		std::string com_name = databuffer;
		vCom.push_back(com_name);

		databuddersize = 200;
		valuenamebufferlength = 200;
	}

	RegCloseKey(hKey);
	return TRUE;
}


CZsSyncReader::CZsSyncReader(CBusiness * pBusiness) : m_pBusiness(pBusiness) {
	m_eReaderStatus = STATUS_CLOSE;
	m_hComm = 0;
}

CZsSyncReader::~CZsSyncReader() {

}

void CZsSyncReader::Clear() {

}

// 重连(连接成功的定义是：底层的串口连接成功，并且和Reader的通讯的prepare成功)
int CZsSyncReader::Reconnect() {

	// 如果已经连接成功
	if (STATUS_OPEN == m_eReaderStatus) {
		return 0;
	}

	BOOL  bRet = FALSE;
	int   ret = 0;
	BOOL  bPrepared = FALSE;

	std::vector<std::string>  vCom;
	// 获取所有的串口信息
	GetAllSerialPortName(vCom);

	// 先关闭之前可能打开的串口
	CloseUartPort();

	// 尝试打开从系统获得的串口列表
	do
	{
		std::vector<std::string>::iterator it;
		for (it = vCom.begin(); it != vCom.end(); it++) {
			std::string  sItem = *it;
	
			bRet = OpenUartPort(sItem.c_str() );
			if (!bRet) {
				CloseUartPort();
				continue;
			}

			Prepare();
			ret = ReadPrepareRet();
			if (0 == ret) {
				bPrepared = TRUE;
				break;
			}
			CloseUartPort();
		}
	} while (0);

	if (bPrepared) {
		m_eReaderStatus = STATUS_OPEN;
		m_pBusiness->NotifyUiSyncReaderStatus(m_eReaderStatus);
		// 延时检查心跳
		m_pBusiness->CheckSyncReaderHeartBeatAsyn(SYNC_READER_HEART_BEAT_TIME);
	}
	else {
		// 连接，重新连接
		m_pBusiness->ReconnectSyncReaderAsyn(RECONNECT_SYNC_READER_TIME);
	}

	return 0;
}

CZsSyncReader::SYNC_READER_STATUS CZsSyncReader::GetStatus() {
	return m_eReaderStatus;
}


// 打开串口
BOOL  CZsSyncReader::OpenUartPort(const char *UartPortName) {
	BOOL bResult = TRUE;
	if (0 == UartPortName) {
		return FALSE;
	}

	if (m_hComm != 0) {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "Open serial port failed, please close first\n");
		return FALSE;
	}

	char szComName[256];
	snprintf(szComName, sizeof(szComName), "\\\\.\\%s", UartPortName);

	HANDLE hComm = CreateFile(
		szComName,
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
BOOL  CZsSyncReader::CloseUartPort() {
	if (0 != m_hComm) {
		CloseHandle(m_hComm);
		m_hComm = 0;
	}
	m_received_data.Clear();
	return TRUE;
}

BOOL  CZsSyncReader::InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits) {
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

BOOL  CZsSyncReader::WriteUartPort(HANDLE hComm, const void * WriteBuf, DWORD ToWriteDataLen, DWORD *WritedDataLen) {
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

BOOL  CZsSyncReader::RcvDataFromUartPort(HANDLE hComm, void *RcvBuf, DWORD ToRcvDataLen, DWORD *RcvedDataLen, LPOVERLAPPED lpOverlapped) {
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

int  CZsSyncReader::Prepare() {

	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort(m_hComm, PREPARE_COMMAND.abyCommand, PREPARE_COMMAND.dwCommandLength, &dwWrited);

	if (bRet) {
		return 0;
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to Prepare \n");
		return ZS_ERR_SYNC_READER_FAILED_TO_WRITE;
	}
}

int  CZsSyncReader::ReadPrepareRet() {
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

	BYTE pData[8192];

	if (m_received_data.GetDataLength() >= 20) {
		m_received_data.Read(pData, 20);
		// 如果最后两个字节是0D 0A，则OK
		if (0 == memcmp(pData + 18, READER_TAIL, 2)) {
			return 0;
		}
	}

	return ZS_ERR_SYNC_READER_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
}

int   CZsSyncReader::SyncData() {
	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort(m_hComm, SYNC_COMMAND.abyCommand, SYNC_COMMAND.dwCommandLength, &dwWrited);

	if (bRet) {
		return 0;
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to Sync command \n");
		return ZS_ERR_SYNC_READER_FAILED_TO_WRITE;
	}
}

int   CZsSyncReader::ReadSyncDataRet( std::vector<SyncItem*> & v ) {
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

	BYTE pData[8192];

	// 最少28字节：                   02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0D 0A
	// 通常32字节(多了4字节ReaderId)：02 12 03 16 11 37 E0 02 59 CD 93 D9 3D 5E 15 21 00 01 E0 04 01 00 A4 79 F3 90 A0 00 00 01 0D 0A

	if (m_received_data.GetDataLength() < 28) {
		return ZS_ERR_SYNC_READER_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
	}

	// 读取第一个Item，里面有总数
	m_received_data.Read(pData, 28);

#if READER_VERSION == 0
	// 如果第17和18字节是 00 00，并且最后两个字节是0D 0A，则OK
	if ( (0 == memcmp(pData + 16, "\x00\x00", 2)) /*&& (0 == memcmp(pData + 26, READER_TAIL, 2))*/ ) {
		return 0;
	}
#else
	if ( 0 == memcmp(pData + 16, "\x00\x00", 2) ) {
		return 0;
	}
#endif

#if READER_VERSION == 0
	// 再读取4个字节
	m_received_data.Read(pData+28, 4);
	// 如果最后两个字节不是 0D 0A
	if ( 0 != memcmp(pData + 30, READER_TAIL, 2) ) {
		return ZS_ERR_SYNC_READER_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
	}
#else
	if (0 != memcmp(pData + 26, READER_TAIL, 2)) {
		return ZS_ERR_SYNC_READER_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
	}
#endif

	DWORD   dwItemCounts = 0;
	dwItemCounts = pData[16] * 256 + pData[17];

	SyncItem * pItem = new SyncItem;
	memset(pItem, 0, sizeof(SyncItem));

	// 时间
	pItem->tTime = GetTelemedTagDate(pData + 1, 5);
	//  tag id
	memcpy(pItem->tTagId.abyUid, pData + 6, 8);
	pItem->tTagId.dwUidLen = 8;
	// 温度
	pItem->dwTemperature = pData[14] * 100 + pData[15];
	// 护士id
	memcpy(pItem->tNurseId.abyUid, pData + 18, 8);
	pItem->tNurseId.dwUidLen = 8;

#if READER_VERSION == 0
	// reader id
	memcpy(pItem->tReaderId.abyUid, pData + 26, 4);
	pItem->tReaderId.dwUidLen = 4;
#else
	memcpy(pItem->tReaderId.abyUid, "\x00\x00\x00\x00", 4);
	pItem->tReaderId.dwUidLen = 4;
#endif

	v.push_back(pItem);

	DWORD  dwIndex = 1;

	// 接着从第二个Item分析
	while (dwIndex < dwItemCounts) {
#if READER_VERSION == 0
		if ( m_received_data.GetDataLength() < 32 ) {
			ClearVector(v);
			return ZS_ERR_SYNC_READER_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
		}
#else
		if (m_received_data.GetDataLength() < 28) {
			ClearVector(v);
			return ZS_ERR_SYNC_READER_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
		}
#endif

#if READER_VERSION == 0
		m_received_data.Read(pData, 32);
#else
		m_received_data.Read(pData, 28);
#endif

		pItem = new SyncItem;
		memset(pItem, 0, sizeof(SyncItem));

		// 时间
		pItem->tTime = GetTelemedTagDate(pData + 1, 5);
		//  tag id
		memcpy(pItem->tTagId.abyUid, pData + 6, 8);
		pItem->tTagId.dwUidLen = 8;
		// 温度
		pItem->dwTemperature = pData[14] * 100 + pData[15];
		// 护士id
		memcpy(pItem->tNurseId.abyUid, pData + 18, 8);
		pItem->tNurseId.dwUidLen = 8;

#if READER_VERSION == 0
		// reader id
		memcpy(pItem->tReaderId.abyUid, pData + 26, 4);
		pItem->tReaderId.dwUidLen = 4;
#else
		memcpy(pItem->tReaderId.abyUid, "\x00\x00\x00\x00", 4);
		pItem->tReaderId.dwUidLen = 4;
#endif
		v.push_back(pItem);

		dwIndex++;
	}

	return 0;
}

int   CZsSyncReader::ClearData() {
	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort(m_hComm, CLEAR_COMMAND.abyCommand, CLEAR_COMMAND.dwCommandLength, &dwWrited);

	if (bRet) {
		return 0;
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to Clear command \n");
		return ZS_ERR_SYNC_READER_FAILED_TO_WRITE;
	}
	return 0;
}

int   CZsSyncReader::ReadClearDataRet() {
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

	BYTE pData[8192];

	if (m_received_data.GetDataLength() >= 20) {
		m_received_data.Read(pData, 20);
		// 如果最后两个字节是0D 0A，则OK
		if (0 == memcmp(pData + 18, READER_TAIL, 2)) {
			return 0;
		}
	}

	return ZS_ERR_SYNC_READER_FAILED_TO_RECEIVE_OR_WRONG_FORMAT;
}

int   CZsSyncReader::Receive(DWORD & dwReceivedCnt) {
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

int   CZsSyncReader::CheckHeartBeat() {

	// 如果已经是关闭状态
	if (m_eReaderStatus == STATUS_CLOSE) {
		return ZS_ERR_SYNC_READER_CLOSE;
	}

	int ret = Prepare();
	if (0 != ret) {
		m_eReaderStatus = STATUS_CLOSE;
		m_pBusiness->NotifyUiSyncReaderStatus(m_eReaderStatus);
		m_pBusiness->ReconnectSyncReaderAsyn(RECONNECT_SYNC_READER_TIME);
		return ret;
	}

	ret = ReadPrepareRet();
	if (0 != ret) {
		m_eReaderStatus = STATUS_CLOSE;
		m_pBusiness->NotifyUiSyncReaderStatus(m_eReaderStatus);
		m_pBusiness->ReconnectSyncReaderAsyn(RECONNECT_SYNC_READER_TIME);
		return ret;
	}

	// 延时检查心跳
	m_pBusiness->CheckSyncReaderHeartBeatAsyn(SYNC_READER_HEART_BEAT_TIME);

	return 0;
}

// 同步数据
int   CZsSyncReader::Synchronize(std::vector<SyncItem*> & v) {
	// 如果已经是关闭状态
	if (m_eReaderStatus == STATUS_CLOSE) {
		return ZS_ERR_SYNC_READER_CLOSE;
	}

	int ret = SyncData();
	if (0 != ret) {
		m_eReaderStatus = STATUS_CLOSE;
		m_pBusiness->NotifyUiSyncReaderStatus(m_eReaderStatus);
		m_pBusiness->ReconnectSyncReaderAsyn(RECONNECT_SYNC_READER_TIME);
		return ret;
	}

	ret = ReadSyncDataRet(v);
	if (0 != ret) {
		m_eReaderStatus = STATUS_CLOSE;
		m_pBusiness->NotifyUiSyncReaderStatus(m_eReaderStatus);
		m_pBusiness->ReconnectSyncReaderAsyn(RECONNECT_SYNC_READER_TIME);
		return ret;
	}

	return 0;
}

int CZsSyncReader::ClearReader() {

	// 如果已经是关闭状态
	if (m_eReaderStatus == STATUS_CLOSE) {
		return ZS_ERR_SYNC_READER_CLOSE;
	}

	int ret = ClearData();
	if (0 != ret) {
		m_eReaderStatus = STATUS_CLOSE;
		m_pBusiness->NotifyUiSyncReaderStatus(m_eReaderStatus);
		m_pBusiness->ReconnectSyncReaderAsyn(RECONNECT_SYNC_READER_TIME);
		return ret;
	}

	ret = ReadClearDataRet();
	if (0 != ret) {
		m_eReaderStatus = STATUS_CLOSE;
		m_pBusiness->NotifyUiSyncReaderStatus(m_eReaderStatus);
		m_pBusiness->ReconnectSyncReaderAsyn(RECONNECT_SYNC_READER_TIME);
		return ret;
	}

	return 0;
}