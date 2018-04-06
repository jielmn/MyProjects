#pragma once

#include <afxdb.h>
#include <vector>
#include "zsCommon.h"

class CBusiness;

class  CZsDatabase {
public:
	CZsDatabase(CBusiness * pBusiness);
	~CZsDatabase();

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

	DATABASE_STATUS GetStatus();

	int AddPatient(const CPatientParam * pParam, DWORD & dwId );
	int GetAllPatients(std::vector<PatientInfo *> & vRet);
	int ModifyPatient(const CPatientParam * pParam);
	int DeletePatient(const CDeletePatientParam * pParam);
	int ImportPatient( PatientInfo * pPatient );

private:
	void Clear();
	int  OnDatabaseException(CException* e);

	DATABASE_STATUS     m_eDbStatus;
	DATABASE_TYPE       m_eDbType;
	CDatabase           m_database;
	CRecordset          m_recordset;
	BOOL                m_bUtf8;                       // 使用utf8字符集

	CBusiness  *        m_pBusiness;
};
