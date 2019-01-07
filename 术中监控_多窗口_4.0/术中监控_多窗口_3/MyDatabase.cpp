#include "MyDatabase.h"
#include <time.h>

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
			"temp           int         NOT NULL," \
			"time           int         NOT NULL,"  \
			"remark         varchar(28) NOT NULL" \
			");";

		ret = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
		if (ret != 0) {
			sqlite3_free_table(azResult);
			return -1;
		}
	}
	sqlite3_free_table(azResult);

	// 删除一周前的温度
	char szSql[8192];
	time_t now = time(0);

	struct tm tTmTime;
	localtime_s(&tTmTime, &now);

	tTmTime.tm_hour = 0;
	tTmTime.tm_min  = 0;
	tTmTime.tm_sec  = 0;

	time_t today_zero_time = mktime(&tTmTime);

	// 一周前的开始位置
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;

	// 删除一周前的温度数据
	SNPRINTF(szSql, sizeof(szSql), "delete from temperature where time < %lu", 
		(DWORD)tWeekBegin );
	sqlite3_exec(m_db, szSql, 0, 0, &zErrMsg);

	return 0;
}

int CMySqliteDatabase::DeinitDb() {
	return sqlite3_close(m_db);
}

int  CMySqliteDatabase::SaveTemp(const CSaveTempSqliteParam * pParam) {
	char sql[8192];
	char *zErrMsg = 0;
	int  ret = 0;

	SNPRINTF(sql, sizeof(sql), "insert into temperature values (null, '%s', %lu, %lu, '' )",
		pParam->m_szTagId, pParam->m_dwTemp, (DWORD)pParam->m_time);
	/* Execute SQL statement */
	sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);

	return 0;
}

int  CMySqliteDatabase::QueryTempByTag(const char * szTagId, std::vector<TempData*> & vRet) {
	// 删除一周前的温度
	char szSql[8192];
	time_t now = time(0);

	struct tm tTmTime;
	localtime_s(&tTmTime, &now);

	tTmTime.tm_hour = 0;
	tTmTime.tm_min = 0;
	tTmTime.tm_sec = 0;

	time_t today_zero_time = mktime(&tTmTime);

	// 一周前的开始位置
	time_t tWeekBegin = today_zero_time - 3600 * 24 * 6;

	SNPRINTF(szSql, sizeof(szSql), "select * from temperature where time >= %lu order by time",
		(DWORD)tWeekBegin);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	char *zErrMsg = 0;            // 错误描述
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, &zErrMsg);
	for (int i = 0; i < nrow; i++) {
		TempData * pItem = new TempData;
		memset(pItem, 0, sizeof(TempData));

		DWORD  dw;
		sscanf_s(azResult[(i + 1)*ncolumn + 2], "%lu", &pItem->dwTemperature);
		sscanf_s(azResult[(i + 1)*ncolumn + 3], "%lu", &dw);
		pItem->tTime = (time_t)dw;
		strncpy_s(pItem->szRemark, azResult[(i + 1)*ncolumn + 4], sizeof(pItem->szRemark));

		vRet.push_back(pItem);
	}
	sqlite3_free_table(azResult);

	return 0;
}