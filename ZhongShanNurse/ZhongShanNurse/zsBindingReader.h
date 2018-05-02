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

	// �̵�
	int Inventory(TagItem * pId);

	enum BINDING_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	BINDING_STATUS GetStatus();

private:
	void Clear();

	// Э�鲿��
	int   RequestPrepare();
	int   ReadPrepareRet();

	int   RequestBeep();
	int   ReadBeepRet();

	int   RequestInv();
	int   ReadInvRet(TagItem * pId);

	void  ReceiveAsPossible(DWORD  dwWaitTime, DWORD dwMaxDataLength = 0);

	CDataBuf                  m_received_data;                // �Ӵ��ڽ������ݵĻ�����	

	BINDING_STATUS      m_eStatus;

	CBusiness  *        m_pBusiness;
};

