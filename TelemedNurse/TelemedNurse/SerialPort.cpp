#include <assert.h>
#include "SerialPort.h"
#include "Business.h"
#include "LmnString.h"
#include "UiMessage.h"

typedef struct tagReaderCmd {
	BYTE      abyCommand[256];
	DWORD     dwCommandLength;
}ReaderCmd;

ReaderCmd              SYNC_COMMAND;
ReaderCmd              PREPARE_COMMAND;
ReaderCmd              CLEAR_COMMAND;

#define   SERIAL_PORT_SLEEP_TIME    1000
#define   READER_TAIL               "\x0d\x0a"

extern    HWND    g_hWnd;


static int TransferReaderCmd(ReaderCmd & cmd, const char * szCmd) {
	if (0 == szCmd) {
		return -1;
	}

	SplitString split;
	split.SplitByBlankChars(szCmd);
	DWORD dwSize = split.Size();
	if (dwSize >= sizeof(cmd.abyCommand)) {
		return -1;
	}

	for (DWORD i = 0; i < dwSize; i++) {
		int  tmp = 0;
		sscanf_s(split[i], "%x", &tmp);
		cmd.abyCommand[i] = (BYTE)tmp;
	}
	cmd.dwCommandLength = dwSize;

	return 0;
}

CSerialPort * CSerialPort::pInstance = 0;

CSerialPort *  CSerialPort::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CSerialPort;
	}
	return pInstance;
}

CSerialPort::CSerialPort() {
	char buf[8192];
	g_cfg->GetConfig("sync command", buf, sizeof(buf), "55 01 03 dd aa");
	TransferReaderCmd(SYNC_COMMAND, buf);

	g_cfg->GetConfig("prepare command", buf, sizeof(buf), "55 01 01 dd aa");
	TransferReaderCmd(PREPARE_COMMAND, buf);

	g_cfg->GetConfig("clear command", buf, sizeof(buf), "55 01 04 dd aa");
	TransferReaderCmd(CLEAR_COMMAND, buf);
}

CSerialPort::~CSerialPort() {

}



