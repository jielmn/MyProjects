#include "MyDatabase.h"

#define  DB_NAME                          "wearable_monitor.db"
#define  DEVICE_TABLE                     "device"            // 设备，用户名绑定
#define  TEMP_TABLE                       "temperature"
#define  HEART_BEAT_TABLE                 "heartbeat"
#define  BLOOD_OXYGEN_TABLE               "bloodoxygen"

CMySqliteDatabase::CMySqliteDatabase() {
	m_db = 0;
}

int  CMySqliteDatabase::InitDb() {
	int ret = sqlite3_open(DB_NAME, &m_db);
	if (ret != 0) {
		return -1;
	}

	CreateTable(DEVICE_TABLE,
		"device_id  varchar(20)  NOT NULL  PRIMARY KEY," \
		"user_name  varchar(20)  NOT NULL " );		

	CreateTable(TEMP_TABLE,
		"device_id  varchar(20)  NOT NULL  PRIMARY KEY," \
		"temp       int          NOT NULL, " \
		"time       int          NOT NULL  " );

	CreateTable(HEART_BEAT_TABLE,
		"device_id  varchar(20)  NOT NULL  PRIMARY KEY," \
		"heartbeat  int          NOT NULL, " \
		"time       int          NOT NULL  ");

	CreateTable(BLOOD_OXYGEN_TABLE,
		"device_id  varchar(20)  NOT NULL  PRIMARY KEY," \
		"oxy        int          NOT NULL, " \
		"time       int          NOT NULL  ");

	// 删除过时的旧数据
	PruneOldData();

	return 0;
}

int  CMySqliteDatabase::DeinitDb() {
	return sqlite3_close(m_db);
}

BOOL  CMySqliteDatabase::CreateTable(const char * szTableName, const char * szSql) {
	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
								  //char *zErrMsg = 0;            // 错误描述
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

// 删除过时的温度数据，Tag数据
void  CMySqliteDatabase::PruneOldData() {
	char *zErrMsg = 0;
	char szSql[8192];
	time_t today_zero_time = GetTodayZeroTime();
	time_t tOneWeekAgo = today_zero_time - 3600 * 24 * 6;
	int ret = 0;

	SNPRINTF(szSql, sizeof(szSql), "delete from %s where time < %lu", TEMP_TABLE, (DWORD)tOneWeekAgo);
	ret = sqlite3_exec(m_db, szSql, 0, 0, &zErrMsg);
	assert(0 == ret);

	SNPRINTF(szSql, sizeof(szSql), "delete from %s where time < %lu", HEART_BEAT_TABLE, (DWORD)tOneWeekAgo);
	ret = sqlite3_exec(m_db, szSql, 0, 0, &zErrMsg);
	assert(0 == ret);

	SNPRINTF(szSql, sizeof(szSql), "delete from %s where time < %lu", BLOOD_OXYGEN_TABLE, (DWORD)tOneWeekAgo);
	ret = sqlite3_exec(m_db, szSql, 0, 0, &zErrMsg);
	assert(0 == ret);
}

BOOL  CMySqliteDatabase::GetDeviceUserName(const CGetNameParam * pParam, char * szName, DWORD  dwNameSize) {
	assert( szName && dwNameSize > 0);

	char szSql[8192];
	SNPRINTF( szSql, sizeof(szSql), "select * from %s where device_id = '%s' ", DEVICE_TABLE, 
		pParam->m_szDeviceId );

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	DWORD  dwValue = 0;
	BOOL  bRet = FALSE;

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	if (nrow > 0) {
		Utf8ToAnsi(szName, dwNameSize, azResult[ncolumn + 1]);
		bRet = TRUE;
	}
	else {
		szName[0] = '\0';
	}
	sqlite3_free_table(azResult);

	return bRet;
}

void  CMySqliteDatabase::SaveDeviceUserName(const CSaveNameParam * pParam) {
	char sql[8192];
	char szName[256];
	char szTemp[256];

	StrReplaceAll(szTemp, sizeof(szTemp), pParam->m_szName, "'", "''");
	AnsiToUtf8(szName, sizeof(szName), szTemp);

	SNPRINTF(sql, sizeof(sql), "SELECT * FROM %s WHERE device_id='%s' ", DEVICE_TABLE, pParam->m_szDeviceId);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
	sqlite3_get_table(m_db, sql, &azResult, &nrow, &ncolumn, 0);
	// 如果存在
	if (nrow > 0) {
		SNPRINTF(sql, sizeof(sql), "UPDATE %s SET user_name = '%s' WHERE device_id = '%s' ",
			DEVICE_TABLE, szName, pParam->m_szDeviceId);
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
	// 不存在
	else {
		SNPRINTF(sql, sizeof(sql), "INSERT INTO %s values ( '%s', '%s' ) ", DEVICE_TABLE,
			pParam->m_szDeviceId, szName );
		sqlite3_exec(m_db, sql, 0, 0, 0);
	}
	sqlite3_free_table(azResult);
}