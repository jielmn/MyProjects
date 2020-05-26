#include "MyDb.h"
#include <assert.h>

#define  DB_NAME                          "building_temp_monitor.db"

#define  FLOORS_TABLE                     "floors"
#define  DEVICES_TABLE                    "devices"

CMySqliteDatabase::CMySqliteDatabase() {
	m_db = 0;
}

int CMySqliteDatabase::InitDb() {
	int ret = sqlite3_open(DB_NAME, &m_db);
	if (ret != 0) {
		return -1;
	}

	CreateTable(FLOORS_TABLE,
		"id         INTEGER        NOT NULL  PRIMARY KEY");             // 楼层

	CreateTable(DEVICES_TABLE,
		"device_id    INTEGER        NOT NULL," \
		"floor_id     INTEGER        NOT NULL," \
		"address      VARCHAR(48)    NOT NULL," \
		"temperature  int            NOT NULL," \
		"time         int            NOT NULL," \
		"PRIMARY KEY(device_id)");

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

BOOL  CMySqliteDatabase::AddFloor(const CAddFloorParam * pParam) {
	char  szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s WHERE id=%d",
		FLOORS_TABLE, pParam->m_nFloor);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果
								  //char *zErrMsg = 0;          // 错误描述
	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	sqlite3_free_table(azResult);

	if (0 == nrow) {
		SNPRINTF(szSql, sizeof(szSql), "INSERT INTO %s VALUES (%d) ", FLOORS_TABLE, pParam->m_nFloor);
		sqlite3_exec(m_db, szSql, 0, 0, 0);
		return TRUE;
	}

	return FALSE;
}

void  CMySqliteDatabase::GetAllFloors(std::vector<int> & vRet) {
	char  szSql[8192];
	SNPRINTF(szSql, sizeof(szSql), "SELECT * FROM %s", FLOORS_TABLE);

	int nrow = 0, ncolumn = 0;    // 查询结果集的行数、列数
	char **azResult = 0;          // 二维数组存放结果

	sqlite3_get_table(m_db, szSql, &azResult, &nrow, &ncolumn, 0);
	for (int i = 0; i < nrow; i++) {
		int nFloor = GetIntFromDb(azResult[(i + 1)*ncolumn + 0]);
		vRet.push_back(nFloor);
	}
	sqlite3_free_table(azResult);
}

void  CMySqliteDatabase::GetDevicesByFloor( const CGetDevicesParam * pParam, 
	                                        std::vector<DeviceItem*> & vRet ) {



}