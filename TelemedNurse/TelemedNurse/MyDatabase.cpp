#include <assert.h>
#include <afx.h>   
#include <time.h>
#include "LmnString.h"
#include "Business.h"
#include "MyDatabase.h"

#define  DB_NAME                          "telemed_nurse.db"
#define  PATIENTS_TABLE_NAME              "Patients"
#define  TAGS_TABLE_NAME                  "Tags"


char * ConvertSqlField( char * szDest, DWORD dwDestSize, const char * filed_value ) {
	int ret = StrReplaceAll( szDest, dwDestSize - 1, filed_value, "'", "''" );
	if (0 == ret) {
		return szDest;
	}
	else {
		return 0;
	}
}



CMyDatabase *  CMyDatabase::pInstance = 0;

CMyDatabase *  CMyDatabase::GetInstance() {
	if (0 == pInstance) {
		pInstance = new CMyDatabase;
	}
	return pInstance;
}

CMyDatabase::CMyDatabase() {
	sigInitDb.connect(this, &CMyDatabase::OnInitDb);
	sigDeinitDb.connect(this, &CMyDatabase::OnDeinitDb);
	sigAMPatient.connect(this, &CMyDatabase::OnPatientEvent);
	m_db = 0;
}

void CMyDatabase::OnInitDb(int * ret) {
	assert(ret);

	*ret = sqlite3_open(DB_NAME, &m_db);
	if (*ret != 0) {
		*ret = (int)ERROR_FAILED_TO_OPEN_DB;
		return;
	}

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	const char * sql = 0;

	// 查看Patients表是否存在，如果不存在，则创建
	sql = "select name from sqlite_master where name = '" PATIENTS_TABLE_NAME "';";
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);

	// 表不存在，则创建表
	if (0 == nrow) {
		sql = "CREATE TABLE " PATIENTS_TABLE_NAME "("  \
			"Id             CHAR(32)    PRIMARY KEY     NOT NULL," \
			"InNo           CHAR(32)    NOT NULL," \
			"Name           CHAR(32)    NOT NULL," \
			"Sex            INT         NOT NULL," \
			"Age            INT         NOT NULL," \
			"Office         CHAR(32)," \
			"BedNo          CHAR(32)," \
			"InDate         DATETIME    NOT NULL," \
			"CardNo         CHAR(32)" \
			");";

		*ret = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
		if (*ret != 0) {
			*ret = (int)ERROR_FAILED_TO_EXECUTE_SQL;
			sqlite3_free_table(azResult);
			return;
		}
	}
	sqlite3_free_table(azResult);


	// 查看Tags表是否存在，如果不存在，则创建
	sql = "select name from sqlite_master where name = '" TAGS_TABLE_NAME "';";
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);

	// 表不存在，则创建表
	if (0 == nrow) {
		sql = "CREATE TABLE " TAGS_TABLE_NAME "("  \
			  "Id             CHAR(32)    PRIMARY KEY     NOT NULL," \
			  "PatientID      CHAR(32)    NOT NULL" \
			");";

		*ret = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
		if (*ret != 0) {
			*ret = (int)ERROR_FAILED_TO_EXECUTE_SQL;
			sqlite3_free_table(azResult);
			return;
		}
	}
	sqlite3_free_table(azResult);
}

void CMyDatabase::OnDeinitDb(int * ret) {
	assert(ret);
	*ret = sqlite3_close(m_db);
}

void  CMyDatabase::OnPatientEvent(PatientInfo * pPatientInfo, OperationType eType, int * ret) {
	assert(ret);
	
	char sql[8192];
	char *zErrMsg = 0;
	
	char tmp[6][256];
	ConvertSqlField(tmp[0], 256, pPatientInfo->szId);
	ConvertSqlField(tmp[1], 256, pPatientInfo->szInNo);
	ConvertSqlField(tmp[2], 256, pPatientInfo->szName);
	ConvertSqlField(tmp[3], 256, pPatientInfo->szOffice);
	ConvertSqlField(tmp[4], 256, pPatientInfo->szBedNo);
	ConvertSqlField(tmp[5], 256, pPatientInfo->szCardNo);

	char  date_buf[256];
	ConvertDateTime(date_buf, sizeof(date_buf), &pPatientInfo->tInDate);

	if (eType == OPERATION_ADD) {
		_snprintf_s(sql, sizeof(sql), "INSERT INTO %s values('%s','%s','%s',%d, %d, '%s', '%s', '%s', '%s')", PATIENTS_TABLE_NAME, tmp[0], tmp[1], tmp[2], pPatientInfo->bMale, pPatientInfo->nAge,
			tmp[3], tmp[4], date_buf, tmp[5]);
	}
	else if (eType == OPERATION_MODIFY) {
		_snprintf_s(sql, sizeof(sql), "UPDATE %s set InNo='%s', Name='%s', Sex=%d, Age=%d, Office='%s', BedNo='%s', InDate='%s', CardNo='%s' where Id='%s';", PATIENTS_TABLE_NAME, tmp[1], tmp[2], pPatientInfo->bMale, pPatientInfo->nAge,
			tmp[3], tmp[4], date_buf, tmp[5], tmp[0] );
	}
	else {
		assert(0);
	}
	

	/* Execute SQL statement */
	*ret = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
	if (*ret != 0) {
		*ret = ERROR_FAILED_TO_EXECUTE_SQL;
	}
}

