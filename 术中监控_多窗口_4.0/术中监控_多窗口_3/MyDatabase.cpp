#include "MyDatabase.h"

#define  DB_NAME                          "surgery3.db"
#define  TEMP_TABLE_NAME                  "temperature"

CMySqliteDatabase::CMySqliteDatabase() {
	m_db = 0;
}

int CMySqliteDatabase::InitDb() {
	int ret = sqlite3_open(DB_NAME, &m_db);
	if ( ret != 0 ) {
		return -1;
	}

	int nrow = 0, ncolumn = 0;    // ��ѯ�����������������
	char **azResult = 0;          // ��ά�����Ž��
	char *zErrMsg = 0;            // ��������
	const char * sql = 0;

	// �鿴Patients���Ƿ���ڣ���������ڣ��򴴽�
	sql = "select name from sqlite_master where name = '" TEMP_TABLE_NAME "';";
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);

	// �����ڣ��򴴽���
	if (0 == nrow) {
		sql = "CREATE TABLE " TEMP_TABLE_NAME "("  \
			"id             INTEGER PRIMARY KEY AUTOINCREMENT," \
			"tag_id         CHAR(16)    NOT NULL," \
			"temp           int         NOT NULL" \
			");";

		ret = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
		if (ret != 0) {
			sqlite3_free_table(azResult);
			return -1;
		}
	}
	sqlite3_free_table(azResult);

	return 0;
}

int CMySqliteDatabase::DeinitDb() {
	return sqlite3_close(m_db);
}