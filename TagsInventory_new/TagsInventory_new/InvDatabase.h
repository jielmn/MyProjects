#pragma once
#include <afxdb.h>
#include "InvCommon.h"

class CBusiness;

class  CInvDatabase {
public:
	CInvDatabase(/*CBusiness * pBusiness*/);
	~CInvDatabase();

	// 重连数据库
	int ReconnectDb();

	enum DATABASE_STATUS {
		STATUS_CLOSE = 0,
		STATUS_OPEN,
	};

	// odbc database type
	enum DATABASE_TYPE {
		TYPE_ORACLE = 0,
		TYPE_MYSQL,
	};

private:
	void Clear();

	DATABASE_STATUS     m_eDbStatus;
	DATABASE_TYPE       m_eDbType;
	CDatabase           m_database;
	CRecordset          m_recordset;
	BOOL                m_bUtf8;                       // 使用utf8字符集

	CBusiness  *        m_pBusiness;
};