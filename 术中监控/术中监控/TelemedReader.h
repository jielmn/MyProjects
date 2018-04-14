#pragma once

#include "exhCommon.h"

class CBusiness;

class  CTelemedReader {
public:
	CTelemedReader(CBusiness * pBusiness);
	~CTelemedReader();

	// ����
	int  Reconnect();
	// ��ȡ�¶�
	int ReadTagTemp(DWORD & dwTemp);
	
	enum READER_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	READER_STATUS GetStatus();

private:
	// �������ӣ��رմ���
	// �򿪴���
	BOOL      OpenUartPort(const char *UartPortName);
	// �رմ���
	BOOL      CloseUartPort();

	BOOL      InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits);
	BOOL      WriteUartPort(HANDLE hComm, const void * WriteBuf, DWORD ToWriteDataLen, DWORD *WritedDataLen);
	BOOL      RcvDataFromUartPort(HANDLE hComm, void *RcvBuf, DWORD ToRcvDataLen, DWORD *RcvedDataLen, LPOVERLAPPED lpOverlapped);

	// ���ݽ���
	int       Receive(DWORD & dwReceivedCnt);
	// �������
	void      Clear();


	// Э�鲿��
	int   Prepare();
	int   ReadPrepareRet();
private:
	READER_STATUS      m_eStatus;
	HANDLE             m_hComm;                           // ���ھ��
	CDataBuf           m_received_data;                   // �Ӵ��ڽ������ݵĻ�����
	CBusiness  *       m_pBusiness;
};


