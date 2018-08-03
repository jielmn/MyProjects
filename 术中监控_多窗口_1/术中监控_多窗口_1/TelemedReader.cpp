#include <assert.h>
#include "Business.h"
#include "TelemedReader.h"
#include "LmnTelSvr.h"

CTelemedReader::CTelemedReader(CBusiness * pBusiness) : m_pBusiness(pBusiness) {
	m_eStatus = STATUS_CLOSE;
	m_hComm = 0;
	m_nIndex = 0;
	memset(m_szComPort, 0, sizeof(m_szComPort));
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
	// ����Ѿ�������
	if (m_eStatus == STATUS_OPEN) {
		return 0;
	}

	assert( m_nIndex >= 0 && m_nIndex < MAX_GRID_COUNT );

	char szPort[256];
	STRNCPY(szPort, g_szComPort[m_nIndex], sizeof(szPort));
	JTelSvrPrint("g_szComPort[%d] = %s", m_nIndex, g_szComPort[m_nIndex] );

	std::vector<std::string>  vCom;
	std::vector<std::string>::iterator it;

	if (szPort[0] != '\0') {
		vCom.push_back(szPort);
	}
	else {
		// ��ȡ���еĴ�����Ϣ
		GetAllSerialPortName(vCom);

		// �ų�������ָ����com��
		for (DWORD i = 0; i < g_dwLayoutColumns * g_dwLayoutRows; i++) {
			if ( i != m_nIndex ) {
				if ( g_szComPort[i][0] != '\0' ) {
					for (it = vCom.begin(); it != vCom.end(); it++) {
						const std::string & sItem = *it;
						if (0 == StrICmp(sItem.c_str(), g_szComPort[i])) {
							vCom.erase(it);
							break;
						}
					}

				}
			}
		}
	}

	// �ȹر�֮ǰ���ܴ򿪵Ĵ���
	CloseUartPort();

	BOOL  bPrepared = FALSE;
	// ���Դ򿪴�ϵͳ��õĴ����б�
	do
	{
		JTelSvrPrint("\ncom ports count = %lu", vCom.size());
		
		for (it = vCom.begin(); it != vCom.end(); it++) {
			const std::string  & sItem = *it;

			JTelSvrPrint("try port %s ", sItem.c_str());

			BOOL bRet = OpenUartPort(sItem.c_str());
			if (!bRet) {
				JTelSvrPrint("port %s failed", sItem.c_str());
				CloseUartPort();
				continue;
			}

			int ret = Prepare();
			if (0 != ret) {
				JTelSvrPrint("port %s failed", sItem.c_str());
				CloseUartPort();
				continue;
			}

			ret = ReadPrepareRet();
			if (0 == ret) {
				bPrepared = TRUE;
				JTelSvrPrint("port %s prepared", sItem.c_str());
				STRNCPY( m_szComPort, sItem.c_str(), sizeof(m_szComPort) );
				break;
			}

			JTelSvrPrint("port %s failed", sItem.c_str());
			CloseUartPort();
		}
	} while (0);

	if (bPrepared) {		
		m_eStatus = STATUS_OPEN;
		m_pBusiness->NotifyUiReaderStatus(m_nIndex,m_eStatus);
	}
	else {
		m_pBusiness->ReconnectReaderAsyn(m_nIndex,RECONNECT_READER_DELAY);
	}

	return 0;
}


// ��ȡ�¶�
int CTelemedReader::ReadTagTemp(DWORD & dwTemp) {

	if (m_eStatus == STATUS_CLOSE) {
		return EXH_ERR_READER_CLOSE;
	}

	// g_log->Output(ILog::LOG_SEVERITY_INFO, "send get temperature command \n");
	JTelSvrPrint("sending get temperature command ...");

	DWORD dwWrited = 0;
	BOOL bRet = WriteUartPort(m_hComm, READ_TAG_DATA_COMMAND.abyCommand, READ_TAG_DATA_COMMAND.dwCommandLength, &dwWrited);

	if (!bRet) {
		//g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to send read tag command! \n");
		JTelSvrPrint("failed to send read tag command");

		CloseUartPort();
		m_eStatus = STATUS_CLOSE;
		m_pBusiness->NotifyUiReaderStatus(m_nIndex, m_eStatus);
		m_pBusiness->ReconnectReaderAsyn(m_nIndex, RECONNECT_READER_DELAY);
		return EXH_ERR_READER_FAILED_TO_WRITE;
	}

	JTelSvrPrint("sended get temperature command");

#ifdef TELEMED_READER_TYPE_1
	ReceiveAsPossible(SERIAL_PORT_SLEEP_TIME, 14);
#else
	DWORD dwReceived = 0;
	do
	{
		Sleep(SERIAL_PORT_SLEEP_TIME);
		Receive(dwReceived);
		// �������������
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
		//g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to receive temp data, wrong format! \n");
		JTelSvrPrint("failed to receive temp data, wrong format!");
	}
	else {
		//g_log->Output(ILog::LOG_SEVERITY_ERROR, "failed to receive temp data, received data length =%lu \n", m_received_data.GetDataLength());
		JTelSvrPrint("failed to receive temp data, received data length =%lu ", m_received_data.GetDataLength());
	}
#else
	// 02 12 04 0E 10 02 E0 02 59 CD 93 D9 3D 5E 21 5E 01 0D 0A 
	if (m_received_data.GetDataLength() >= 19) {
		m_received_data.Read(pData, 19);
		// �����������ֽ���0D 0A����OK
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

	CloseUartPort();
	m_eStatus = STATUS_CLOSE;	
	m_pBusiness->NotifyUiReaderStatus(m_nIndex, m_eStatus);
	m_pBusiness->ReconnectReaderAsyn(m_nIndex, RECONNECT_READER_DELAY);

	//m_received_data.Reform();
	return EXH_ERR_READER_TIMEOUT_OR_WRONG_FORMAT; 
}








// �򿪴���
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
		GENERIC_WRITE | GENERIC_READ, //����Ȩ��  
		0,                            //������  
		NULL,                         //���صľ���������ӽ��̼̳�  
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,         //0��ͬ��ģʽ��FILE_FLAG_OVERLAPPED���첽ģʽ  
		0                             //��ʹ����ʱ�ļ����  
	);
	
	// DWORD t = GetLastError();
	if (INVALID_HANDLE_VALUE == hComm)
	{
		JTelSvrPrint("    failed to open %s", UartPortName);
		bResult = FALSE;
	}
	else
	{
		if (InitUartPort(hComm, 9600, 8, NOPARITY, ONESTOPBIT))
		{
			m_hComm = hComm;

#if 0
			COMMTIMEOUTS  timeout;
			memset(&timeout, 0, sizeof(COMMTIMEOUTS));
			GetCommTimeouts(hComm, &timeout);
			// д����2�� timeout
			timeout.WriteTotalTimeoutConstant = 2000;
			SetCommTimeouts(hComm, &timeout);
#endif

		}
		else
		{
			JTelSvrPrint("    failed to init %s", UartPortName);
			bResult = FALSE;
		}
	}

	return bResult;
}

