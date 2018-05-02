#pragma once

#include <afxdb.h>
#include "zsCommon.h"
#include "LmnSerialPort.h"

class CBusiness;

class  CZsBindingReader : public CLmnSerialPort {
public:
	CZsBindingReader(CBusiness * pBusiness);
	~CZsBindingReader();

	int Init();

	// 盘点
	int Inventory(TagItem * pId);

	enum BINDING_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	BINDING_STATUS GetStatus();

private:
	void Clear();

	// 协议部分
	int   RequestPrepare();
	int   ReadPrepareRet();

	int   RequestBeep();
	int   ReadBeepRet();

	int   RequestInv();
	int   ReadInvRet(TagItem * pId);

	void  ReceiveAsPossible(DWORD  dwWaitTime, DWORD dwMaxDataLength = 0);

	CDataBuf                  m_received_data;                // 从串口接收数据的缓冲区	

	BINDING_STATUS      m_eStatus;

	CBusiness  *        m_pBusiness;
};

