#pragma once
#include <afxdb.h>
#include "InvCommon.h"

#include "inc/rfidlib.h"
#include "inc/rfidlib_reader.h"
#include "inc/rfidlib_AIP_ISO15693.h"
#include "inc/rfidlib_aip_iso14443A.h"
#include "inc/rfidlib_aip_iso18000p3m3.h"

class CBusiness;

class  CInvReader {
public:
	CInvReader(CBusiness * pBusiness);
	~CInvReader();

	// 重连数据库
	int ReconnectReader();
	// 盘点
	void Inventory();

	enum READER_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	READER_STATUS GetStatus();

private:
	void Clear();

	READER_STATUS       m_eReaderStatus;
	RFID_READER_HANDLE  m_hr_reader;
	BOOL                m_bNewAI;
	BOOL                m_enISO15693;
	BOOL 	            m_enAFI;
	DWORD	            m_afi;
	BOOL	            m_enISO14443a;
	BOOL                m_enISO18000p3m3;

	CBusiness  *        m_pBusiness;
};