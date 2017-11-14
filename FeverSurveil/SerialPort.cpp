#include "stdafx.h"
#include "SerialPort.h"

// һ���¶�����20�ֽ�
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
		GENERIC_WRITE | GENERIC_READ, //����Ȩ��  
		0,                            //������  
		NULL,                         //���صľ���������ӽ��̼̳�  
		OPEN_EXISTING,
		0,                            //0��ͬ��ģʽ��FILE_FLAG_OVERLAPPED���첽ģʽ  
		0                             //��ʹ����ʱ�ļ����  
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

	ClearCommError(hComm, &dwError, &ComStat);//���ش��ڴ���ͱ���(Ҳ���Բ鿴������״̬)
	BOOL bWriteStat = WriteFile( hComm, WriteBuf, ToWriteDataLen, WritedDataLen, NULL );
	if (!bWriteStat)
	{
		g_log->Output(ILog::LOG_SEVERITY_ERROR, "д����ʧ��!\n");
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
	// �Ѿ���״̬
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

	// �Ѿ���״̬
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
	// �Ѿ���״̬
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
	// �Ѿ���״̬
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
	// �Ѿ���״̬
	if (COM_STATUS_OPEN != m_eMainStatus) {
		return -1;
	}

	char buf[8192];
	if ( RcvDataFromUartPort( m_hComm, buf, sizeof(buf), &dwReceivedCnt, 0) )
	{
		// ���浽������
		if (dwReceivedCnt) {
			m_received_data.Append(buf, dwReceivedCnt);
		}		
	}

	return 0;
}

int  CSerialPort::FormatData(std::vector<TagData*> & v) {
	// �Ѿ���״̬
	if (COM_STATUS_OPEN != m_eMainStatus) {
		m_received_data.Clear();
		return -1;
	}

	// ͬ������������
	if (COM_STATUS_UPLOADING == m_eSubStatus) {
		if (m_received_data.GetDataLength() % TAG_DATA_SIZE != 0) {
			g_log->Output(ILog::LOG_SEVERITY_ERROR, "ͬ�����ݵĽ������20�ֽڵ���������ʵ���ֽ���=%d \n", m_received_data.GetDataLength());
			m_received_data.Clear();
			return -1;
		}

		char tmp[256];
		g_cfg->GetConfig("use system clock", tmp, sizeof(tmp), "false");
		BOOL bUseSystemClock = GetBoolean(tmp);

		// tag �¶����ݵĸ���
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
			// ��ʽ����
			else {
				g_log->Output(ILog::LOG_SEVERITY_ERROR, "ͬ�����ݵĽ����ʽ���󣺲���02��0a 02��ͷ \n");
				m_received_data.Clear();
				return -1;
			}

			BYTE * pBuf = buf + nOffset;

			TagData * pItem = new TagData;

			struct tm timeinfo;
			// 5���ֽ���ʱ��
			// ���ʹ��ϵͳʱ��
			if (bUseSystemClock) {
				time_t tTmp = time(0);
				localtime_s(&timeinfo, &tTmp);
				timeinfo.tm_hour = pBuf[3];
				timeinfo.tm_min = pBuf[4];
				timeinfo.tm_sec = 0;
				pItem->tTime = mktime(&timeinfo);
			}
			// ʹ��reader��ʱ��
			else {
				timeinfo.tm_year = 2000 + pBuf[0] - 1900;
				timeinfo.tm_mon =  pBuf[1] - 1;
				timeinfo.tm_mday = pBuf[2];
				timeinfo.tm_hour = pBuf[3];
				timeinfo.tm_min = pBuf[4];
				timeinfo.tm_sec = 0;
				pItem->tTime = mktime(&timeinfo);
			}
			// ƫ��5���ֽ�
			pBuf += 5;

			// 8���ֽ���Tag ID
			memcpy(pItem->abyTagId, pBuf, 8);
			pBuf += 8;

			// 2���ֽ����¶�
			pItem->dwTemperatur = pBuf[0] * 100 + pBuf[1];
			pBuf += 2;

			// 2���ֽ��Ǹ���
			DWORD dwCount = pBuf[0] * 256 + pBuf[1];
			if (dwCount == 0) {
				delete pItem;
				if (nCnt != 1) {
					g_log->Output(ILog::LOG_SEVERITY_ERROR, "ͬ�����ݵĽ����ʽ���󣺸������ֽڼ�����%d���������ڱ�ʶ��%d�� \n", nCnt, (int)dwCount);
					return -1;
				}
				break;
			} else if (dwCount != nCnt) {
				g_log->Output(ILog::LOG_SEVERITY_ERROR, "ͬ�����ݵĽ����ʽ���󣺸������ֽڼ�����%d���������ڱ�ʶ��%d�� \n", nCnt, (int)dwCount );
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
			g_log->Output(ILog::LOG_SEVERITY_ERROR, "Prepare���ݵĽ������20�ֽڣ�ʵ���ֽ�����%d \n", m_received_data.GetDataLength());
			m_received_data.Clear();
			return -1;
		}
		m_bPrepared = TRUE;
	}
	else if (m_eSubStatus == COM_STATUS_CLEARING) {
		if (m_received_data.GetDataLength() != TAG_DATA_SIZE) {
			g_log->Output(ILog::LOG_SEVERITY_ERROR, "Clear���ݵĽ������20�ֽڣ�ʵ���ֽ�����%d \n", m_received_data.GetDataLength());
			m_received_data.Clear();
			return -1;
		}
	}

	m_eSubStatus = COM_STATUS_IDLE;
	m_received_data.Clear();
	return 0;
}