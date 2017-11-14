#pragma once


class CSerialPort
{
public:
	CSerialPort();
	~CSerialPort();

	int        Init();
	int        Deinit();
	int        Sync();
	int        Prepare();
	int        ClearReader();

	ComStatus  GetMainStatus() const;
	ComStatus  GetSubStatus() const;
	BOOL       IfPrepared() const;

	int        Receive( DWORD & dwReceivedCnt );            //��������

	int        FormatData( std::vector<TagData*> & v );     // �ѽ��յ����������� ( upload )

	int        FormatData();                                // �ѽ��յ�����������( prepare, clear )

private:
	HANDLE     m_hComm;
	ComStatus  m_eMainStatus;
	ComStatus  m_eSubStatus;
	CDataBuf   m_received_data;
	BOOL       m_bPrepared;                                 // �Ƿ�׼���ã���Ϊ����������������ϣ�����readerû�д򿪵�Դ��������Ϊû�����ӳɹ���

	BOOL      InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits);
	BOOL      OpenUartPort(char *UartPortName);
	BOOL      WriteUartPort(HANDLE hComm, const void * WriteBuf, DWORD ToWriteDataLen, DWORD *WritedDataLen);
	BOOL      RcvDataFromUartPort(HANDLE hComm, void *RcvBuf, DWORD ToRcvDataLen, DWORD *RcvedDataLen, LPOVERLAPPED lpOverlapped);
	
};