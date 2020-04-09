#include "MyDb.h"

#define  DB_NAME                          "primary_school.db"

#define  CLASSES_TABLE                    "classes"
#define  ROOMS_TABLE                      "rooms"

CMySqliteDatabase::CMySqliteDatabase() {
	m_db = 0;
}

int CMySqliteDatabase::InitDb() {
	int ret = sqlite3_open(DB_NAME, &m_db);
	if (ret != 0) {
		return -1;
	}

	CreateTable( CLASSES_TABLE,
		"id         INTEGER        NOT NULL       PRIMARY KEY" );             // 年级 << 16 | 班级

	CreateTable( ROOMS_TABLE,
		"class_id     INTEGER        NOT NULL," \
		"position     INTEGER        NOT NULL," \
		"name         VARCHAR(16)    NOT NULL," \
		"sex          int            NOT NULL," \
		"tag_id       CHAR(16)       NULL      UNIQUE," \
		"temperature  int            NOT NULL," \
		"time         int            NOT NULL," \
		"PRIMARY KEY(class_id,  position)");

	return 0; 
}

int CMySqliteDatabase::DeinitDb() {
	return sqlite3_close(m_db);
}

// szTableName: 要创建的表名
// szSql: create table (...)，括号里面的内容
BOOL  CMySqliteDatabase::CreateTable(const char * szTableName, const char * szSql) {
	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	//char *zErrMsg = 0;          // 错误描述
	char buf[8192];

	// 查看温度表是否存在，如果不存在，则创建
	SNPRINTF(buf, sizeof(buf), "select name from sqlite_master where name = '%s';", szTableName);
	sqlite3_get_table(m_db, buf, &azResult, &nrow, &ncolumn, 0);

	BOOL  bCreated = FALSE;
	// 表不存在，则创建表
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

BOOL  CMySqliteDatabase::AddClass(const CAddClassParam * pParam) {

	char  szSql[8192];
	DWORD  dwNo = MAKELONG(pParam->m_dwClass, pParam->m_dwGrade);
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE id=%lu", 
		CLASSES_TABLE, dwNo );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	//char *zErrMsg = 0;          // 错误描述
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	if (0 == nrow) {
		SNPRINTF(szSql, sizeof(szSql), "INSERT INTO %s VALUES (%lu) ", CLASSES_TABLE, dwNo);
		sqlite3_exec(m_db, szSql, 0, 0, 0);
		return TRUE;
	}
	// 已经存在
	else {
		return FALSE;
	}


	return FALSE;
}