BOOL CSerialPort::InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits) {
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

BOOL CSerialPort::OpenUartPort( const char *UartPortName)
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

BOOL   CSerialPort::CloseUartPort() {
	if (0 != m_hComm) {
		CloseHandle(m_hComm);
		m_hComm = 0;
	}
	m_received_data.Clear();
	return TRUE;
}

BOOL   CSerialPort::WriteUartPort(HANDLE hComm, const void * WriteBuf, DWORD ToWriteDataLen, DWORD *WritedDataLen) {
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

int   CSerialPort::Prepare() {

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

int   CSerialPort::SkipInstantResp() {
	BYTE pData[8192];
	do
	{
		// 清除掉扫描tag的及时反馈
		if (m_received_data.GetDataLength() >= 19) {
			m_received_data.Read(pData, 19);
			// 如果最后两个字节是0D 0A，则认为是温度贴的及时反馈
			// 以下最后两个字节不是0D 0A
			if (0 != memcmp(pData + 17, READER_TAIL, 2)) {
				DWORD pos = m_received_data.GetReadPos();
				assert(pos >= 19);
				m_received_data.SetReadPos(pos - 19);
				break;
			}
		}
		else {
			break;
		}
	} while (0);
	return 0;
}

int   CSerialPort::ReadPrepareRet() {
	DWORD dwReceived = 0;

	do 
	{
		Sleep(SERIAL_PORT_SLEEP_TIME);
		Receive(dwReceived);
		// 如果再无数据了
		if (0 == dwReceived) {
			break;
		}		
	} while ( TRUE );
	
	BYTE pData[8192];

	SkipInstantResp();
	if (m_received_data.GetDataLength() >= 20) {
		m_received_data.Read(pData, 20);
		// 如果最后两个字节是0D 0A，则OK
		if (0 == memcmp(pData + 18, READER_TAIL, 2)) {
			return 0;
		}
	}

	return -1;
}

int   CSerialPort::Clear() {
	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort(m_hComm, CLEAR_COMMAND.abyCommand, CLEAR_COMMAND.dwCommandLength, &dwWrited);

	if (bRet) {
		return 0;
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to Clear \n");
		return -1;
	}
}

int   CSerialPort::ReadClearRet() {
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
	SkipInstantResp();

	if (m_received_data.GetDataLength() >= 20) {
		m_received_data.Read(pData, 20);
		// 如果最后两个字节是0D 0A，则OK
		if (0 == memcmp(pData + 18, READER_TAIL, 2)) {
			return 0;
		}
	}

	return -1;
}

int   CSerialPort::Sync() {
	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort(m_hComm, SYNC_COMMAND.abyCommand, SYNC_COMMAND.dwCommandLength, &dwWrited);

	if (bRet) {
		return 0;
	}
	else {
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to Sync \n");
		return -1;
	}
}

int   CSerialPort::ReadSyncRet(std::vector<TagData*> & v) {
	DWORD dwReceived = 0;
	int ret = -1;

	

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
	int nCount = -1;
	int nIndex = 0;
	// 必须要有一个20字节的item
	do 
	{
		SkipInstantResp();

		if (m_received_data.GetDataLength() >= 20) {
			m_received_data.Read(pData, 20);
			// 如果最后两个字节不是是0D 0A，则FAIL
			if (0 != memcmp(pData + 18, READER_TAIL, 2)) {
				break;
			}
		}
		else {
			break;
		}

		BYTE * pBuf = pData;
		// 第一个字节是'\02'		
		pBuf += 1;

		// 时间5个字节
		struct tm timeinfo;
		time_t   tTime;
		timeinfo.tm_year = 2000 + pBuf[0] - 1900;
		timeinfo.tm_mon  = pBuf[1] - 1;
		timeinfo.tm_mday = pBuf[2];
		timeinfo.tm_hour = pBuf[3];
		timeinfo.tm_min  = pBuf[4];
		timeinfo.tm_sec  = 0;
		tTime = mktime(&timeinfo);
		// 偏移5个字节
		pBuf += 5;

		// 8个字节是Tag ID
		TagId  tId;
		assert(sizeof(tId.Id) >= 8);
		memcpy( tId.Id, pBuf, 8 );
		pBuf += 8;

		// 2个字节是温度
		DWORD dwTemperature = 0;
		dwTemperature = pBuf[0] * 100 + pBuf[1];
		pBuf += 2;

		// 2个字节是个数
		DWORD dwCount = pBuf[0] * 256 + pBuf[1];
		pBuf += 2;

		// 第一次读到item，获取总数
		if ( nCount < 0 ) {
			nCount = (int)dwCount;

			// 没有同步的数据
			if (nCount == 0) {
				ret = 0;
				break;
			}
		}
		else {
			// SYNC格式错误
			if ((int)dwCount != nCount) {
				break;
			}
		}

		TagData* pItem = new TagData;
		memset(pItem, 0, sizeof(TagData));

		pItem->tTime = tTime;
		pItem->dwTemperature = dwTemperature;
		memcpy(&pItem->tTagId, &tId, sizeof(TagId));

		v.push_back(pItem);

		nIndex++;
		// 个数达到要求
		if (nIndex == nCount) {
			ret = 0;
			break;
		}
	} while ( TRUE );

	if (0 != ret) {
		ClearVector(v);
	}

	return ret;
}

int   CSerialPort::Receive(DWORD & dwReceivedCnt) {
	char buf[8192];
	dwReceivedCnt = 0;
	if ( RcvDataFromUartPort(m_hComm, buf, sizeof(buf), &dwReceivedCnt, 0 ))
	{
		// 保存到缓冲中
		if (dwReceivedCnt) {
			m_received_data.Append(buf, dwReceivedCnt);
		}
	}
	return 0;
}


void   CSerialPort::NotifySerialPortStatus(SerialPortStatus eStatus) {
	if (g_hWnd != 0) {
		::PostMessage(g_hWnd, UM_SERIAL_PORT_STATUS, (WPARAM)eStatus, 0 );
	}
}

void  CSerialPort::NotifySerialPortSyncRet(SerialPortSyncError eError, std::vector<TagData*> * pVRet /*= 0*/) {
	if (g_hWnd == 0) {
		return;
	}
	::PostMessage(g_hWnd, UM_SERIAL_PORT_SYNC_RET, (WPARAM)eError, (LPARAM)pVRet );
}

BOOL  CSerialPort::IfOpened() {
	return m_hComm == 0 ? FALSE : TRUE;
}


LmnToolkits::Thread  * g_thrd_serial_port = 0;
MessageHandlerSerialPort * g_handler_serial_port = 0;

MessageHandlerSerialPort::MessageHandlerSerialPort() {

}

MessageHandlerSerialPort::~MessageHandlerSerialPort() {

}


static BOOL GetAllSerialPortName( std::vector<std::string> & vCom ) {

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

void MessageHandlerSerialPort::OnMessage(DWORD dwMessageId, const LmnToolkits::MessageData * pMessageData) {
	char buf[8192];
	switch ( dwMessageId)
	{
	case MSG_RECONNECT_SERIAL_PORT:
	{
		BOOL  bRet      = FALSE;
		BOOL  bPrepared = FALSE;
		int   ret       = 0;
		std::vector<std::string>  vCom;
		GetAllSerialPortName(vCom);

		// 如果已经连接
		if ( CSerialPort::GetInstance()->IfOpened() ) {
			break;
		}
		
		g_cfg->GetConfig("serial port", buf, sizeof(buf), "" );

		CSerialPort::GetInstance()->CloseUartPort();

		//CSerialPort::GetInstance()->NotifySerialPortStatus(SERIAL_PORT_STATUS_OPENING);
		// 尝试打开配置文件里的串口
		do 
		{
			if (buf[0] == '\0')
				break;

			bRet = CSerialPort::GetInstance()->OpenUartPort(buf);
			if (!bRet) {
				CSerialPort::GetInstance()->CloseUartPort();
				break;
			}
		} while (0);

		// 如果打开成功，尝试prepare
		if (bRet) {
			CSerialPort::GetInstance()->Prepare();
			ret = CSerialPort::GetInstance()->ReadPrepareRet();
			if (0 == ret) {
				bPrepared = TRUE;
			}
		}

		// 尝试打开从系统获得的串口列表
		do 
		{
			if (bPrepared) {
				break;
			}

			std::vector<std::string>::iterator it;
			for (it = vCom.begin(); it != vCom.end(); it++) {
				std::string  sItem = *it;

				bRet = CSerialPort::GetInstance()->OpenUartPort( sItem.c_str() );
				if (!bRet) {
					CSerialPort::GetInstance()->CloseUartPort();
					continue;
				}

				CSerialPort::GetInstance()->Prepare();
				ret = CSerialPort::GetInstance()->ReadPrepareRet();
				if (0 == ret) {
					bPrepared = TRUE;
					break;
				}
			}
		} while ( 0 );

		if (bPrepared) {
			CSerialPort::GetInstance()->NotifySerialPortStatus(SERIAL_PORT_STATUS_OPEND);
			// 定时检查心跳
			// g_thrd_serial_port->PostDelayMessage(DELAY_SERIAL_PORT_HEART_BEAT, g_handler_serial_port, MSG_SERIAL_PORT_HEART_BEAT);
		}
		else {
			// 连接，重新连接
			CSerialPort::GetInstance()->CloseUartPort();
			CSerialPort::GetInstance()->NotifySerialPortStatus(SERIAL_PORT_STATUS_CLOSED);
			g_thrd_serial_port->PostDelayMessage( DELAY_RECONNECT_SERIAL_PORT, g_handler_serial_port, MSG_RECONNECT_SERIAL_PORT);
		}
	}
	break;

	case MSG_SERIAL_PORT_HEART_BEAT: 
	{
		CSerialPort::GetInstance()->Prepare();
		int ret = CSerialPort::GetInstance()->ReadPrepareRet();
		if (0 == ret) {
			// 定时检查心跳
			g_thrd_serial_port->PostDelayMessage(DELAY_SERIAL_PORT_HEART_BEAT, g_handler_serial_port, MSG_SERIAL_PORT_HEART_BEAT);
		}
		else {
			// 心跳失败，重新连接
			CSerialPort::GetInstance()->CloseUartPort();
			CSerialPort::GetInstance()->NotifySerialPortStatus(SERIAL_PORT_STATUS_CLOSED);
			g_thrd_serial_port->PostDelayMessage(DELAY_RECONNECT_SERIAL_PORT, g_handler_serial_port, MSG_RECONNECT_SERIAL_PORT);
		}
	}
	break;

	case MSG_SERIAL_PORT_SYNC:
	{
		BOOL bOpend = CSerialPort::GetInstance()->IfOpened();
		if (!bOpend) {
			CSerialPort::GetInstance()->NotifySerialPortSyncRet(SERIAL_PORT_SYNC_ERROR_NOT_OPENED);
			break;
		}

		int ret = CSerialPort::GetInstance()->Sync();
		if (0 != ret) {
			CSerialPort::GetInstance()->NotifySerialPortSyncRet(SERIAL_PORT_SYNC_ERROR_FAILED_TO_SYNC);

			// 连接，重新连接
			CSerialPort::GetInstance()->CloseUartPort();
			CSerialPort::GetInstance()->NotifySerialPortStatus(SERIAL_PORT_STATUS_CLOSED);
			g_thrd_serial_port->PostDelayMessage(DELAY_RECONNECT_SERIAL_PORT, g_handler_serial_port, MSG_RECONNECT_SERIAL_PORT);
			break;
		}

		std::vector<TagData*> * pVec = new std::vector<TagData*>;
		ret = CSerialPort::GetInstance()->ReadSyncRet( *pVec );
		if (0 != ret) {
			ClearVector(*pVec);
			delete pVec;
			CSerialPort::GetInstance()->NotifySerialPortSyncRet(SERIAL_PORT_SYNC_ERROR_FAILED_TO_RECEIVE_RET);

			// 连接，重新连接
			CSerialPort::GetInstance()->CloseUartPort();
			CSerialPort::GetInstance()->NotifySerialPortStatus(SERIAL_PORT_STATUS_CLOSED);
			g_thrd_serial_port->PostDelayMessage(DELAY_RECONNECT_SERIAL_PORT, g_handler_serial_port, MSG_RECONNECT_SERIAL_PORT);
			break;
		}

		CSerialPort::GetInstance()->NotifySerialPortSyncRet(SERIAL_PORT_SYNC_ERROR_OK, pVec );
	}
	break;

	case MSG_SERIAL_PORT_CLEAR:
	{
		int ret = CSerialPort::GetInstance()->Clear();
		if (0 != ret) {
			// 连接，重新连接
			CSerialPort::GetInstance()->CloseUartPort();
			CSerialPort::GetInstance()->NotifySerialPortStatus(SERIAL_PORT_STATUS_CLOSED);
			g_thrd_serial_port->PostDelayMessage(DELAY_RECONNECT_SERIAL_PORT, g_handler_serial_port, MSG_RECONNECT_SERIAL_PORT);
			break;
		}

		ret = CSerialPort::GetInstance()->ReadClearRet(); 
		if (0 != ret) {
			// 连接，重新连接
			CSerialPort::GetInstance()->CloseUartPort();
			CSerialPort::GetInstance()->NotifySerialPortStatus(SERIAL_PORT_STATUS_CLOSED);
			g_thrd_serial_port->PostDelayMessage(DELAY_RECONNECT_SERIAL_PORT, g_handler_serial_port, MSG_RECONNECT_SERIAL_PORT);
			break;
		}
	}
	break;

	default:
		break;
	}
}



void  SyncReader() {
	g_thrd_serial_port->PostMessage(g_handler_serial_port, MSG_SERIAL_PORT_SYNC );
	
}

void  ClearReader() {
	g_thrd_serial_port->PostMessage(g_handler_serial_port, MSG_SERIAL_PORT_CLEAR);
}