int  CMyDatabase::GetAllPatients(std::vector<PatientInfo *> & vPatients) {
	ClearVector(vPatients);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	const char * sql = 0;

	sql = "select * from " PATIENTS_TABLE_NAME " order by InDate desc;";
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	for (int i = 0; i < nrow; i++) {
		PatientInfo * pInfo = new PatientInfo;
		memset(pInfo, 0, sizeof(PatientInfo));

		strncpy_s(pInfo->szId,   azResult[(i+1)*ncolumn +0],    sizeof(pInfo->szId) );
		strncpy_s(pInfo->szInNo, azResult[(i + 1)*ncolumn + 1],  sizeof(pInfo->szInNo));
		strncpy_s(pInfo->szName, azResult[(i + 1)*ncolumn + 2],  sizeof(pInfo->szName));
		sscanf_s(azResult[(i + 1)*ncolumn + 3], "%d", &pInfo->bMale);
		sscanf_s(azResult[(i + 1)*ncolumn + 4], "%d", &pInfo->nAge);
		strncpy_s(pInfo->szOffice, azResult[(i + 1)*ncolumn + 5], sizeof(pInfo->szOffice));
		strncpy_s(pInfo->szBedNo,  azResult[(i + 1)*ncolumn + 6], sizeof(pInfo->szBedNo));
		pInfo->tInDate = ConvertDateTime(azResult[(i + 1)*ncolumn + 7]);
		strncpy_s(pInfo->szCardNo, azResult[(i + 1)*ncolumn + 8], sizeof(pInfo->szCardNo));

		vPatients.push_back(pInfo);
	}
	sqlite3_free_table(azResult);

	return 0;
}

int  CMyDatabase::GetPatient(const char * szId, PatientInfo * pInfo) {
	assert(szId);
	assert(pInfo);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	char  sql[8192];
	char buf[8192];

	_snprintf_s( sql, sizeof(sql), "select * from %s where Id='%s'", PATIENTS_TABLE_NAME, ConvertSqlField(buf, sizeof(buf), szId) );

	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
	if (nrow > 0) {
		int i = 0;
		strncpy_s(pInfo->szId, azResult[(i + 1)*ncolumn + 0], sizeof(pInfo->szId));
		strncpy_s(pInfo->szInNo, azResult[(i + 1)*ncolumn + 1], sizeof(pInfo->szInNo));
		strncpy_s(pInfo->szName, azResult[(i + 1)*ncolumn + 2], sizeof(pInfo->szName));
		sscanf_s(azResult[(i + 1)*ncolumn + 3], "%d", &pInfo->bMale);
		sscanf_s(azResult[(i + 1)*ncolumn + 4], "%d", &pInfo->nAge);
		strncpy_s(pInfo->szOffice, azResult[(i + 1)*ncolumn + 5], sizeof(pInfo->szOffice));
		strncpy_s(pInfo->szBedNo, azResult[(i + 1)*ncolumn + 6], sizeof(pInfo->szBedNo));
		pInfo->tInDate = ConvertDateTime(azResult[(i + 1)*ncolumn + 7]);
		strncpy_s(pInfo->szCardNo, azResult[(i + 1)*ncolumn + 8], sizeof(pInfo->szCardNo));
	}
	sqlite3_free_table(azResult);

	return 0;
}

int  CMyDatabase::DeletePatient(const char * szId) {
	assert(szId);

	char sql[8192];
	char *zErrMsg = 0;

	char tmp[256];
	ConvertSqlField(tmp, 256, szId);

	_snprintf_s(sql, sizeof(sql), "DELETE FROM %s where Id='%s'", PATIENTS_TABLE_NAME, tmp );


	/* Execute SQL statement */
	int ret = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
	if ( ret != 0) {
		ret = ERROR_FAILED_TO_EXECUTE_SQL;
	}
	return ret;
}