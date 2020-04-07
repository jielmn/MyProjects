#include "MyDatabase.h"
#include <time.h>

#define  DB_NAME                          "bluecheck.db"
#define  READER_TABLE                     "readers"

CMySqliteDatabase::CMySqliteDatabase() {
	m_db = 0;
}

int CMySqliteDatabase::InitDb() {
	int ret = sqlite3_open(DB_NAME, &m_db);
	if (ret != 0) {
		return -1;
	}

	BOOL bRet = CreateTable(READER_TABLE,
		"id         CHAR(16)       PRIMARY KEY," \
		"pass       int            NOT NULL," \
		"failfact   int            NOT NULL," \
		"time       int            NOT NULL" );

	return 0;
}

int CMySqliteDatabase::DeinitDb() {
	return sqlite3_close(m_db);
}

// szTableName: Ҫ�����ı���
// szSql: create table (...)���������������
BOOL  CMySqliteDatabase::CreateTable(const char * szTableName, const char * szSql) {
	int nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;          // ��ά�����Ž��
								  //char *zErrMsg = 0;            // ��������
	char buf[8192];

	// �鿴�¶ȱ��Ƿ���ڣ���������ڣ��򴴽�
	SNPRINTF(buf, sizeof(buf), "select name from sqlite_master where name = '%s';", szTableName);
	sqlite3_get_table(m_db, buf, &azResult, &nrow, &ncolumn, 0);

	BOOL  bCreated = FALSE;
	// �����ڣ��򴴽���
	if (0 == nrow) {
		SNPRINTF(buf, sizeof(buf), "CREATE TABLE %s (%s);", szTableName, szSql);
		int ret = sqlite3_exec(m_db, buf, 0, 0, 0);
		if (ret != 0) {
			sqlite3_free_table(azResult);
			return bCreated;
		}
		bCreated = TRUE;
	}
	sqlite3_free_table(azResult);

	return bCreated;
}

int  CMySqliteDatabase::SaveResult(const CSaveResultParam * pParam) {
	char szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "select * from %s where id='%s'", READER_TABLE, pParam->m_szMac);

	int nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;          // ��ά�����Ž��
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	DWORD now = (DWORD)time(0);
	int ret = 0;

	// ����м�¼
	if (nrow > 0) {
		SNPRINTF(szSql, sizeof(szSql), "UPDATE %s set pass=%d, failfact=%d, time=%lu WHERE id='%s' ",
			READER_TABLE, pParam->m_bPass, pParam->m_dwFact, now, pParam->m_szMac );
		ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
	}
	// û�м�¼ 
	else {
		SNPRINTF(szSql, sizeof(szSql), "INSERT INTO %s VALUES('%s', %d, %d, %lu) ",
			READER_TABLE, pParam->m_szMac, pParam->m_bPass, pParam->m_dwFact, now);
		ret = sqlite3_exec(m_db, szSql, 0, 0, 0);
	}

	return ret;
}

int  CMySqliteDatabase::QueryData(const CQueryDataParam * pParam, std::vector<ReaderItem*> & vRet) {

	char szSql[8192];
	
	DWORD  t1 = (DWORD)pParam->m_t1;
	DWORD  t2 = (DWORD)pParam->m_t2;

	if (pParam->m_szMac[0] == '\0') {
		SNPRINTF(szSql, sizeof(szSql), "select * from %s WHERE time>=%lu AND time<%lu", 
			READER_TABLE, t1, t2 );
	}
	else {
		SNPRINTF(szSql, sizeof(szSql), "select * from %s WHERE time>=%lu AND time<%lu AND id LIKE '%%%s%%' ",
			READER_TABLE, t1, t2, pParam->m_szMac);
	}
	

	int nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;          // ��ά�����Ž��
	DWORD  dwValue = 0;

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);	

	for (int i = 0; i < nrow; i++) {
		ReaderItem * pItem = new ReaderItem;
		memset(pItem, 0, sizeof(ReaderItem));

		GetStrFromdDb(pItem->m_szMac, sizeof(pItem->m_szMac), azResult[(i + 1)*ncolumn + 0]);
		pItem->m_bPass  = GetIntFromDb(azResult[(i + 1)*ncolumn + 1]);
		pItem->m_dwFact = GetIntFromDb(azResult[(i + 1)*ncolumn + 2]);
		pItem->m_time   = (time_t)GetIntFromDb(azResult[(i + 1)*ncolumn + 3]);

		vRet.push_back(pItem);
	}

	sqlite3_free_table(azResult);

	return 0;
}