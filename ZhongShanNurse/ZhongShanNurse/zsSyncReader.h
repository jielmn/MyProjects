#pragma once

#include "zsCommon.h"

class CBusiness;

class  CZsSyncReader {
public:
	CZsSyncReader(CBusiness * pBusiness);
	~CZsSyncReader();

	// ����
	int Reconnect();
	// �������
	int CheckHeartBeat();
	// ͬ������
	int Synchronize(std::vector<SyncItem*> & v);
	// �������
	int ClearReader();

	enum SYNC_READER_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	SYNC_READER_STATUS GetStatus();

private:
	void Clear();

	SYNC_READER_STATUS     m_eReaderStatus;	
	CBusiness  *           m_pBusiness;

	HANDLE                 m_hComm;                           // ���ھ��
	CDataBuf               m_received_data;                   // �Ӵ��ڽ������ݵĻ�����

private:
	// �������ӣ��رմ���
	// �򿪴���
	BOOL      OpenUartPort(const char *UartPortName);
	// �رմ���
	BOOL      CloseUartPort();

	BOOL      InitUartPort(HANDLE hComm, DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits);
	BOOL      WriteUartPort(HANDLE hComm, const void * WriteBuf, DWORD ToWriteDataLen, DWORD *WritedDataLen);
	BOOL      RcvDataFromUartPort(HANDLE hComm, void *RcvBuf, DWORD ToRcvDataLen, DWORD *RcvedDataLen, LPOVERLAPPED lpOverlapped);


	// ReaderЭ�鴦��
	int   Prepare();
	int   ReadPrepareRet();
	int   SyncData();
	int   ReadSyncDataRet(std::vector<SyncItem*> & v);
	int   ClearData();
	int   ReadClearDataRet();


	int   Receive(DWORD & dwReceivedCnt);            // ��������
};

