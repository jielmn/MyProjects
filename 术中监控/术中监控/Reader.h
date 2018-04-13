#pragma once

#include "exhCommon.h"

class CBusiness;

class  CReader {
public:
	CReader(CBusiness * pBusiness);
	~CReader();
	
	// ����
	int  Reconnect();
	// ��ȡTag�¶�����
	int  GetTagTemperatureData();

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

	// Э�鲿��
	int   Prepare();
	int   ReadPrepareRet();
	int   Inventory();
	int   ReadInventoryRet();
	int   ReadTagBlock( int nStartBlock, int nReadLength );
	int   ReadTagBlockRet( std::vector<TagBlock*> &v );

	int   Receive(DWORD & dwReceivedCnt);            // ��������
	int   ReadToken(Token & t);

	void  Clear();
private:
	READER_STATUS      m_eStatus;
	HANDLE             m_hComm;                           // ���ھ��
	CDataBuf           m_received_data;                   // �Ӵ��ڽ������ݵĻ�����
	CBusiness  *       m_pBusiness;

	TagItem            m_tag;                             // ���ĵ���tag
	BOOL               m_bFoundTag;                       // ����������tag
};

