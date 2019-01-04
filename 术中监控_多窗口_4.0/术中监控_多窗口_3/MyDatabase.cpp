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

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	const char * sql = 0;

	// 查看Patients表是否存在，如果不存在，则创建
	sql = "select name from sqlite_master where name = '" TEMP_TABLE_NAME "';";
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);

	// 表不存在，则创建表
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