// �رմ���
BOOL   CTelemedReader::CloseUartPort() {
	if (0 != m_hComm) {
		CloseHandle(m_hComm);
		// DWORD t = GetLastError();
		m_hComm = 0;
	}
	m_received_data.Clear();
	memset( m_szComPort, 0, sizeof(m_szComPort) );
	return TRUE;
}

BOOL   CTelemedReader::InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits) {
	BOOL bResult = true;

	//���ý��ջ�����������������Ĵ�С  
	DWORD dwInQueue = 1024;
	DWORD dwOutQueue = 1024;
	if (!SetupComm(hComm, dwInQueue, dwOutQueue))
	{
		bResult = false;
	}
	else
	{
		//���ö�д�ĳ�ʱʱ��  �Ժ������  
		COMMTIMEOUTS timeouts;
		//for read ReadTotalTimeouts = ReadTotalTimeoutMultiplier * ToReadByteNumber + ReadTotalTimeoutConstant��  
		timeouts.ReadIntervalTimeout = MAXDWORD;                    //���������ַ�֮������ʱʱ��  
		timeouts.ReadTotalTimeoutMultiplier = 0;            //���ȡҪ���ֽ�����˵�ϵ��  
		timeouts.ReadTotalTimeoutConstant = 0;              //��ȡ�ܳ�ʱʱ�䳣��  
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
					//��ս��ջ������������buffer  
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

	ClearCommError(hComm, &dwError, &ComStat);//���ش��ڴ���ͱ���(Ҳ���Բ鿴������״̬)

	OVERLAPPED o;
	memset(&o, 0, sizeof(OVERLAPPED));

	BOOL bReadDone = WriteFile(hComm, WriteBuf, ToWriteDataLen, WritedDataLen, &o);
	dwError = GetLastError();

	if ( !bReadDone && (dwError == ERROR_IO_PENDING) ) {
		WaitForSingleObject(hComm, 2000);    //��ʱ�˴�Ҳ�ᷢ������
		// dwError = GetLastError();
		bReadDone = TRUE;
	}

	// ��������ݲ���expected��
	if (o.InternalHigh != ToWriteDataLen) {
		JTelSvrPrint("    failed to write com port");
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

	OVERLAPPED o;
	memset(&o, 0, sizeof(OVERLAPPED));

	if (RcvDataFromUartPort(m_hComm, buf, sizeof(buf), &dwReceivedCnt, &o))
	{
		// ���浽������
		if (dwReceivedCnt) {
			m_received_data.Reform();
			m_received_data.Append(buf, dwReceivedCnt);
		}
	}
	return 0;
}





int  CTelemedReader::Prepare() {

	DWORD dwWrited = 0;
	JTelSvrPrint("    sending prepare command ...");
	BOOL bRet = WriteUartPort(m_hComm, PREPARE_COMMAND.abyCommand, PREPARE_COMMAND.dwCommandLength, &dwWrited);	

	if (bRet) {
		JTelSvrPrint("    prepare command OK");
		return 0;
	}
	else {
		JTelSvrPrint("    failed to prepare");
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
		// �������������
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
			JTelSvrPrint("    receive prepare response OK");
			return 0;
		}
	}
#else
	if (m_received_data.GetDataLength() >= 20) {
		m_received_data.Read(pData, 20);
		// �����������ֽ���0D 0A����OK
		if (0 == memcmp(pData + 18, READER_TAIL, 2)) {
			return 0;
		}
	}
#endif

	JTelSvrPrint("    failed to receive prepare response");
	return -1;
}



void  CTelemedReader::ReceiveAsPossible( DWORD dwMaxTime, DWORD dwMaxDataLength ) {
	const DWORD  dwIntervalTime = 200;
	DWORD  dwLeftTime = dwMaxTime;

	DWORD  dwReceived = 0;
	DWORD  dwReceivedCnt = m_received_data.GetDataLength();

	do
	{
		// ����ﵽ���ݳ���Ҫ��
		if (dwReceivedCnt >= dwMaxDataLength) {
			break;
		}

		Sleep(dwIntervalTime);
		Receive(dwReceived);

		dwReceivedCnt += dwReceived;
		
		if ( dwLeftTime > dwIntervalTime ) {
			dwLeftTime -= dwIntervalTime;
		}
		// ʱ���Ѿ�����
		else {
			break;
		}

	} while (TRUE);
}