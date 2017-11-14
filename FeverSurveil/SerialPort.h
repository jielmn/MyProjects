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

	int        Receive( DWORD & dwReceivedCnt );            //接收数据

	int        FormatData( std::vector<TagData*> & v );     // 把接收到的数据整理 ( upload )

	int        FormatData();                                // 把接收到的数据整理( prepare, clear )

private:
	HANDLE     m_hComm;
	ComStatus  m_eMainStatus;
	ComStatus  m_eSubStatus;
	CDataBuf   m_received_data;
	BOOL       m_bPrepared;                                 // 是否准备好，因为如果蓝牙串口连接上，但是reader没有打开电源，还是认为没有连接成功。

	BOOL      InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits);
	BOOL      OpenUartPort(char *UartPortName);
	BOOL      WriteUartPort(HANDLE hComm, const void * WriteBuf, DWORD ToWriteDataLen, DWORD *WritedDataLen);
	BOOL      RcvDataFromUartPort(HANDLE hComm, void *RcvBuf, DWORD ToRcvDataLen, DWORD *RcvedDataLen, LPOVERLAPPED lpOverlapped);
	
};